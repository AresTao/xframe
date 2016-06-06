#ifndef __DBREDISCLUSTER_H
#define __DBREDISCLUSTER_H
#include <stdio.h>
#include <vector>
#include <sstream>
#include <set>
#include <unordered_map>
#include <functional>
#include <string.h>
#include <stdlib.h>
#include "hiredis.h"
#include "comtypedef.h"
#include "db.h"

class CDBRedisCluster:public CDB
{
public:
        const static int HASH_SLOTS = 16384;

        enum ErrorE {
            E_OK = 0, 
            E_COMMANDS = 1,
            E_SLOT_MISSED = 2,
            E_IO = 3,
            E_TTL = 4,
            E_OTHERS = 5 
        };

        typedef struct NodeInfoS{

            char * host;
            int port;
            bool operator==(const struct NodeInfoS &other) const {
                return (host==other.host && port==other.port);
            }

            bool operator<(const struct NodeInfoS &rs) const {
                if( host<rs.host )
                    return true;
                else
                    return port<rs.port;
            }
        }NodeInfoType, *NodeInfoPtr, &NodeInfoRef;   	
        
        struct KeyHasherS {
            std::size_t operator()(const NodeInfoType &node) const {
                return (std::hash<std::string>()(node.host) ^ std::hash<int>()(node.port));
            }
        };

        typedef std::unordered_map<NodeInfoType, void *, KeyHasherS> ConnectionsType;//NodeInfoType=>redisContext
        typedef ConnectionsType::iterator ConnectionsIter;
        typedef ConnectionsType::const_iterator ConnectionsCIter;

        typedef std::set<NodeInfoType> NodePoolType;
        typedef NodePoolType::iterator NodePoolIter;
        typedef NodePoolType::const_iterator NodePoolCIter;

        inline int err() const { return errno_; }
        inline std::string strerr() const { return error_.str(); } 	
   	
        
        CDBRedisCluster(void);
   		 
        ~CDBRedisCluster(void);

	int setUp(const char *startup, bool lazy);
	INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);
         
 	INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);
        void disConnDB(void);
	INT execSQL1(const char* pcSQLStmt);
	INT execSQL2(const char* format, va_list ap);

        INT getRowCount();

	PTSelectResult getSelectResult();

	int isSelectStmt(const char* sql) {return 0;};
	INT getNextRow(struct slctRslt** ppstruSlctRslt) {return 0;};
	INT getIntoPara(CHAR* pcParaType,...) {return 0;};
	void beginTrans(BOOL bIsReadOnly = FALSE) {};
        void commit() {};
        void rollback() {};
		
	virtual BOOL ping();

	const char* type() { return "REDISCLUSTER"; }
	CDB::DBType  type_i() { return CDB::DB_REDIS; }
		
private:
	std::ostringstream& set_error(ErrorE e);
        int parse_startup(const char *startup);
        int load_slots_cache();
        int clear_slots_cache();
        redisContext *get_random_from_startup(NodeInfoPtr pnode); 
        uint16_t get_key_hash(const std::string &key);
        redisReply* redis_command_argv(const std::string& key, int argc, const char **argv, const size_t *argvlen);

        NodePoolType startup_nodes_;
        ConnectionsType connections_;
        std::vector<NodeInfoType> slots_;
        bool load_slots_asap_;

        ErrorE errno_;
        std::ostringstream error_;

        INT reconnect(); 	
	INT connect();	 	
		
	INT analystsResult(redisReply *reply);	

	redisContext * mRedis;
		
	CStr mDBAddr;
	INT	 mDBPort;
		
	struct timeval mTimeout;
		
	BOOL  alreadyInit; 
	 
};

#endif
