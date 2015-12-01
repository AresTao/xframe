/****************************************************************
 * FileName			dbredis_cluster.C
 * Author			ztwu
 * Date				2015.08.04
 * Version       		1.0
 ***************************************************************/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include "dbredis_cluster.h"
#include "func.h"
#include "comtypedef_vchar.h"

static const char *UNSUPPORT = "#INFO#SHUTDOWN#MULTI#SLAVEOF#CONFIG#";

static inline std::string to_upper(const std::string& in) {
    std::string out;
    out.reserve( in.size() );
    for(std::size_t i=0; i<in.length(); i++ ) {
        out += char( toupper(in[i]) );       
    }
    return out;
}

CDBRedisCluster::CDBRedisCluster():CDB(), load_slots_asap_(false), errno_(E_OK)
{
    mDBAddr="localhost";
    mDBPort=6379; //use the default port of redis.

    mTimeout={ 1, 500000 }; // 1.5 seconds

    alreadyInit = FALSE;
}

CDBRedisCluster::~CDBRedisCluster()
{
}
INT  CDBRedisCluster::connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv)
{
    if (alreadyInit) { disConnDB(); }

    alreadyInit=FALSE;
    return 0;
}

void CDBRedisCluster::disConnDB()
{
    alreadyInit=FALSE;
}
/////////////////////////////////////////////////////////
// Function	connDB
// Description:
//	init function
//
////////////////////////////////////////////////////////////
INT CDBRedisCluster::connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv, const CHAR* host, INT port)
{
    if ( host == "") mDBAddr="localhost";
    else mDBAddr=host;

    if( port<=0)  mDBPort=6379;
    else mDBPort=port;

    //host string is insist of multiple ip address. Like 127.0.0.1:6379,127.0.0.1:6380 ...
    int res = parse_startup(host);

    UniINFO("Redis cluster nodes size is %d", res);
    if( res < 0 ) {
        UniERROR("redis init error. 'dbhost' in etc/CallServer_config.xml maybe wrong.");
        return -1;
    }
    slots_.resize( HASH_SLOTS );

    if (load_slots_asap_)
    {
        load_slots_cache();
    }

    return 1;
}

std::ostringstream& CDBRedisCluster::set_error(ErrorE e) {
    errno_ = e;
    error_.str("");
    return error_;
}

int CDBRedisCluster::parse_startup(const char *startup) {
    char *p1, *p2, *p3;
    char *tmp = (char *)malloc( strlen(startup)+1 );
    memcpy(tmp, startup, strlen(startup)+1);

    startup_nodes_.clear();

    UniINFO("startup %s" , tmp);
    p1 = p2 = tmp;
    do {
        p2 = strchr(p1, ',');
        if( p2 ) {
            *p2 = '\0';
        }
        p3 = strchr(p1, ':');
        if( p3 ) {

            NodeInfoType node;

            *p3 = '\0';
            node.port = atoi(p3+1); //get port 

            while(p1<p3 && *p1==' ')//trim left
                p1++;

            p3--;
            while(p3>p1 && *p3==' ')//trim right
                *(p3--) = '\0';
            char* host = new char[40];
            strcpy(host, p1);
            node.host = host; //get host
            if( startup_nodes_.insert(node).second ) {
                UniINFO("add parse startup %s %d" , node.host , node.port);
            }
        }
        if( p2 )
            p1 = p2+1;
        else
            break;
    }while(1);

    free( tmp );

    return startup_nodes_.size();
}

int CDBRedisCluster::load_slots_cache() {

    int start, end;
    int count = 0;
    redisReply *reply, *subr, *innr;
    NodePoolType newnodes;

    NodePoolIter citer = startup_nodes_.begin();
    for(; citer!=startup_nodes_.end(); citer++) {

        UniINFO("cache %s %d", citer->host, citer->port);
        redisContext *c = redisConnect(citer->host, citer->port);
        if( c && c->err ) {
            redisFree( c );
            continue;
        }

        reply = (redisReply *)redisCommand(c, "cluster slots");
        if( !reply ) {
            redisFree(c);
            continue;
        } else if( reply->type==REDIS_REPLY_ERROR ) {
            redisFree(c);
            freeReplyObject(reply);
            continue;
        }

        for(size_t i=0; i<reply->elements; i++) {

            subr = reply->element[i];
            if( subr->elements<3
                    || subr->element[0]->type!=REDIS_REPLY_INTEGER
                    || subr->element[1]->type!=REDIS_REPLY_INTEGER 
                    || subr->element[2]->type!=REDIS_REPLY_ARRAY )
                continue;

            start = subr->element[0]->integer;
            end = subr->element[1]->integer;
            innr = subr->element[2];

            if( innr->elements<2 
                    || innr->element[0]->type!=REDIS_REPLY_STRING 
                    || innr->element[1]->type!=REDIS_REPLY_INTEGER )
                continue;

            NodeInfoType node ;
            node.host = innr->element[0]->str;
            node.port = innr->element[1]->integer;

            for(int jj=start; jj<=end; jj++)
                slots_[jj] = node;
            count += (end-start+1);

        }//for i

        redisFree(c);
        freeReplyObject(reply);
        break;

    }//for citer

    if( citer!=startup_nodes_.end() )  {
        UniDEBUG("load_slots_cache count %d from %s:%d" ,count , citer->host , citer->port);
    }
    else {
        UniDEBUG("load_slots_cache fail from all startup node");
    }

    for( citer=newnodes.begin(); citer!=newnodes.end(); citer++ )
        startup_nodes_.insert( *citer );

    return count;
}

int CDBRedisCluster::clear_slots_cache() {
    slots_.clear();
    slots_.resize(HASH_SLOTS);
    return 0;
}

redisContext *CDBRedisCluster::get_random_from_startup(NodeInfoPtr pnode) {
    std::size_t len = startup_nodes_.size();
    if( len==0 )
        return NULL;

    struct timeval tp;
    gettimeofday(&tp, NULL);
    NodePoolCIter node_citer = startup_nodes_.begin();
    std::advance( node_citer, tp.tv_usec % len );//random position

    for( size_t i=0; i<len; i++, node_citer++ ) {

        if( node_citer==startup_nodes_.end() ) //reset
            node_citer = startup_nodes_.begin();

        ConnectionsCIter conn_citer = connections_.find( *node_citer );
        redisContext *c = NULL;

        if( conn_citer==connections_.end() ) {

            c = redisConnect(node_citer->host, node_citer->port);
            if( c && c->err )  { //try next node
                redisFree( c );
                continue;
            }
            *pnode = *node_citer;
            connections_.insert( std::make_pair(*node_citer, c) );

        } else { //connection already exists

            *pnode = conn_citer->first;
            c = (redisContext *)conn_citer->second;

        }
        return c;

    }
    return NULL;
}

uint16_t CDBRedisCluster::get_key_hash(const std::string &key) {
    std::string::size_type pos1, pos2;
    std::string hashing_key = key;

    pos1 = key.find("{");
    if( pos1!=std::string::npos ) {
        pos2 = key.find("}", pos1+1);
        if( pos2!=std::string::npos ) {
            hashing_key = key.substr(pos1+1, pos2-pos1-1);
        }
    }
    return crc16(hashing_key.c_str(), hashing_key.length());
}


INT CDBRedisCluster::execSQL2(const char* format, va_list ap)
{
    uint16_t hashing = get_key_hash(std::string(format));
    int ttl = 5;
    NodeInfoType node;
    redisContext *c = NULL;
    redisReply *reply = NULL;
    bool try_random_node = true;

    if( load_slots_asap_ ) {
        load_slots_asap_ = false;
        load_slots_cache();
    }
    int re;

    int slot = hashing % HASH_SLOTS;
    while( ttl>0 ) {
        ttl--;
        c = NULL;

        if( try_random_node ) {

            //try_random_node = false;
            c = get_random_from_startup(&node);
            if( !c ) {
                return NULL;
            }

        } else {

            node = slots_[0];
            if( node.host == NULL ) { //not hit

                UniDEBUG("slot not hit, try get connection from startup nodes.");
                c = get_random_from_startup(&node);
                if( !c ) {
                    return NULL;
                }

            } else {

                UniDEBUG("slot hit at %s %d " , node.host, node.port);

            }

        }
        if( !c ) {

            ConnectionsCIter citer = connections_.find( node );
            if( citer==connections_.end() ) { 

                UniDEBUG("exesql2 connection dont exists");
                c = redisConnect( node.host, node.port );
                if( c && c->err )  { //next ttl
                    redisFree( c );
                    try_random_node = true;//try random next ttl
                    continue;
                }
                connections_.insert( std::make_pair(node, c) );

            } else {//connection already exists
                UniDEBUG("exesql2 connection exists");
                c = (redisContext *)citer->second;
                break;
            }

        }else
        {
            break;
        }
    }
    reply = (redisReply *)redisvCommand(c, format, ap);
    if( !reply ) 
    {//next ttl

        redisFree( c );
        connections_.erase( node );
        try_random_node = true;//try random next ttl

    } else if( reply->type==REDIS_REPLY_ERROR 
            &&(!strncmp(reply->str,"MOVED",5) || !strcmp(reply->str,"ASK")) ) { //next ttl

        char *p = reply->str, *s; 
        s = strchr(p,' ');      /* MOVED[S]3999 127.0.0.1:6381 */
        p = strchr(s+1,' ');    /* MOVED[S]3999[P]127.0.0.1:6381 */
        *p = '\0';
        slot = atoi(s+1);
        s = strchr(p+1,':');    /* MOVED 3999[P]127.0.0.1[S]6381 */
        *s = '\0';
        slots_[slot].host = p+1;
        slots_[slot].port = atoi(s+1);

        load_slots_asap_ = true;//cluster nodes must have being changed, load slots cache as soon as possible.
        re = 0;
    } else {

        freeResult();   
        re=analystsResult(reply);
        freeReplyObject(reply);

    }

    return re;
}

INT CDBRedisCluster::execSQL1(const char* pcSQLStmt)
{
    uint16_t hashing = get_key_hash(std::string(pcSQLStmt));
    int ttl = 5;
    NodeInfoType node;
    redisContext *c = NULL;
    redisReply *reply = NULL;
    bool try_random_node = true;

    set_error(E_OK); 

    if( load_slots_asap_ ) {
        load_slots_asap_ = false;
        load_slots_cache();
    }
    int re;

    int slot = hashing % HASH_SLOTS;
    UniDEBUG("slot num %d", slot);
    if (strlen(pcSQLStmt) >= MAX_STMT_LEN)
    {
        strcpy(m_pcErrMsg , (CHAR*)"Too Long Statement(beyond 1024 bytes)");
        re = 0;
    }

    while( ttl>0 ) {
        ttl--;
        c = NULL;

        if( try_random_node ) {

            //try_random_node = false;
            c = get_random_from_startup(&node);
            if( !c ) {
                UniDEBUG("try random from startup fail");
                return NULL;
            }

        } else {

            node = slots_[0];
            if( node.host == NULL ) { //not hit

                UniDEBUG("slot not hit, try get connection from startup nodes.");
                c = get_random_from_startup(&node);
                if( !c ) {
                    return NULL;
                }

            } else {

                UniDEBUG("slot hit at " , node.host , node.port);

            }

        }

        if( !c ) {

            ConnectionsCIter citer = connections_.find( node );
            if( citer==connections_.end() ) { 

                UniDEBUG("context dont exists");
                c = redisConnect( node.host, node.port );
                if( c && c->err )  { //next ttl
                    redisFree( c );
                    try_random_node = true;//try random next ttl
                    continue;
                }
                connections_.insert( std::make_pair(node, c) );

            } else {//connection already exists
                UniDEBUG("context exists");
                c = (redisContext *)citer->second;
                break;
            }

        } else {
            break;
        }
    }    

    reply = (redisReply *)redisCommand(c, pcSQLStmt);
    if( !reply ) 
    {//next ttl

        redisFree( c );
        connections_.erase( node );
        try_random_node = true;//try random next ttl

    } else if( reply->type==REDIS_REPLY_ERROR 
            &&(!strncmp(reply->str,"MOVED",5) || !strcmp(reply->str,"ASK")) ) { //next ttl

        char *p = reply->str, *s; 
        s = strchr(p,' ');      /* MOVED[S]3999 127.0.0.1:6381 */
        p = strchr(s+1,' ');    /* MOVED[S]3999[P]127.0.0.1:6381 */
        *p = '\0';
        slot = atoi(s+1);
        s = strchr(p+1,':');    /* MOVED 3999[P]127.0.0.1[S]6381 */
        *s = '\0';
        slots_[slot].host = p+1;
        slots_[slot].port = atoi(s+1);

        load_slots_asap_ = true;//cluster nodes must have being changed, load slots cache as soon as possible.
        re = 0;
    } else {
        freeResult();   
        re=analystsResult(reply);
        freeReplyObject(reply);
    }

    return re;
}

INT CDBRedisCluster::analystsResult(redisReply *reply)
{
    INT re=0;
    if(reply!=NULL)
    {
        if(reply->type == REDIS_REPLY_STATUS)
        {
            //调用的是set，返回set状态
            if(strcasecmp(reply->str,"OK") == 0)
            {
                //返回成功
                m_pSelectResult-> rowNum  = 0;
                re=1;
            }
            else
            {
                m_pSelectResult-> rowNum  = 0;
                strcpy(m_pcErrMsg , (CHAR*)"redisReply.type=REDIS_REPLY_STATUS! rsp!=OK.");
                re=0;
            }
        }
        else if(reply->type == REDIS_REPLY_STRING)
        {
            //返回了一个字符串型值
            TRow    *pRowTemp = new TRow;
            TField  fieldTemp;
            fieldTemp.isNull = FALSE;
            fieldTemp.type = T_STRING;
            strncpy(fieldTemp.value.stringValue,reply->str,reply->len+1);
            pRowTemp->arrayField[0] = fieldTemp;
            m_pSelectResult->pRows = pRowTemp;
            pRowTemp->next = NULL;
            m_pSelectResult-> rowNum  = 1;
            m_pSelectResult-> fieldNum = 1;
            re=1;							
        }
        else if(reply->type == REDIS_REPLY_INTEGER)
        {
            //返回了一个整形值
            TRow    *pRowTemp = new TRow;
            TField  fieldTemp;
            fieldTemp.isNull = FALSE;
            fieldTemp.type = T_LONGLONG;
            fieldTemp.value.longlongValue=reply->integer;
            pRowTemp->arrayField[0] = fieldTemp;
            m_pSelectResult->pRows = pRowTemp;
            pRowTemp->next = NULL;
            m_pSelectResult-> rowNum  = 1;
            m_pSelectResult-> fieldNum = 1;
            re=1;		
        }
        else if(reply->type == REDIS_REPLY_ARRAY)
        {
            TRow    *pRowTemp = NULL;
            TRow	*pRowSwap = NULL;
            int		countNilElement=0;
            //返回数组，认为是有多个返回记录
            for (int i = 0; i < reply->elements; i++) 
            {
                pRowTemp = new TRow;
                if (reply->element[i]->type == REDIS_REPLY_INTEGER )
                {
                    TField  fieldTemp;
                    fieldTemp.isNull = FALSE;
                    fieldTemp.type = T_LONGLONG;
                    fieldTemp.value.longlongValue=reply->element[i]->integer;
                    pRowTemp->arrayField[0] = fieldTemp;

                }
                else if(reply->element[i]->type == REDIS_REPLY_STRING)
                {
                    TField  fieldTemp;
                    fieldTemp.isNull = FALSE;
                    fieldTemp.type = T_STRING;
                    strncpy(fieldTemp.value.stringValue,reply->element[i]->str,reply->element[i]->len+1);
                    pRowTemp->arrayField[0] = fieldTemp;
                }
                else
                {
                    //返回其他，则认为是空值
                    TField  fieldTemp;
                    fieldTemp.isNull = TRUE;
                    fieldTemp.type = T_STRING;
                    fieldTemp.value.stringValue[0]=0;
                    pRowTemp->arrayField[0] = fieldTemp;
                    countNilElement++; //统计空元素个数 by ZhangZeng
                }

                if ( i == 0)
                {
                    m_pSelectResult->pRows = pRowTemp;
                    pRowSwap = pRowTemp;
                    pRowTemp->next = NULL;
                }
                else
                {
                    pRowSwap ->next = pRowTemp;
                    pRowTemp->next  = NULL;
                    pRowSwap = pRowTemp;
                }
            }

            if(countNilElement<reply->elements)
            {
                m_pSelectResult-> rowNum  = reply->elements;
                m_pSelectResult-> fieldNum = 1;
            }
            else //数组元素全部为空则为无效匹配 by ZhangZeng
            {
                m_pSelectResult-> rowNum = 0;
            }
            re=1;
        }
        else if(reply->type == REDIS_REPLY_NIL)
        {
            //返回空，语句无匹配结果
            m_pSelectResult-> rowNum  = 0;
            re=1;
        }
        else if(reply->type == REDIS_REPLY_ERROR)
        {
            //返回失败
            strcpy(m_pcErrMsg , (CHAR*)"redisReply.type=REDIS_REPLY_ERROR!");
            re=0;
        }
        else
        {
            strcpy(m_pcErrMsg , (CHAR*)"redisReply.type=UNKNOWEN!");
            re=0;
        }
    }
    else
    {
        re=0;
    }

    return re;		
}

PTSelectResult CDBRedisCluster::getSelectResult()
{
    return m_pSelectResult;
}

/////////////////////////////////////////////////////////
// Function：	getRowCount
// Description:
// Return Values
//	INT 返回执行语句的行数
//////////////////////////////////////////////////////////
INT CDBRedisCluster::getRowCount()
{
    return m_pSelectResult-> rowNum;
}

BOOL CDBRedisCluster::ping()
{
    if(alreadyInit)
    {
        if(execSQL1("ping"))  return TRUE;
    }

    return FALSE;
}

