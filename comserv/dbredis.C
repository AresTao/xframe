/****************************************************************
 * FileName			dbredis.C
 * Author			LiJinglin
 * Date				2014.08.20
 * Version			1.0
 ***************************************************************/
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include "dbredis.h"

#include "comtypedef_vchar.h"


CDBRedis::CDBRedis():CDB()
{
	mDBAddr="localhost";
	mDBPort=6379; //use the default port of redis.
	
	mTimeout={ 1, 500000 }; // 1.5 seconds
	
	alreadyInit = FALSE;
	
	mRedis=NULL;
}

CDBRedis::~CDBRedis()
{
	if(alreadyInit) disConnDB();
}



INT CDBRedis::connect()
{
	redisReply *reply;
	mRedis = redisConnectWithTimeout(mDBAddr.c_str(), mDBPort, mTimeout);
	if (mRedis == NULL || mRedis->err) 
	{
		if (mRedis) 
		{
			UniERROR("Connection Redis server error: %s\n", mRedis->errstr);
			redisFree(mRedis);
		} 
		else 
		{
			UniERROR("Connection  Redis server error: can't allocate redis context\n");
		}
		return 0;
	}
	else
	{
		if(!redisEnableKeepAlive(mRedis))
		{
			UniERROR("Set Redis connection keep alive error");
		}
		return 1;
	}

}

/////////////////////////////////////////////////////////
// Function：	connDB
// Description:
//	执行连接数据库的操作
//
// Args：
//	CHAR* uid:指向用户名的字符指针;
//	CHAR* psw:指向用户口令的字符指;
//	CHAR* dbsv:所要连接的数据库服务名.
// Return Values
//	INT 当返回1时,成功连接数据库;
//		当返回0时,连接失败.
//////////////////////////////////////////////////////////
INT CDBRedis::connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv, const CHAR* host, INT port)
{
	if ( host == "") mDBAddr="localhost";
	else mDBAddr=host;
	
	if( port<=0)  mDBPort=6379;
	else mDBPort=port;
	
	strcpy(m_pcDBsv,dbsv);
	strcpy(m_pcUsr,uid);
	strcpy(m_pcPwd,pwd);
		
	return connDB(uid,pwd,dbsv);

}

INT  CDBRedis::connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv)
{
	if (alreadyInit) { disConnDB(); }
	
	alreadyInit=FALSE;
	int i=3;
	//尝试连接3次
	while(i)
	{
		if(connect())
		{
			alreadyInit=TRUE;
			return 1;
		}		
		i--;
	}
	
	return 0;
}

//重连数据库
INT CDBRedis::reconnect()
{
	if (alreadyInit) { disConnDB(); alreadyInit=FALSE;}
	if(connect())
	{
		alreadyInit=TRUE;
		return 1;
	}		
	return 0;
}

//	断开数据库操作;
void CDBRedis::disConnDB()
{
	if(mRedis!=NULL)
	{
		redisFree(mRedis);
		alreadyInit=FALSE;
		mRedis=NULL;
	}
	else
	{
		alreadyInit=FALSE;
	}
}

/////////////////////////////////////////////////////////
// Function：	execSQL
// Description:
//	对用户提交的SQL语句进行处理.如果是查询语句,将查询结果
//	以链表的形式保存.只有在connDB()函数成功执行之后,才能
//	够执行成功.否则,产生未连接数据库错误.
// Args：
//	CHAR* 指向用户提交的SQL语句的字符指针.
// Return Values
//	INT 当返回1时,表明SQL语句成功执行;
//		当返回0时,表明SQL语句执行失败.
//////////////////////////////////////////////////////////
INT CDBRedis::execSQL2(const char* format, va_list ap)
{
	INT re=0;
	redisReply *reply;
	reply = (redisReply *)redisvCommand(mRedis, format, ap);
	if (reply)
	{
		freeResult();	//先清理掉上一次的查询结果
		re=analystsResult(reply);
		freeReplyObject(reply);
	}
	else
	{
		//add by LJL. 20140924 如果连接失败，判断返回信息，重新连接并执行查询语句
		if(mRedis->err)
		{
			//返回空，且RedisContext上下文报错，尝试重新连接
			if(reconnect())
			{
				reply = (redisReply *)redisvCommand(mRedis, format, ap);
				if (reply)
				{
					freeResult();	//先清理掉上一次的查询结果
					re=analystsResult(reply);
					freeReplyObject(reply);
				}
				else
				{
					sprintf(m_pcErrMsg , "reconnect OK, but redisReply=NULL, redisContext.err=%d:%s",mRedis->err, mRedis->errstr);
					re=0;
				}
			}
			else
			{
				sprintf(m_pcErrMsg , "reconnect ERROR! redisContext.err=%d:%s",mRedis->err, mRedis->errstr);
				re=0;
			}
		}
		else
		{
			//返回空，但没报错
			strcpy(m_pcErrMsg , (CHAR*)"redisReply==NULL, redisContext.err=0!");
			re=0;
		}
	}

   return re;
}


INT CDBRedis::execSQL1(const char* pcSQLStmt)
{
	int re;
	//如果SQL语句过长，直接返回错误信息。
	if (strlen(pcSQLStmt) >= MAX_STMT_LEN)
    {
    	strcpy(m_pcErrMsg , (CHAR*)"Too Long Statement(beyond 1024 bytes)");
		re = 0;
    }

    //如果没有连接到数据库，直接返回错误信息。
   else if (!alreadyInit)
    {
    	strcpy(m_pcErrMsg , (CHAR*)"Have not connected to the database or the connection lost now!");
		UniERROR(m_pcErrMsg);
		re = 0;
    }
	
	redisReply *reply;
	reply = (redisReply *)redisCommand(mRedis, pcSQLStmt);
	if (reply)
	{
		freeResult();	//先清理掉上一次的查询结果
		re=analystsResult(reply);
		freeReplyObject(reply);
	}
	else
	{
	
		//#define REDIS_ERR_IO 1 /* Error in read or write */
		//#define REDIS_ERR_EOF 3 /* End of file */
		//#define REDIS_ERR_PROTOCOL 4 /* Protocol error */
		//#define REDIS_ERR_OOM 5 /* Out of memory */
		//#define REDIS_ERR_OTHER 2 /* Everything else... */
		//add by LJL. 20140924 如果连接失败，判断返回信息，重新连接并执行查询语句
		if(mRedis->err)
		{
			//返回空，且RedisContext上下文报错，尝试重新连接
			if(reconnect())
			{
				reply = (redisReply *)redisCommand(mRedis, pcSQLStmt);
				if (reply)
				{
					freeResult();	//先清理掉上一次的查询结果
					re=analystsResult(reply);
					freeReplyObject(reply);
				}
				else
				{
					sprintf(m_pcErrMsg , "reconnect OK, but redisReply=NULL, redisContext.err=%d:%s",mRedis->err, mRedis->errstr);
					re=0;
				}
			}
			else
			{
				sprintf(m_pcErrMsg , "reconnect ERROR! redisContext.err=%d:%s",mRedis->err, mRedis->errstr);
				re=0;
			}
		}
		else
		{
			//返回空，但没报错
			strcpy(m_pcErrMsg , (CHAR*)"redisReply==NULL, redisContext.err=0!");
			re=0;
		}
	}

   return re;
}


INT CDBRedis::analystsResult(redisReply *reply)
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


PTSelectResult CDBRedis::getSelectResult()
{
       return m_pSelectResult;
}


/////////////////////////////////////////////////////////
// Function：	getRowCount
// Description:
// Return Values
//	INT 返回执行语句的行数
//////////////////////////////////////////////////////////
INT CDBRedis::getRowCount()
{
   return m_pSelectResult-> rowNum;
}


//检查到数据库的连接是否正常。如果关闭，试图重连接
BOOL CDBRedis::ping()
{
	if(alreadyInit)
	{
		if(execSQL1("ping"))  return TRUE;
	}
	
	return FALSE;
}

