/****************************************************************
 * FileName��			dbredis.C
 * Author��				LiJinglin
 * Date��				2014.08.20
 * Version��			1.0
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
// Function��	connDB
// Description:
//	ִ���������ݿ�Ĳ���
//
// Args��
//	CHAR* uid:ָ���û������ַ�ָ��;
//	CHAR* psw:ָ���û�������ַ�ָ;
//	CHAR* dbsv:��Ҫ���ӵ����ݿ������.
// Return Values
//	INT ������1ʱ,�ɹ��������ݿ�;
//		������0ʱ,����ʧ��.
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
	//��������3��
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

//�������ݿ�
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

//	�Ͽ����ݿ����;
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
// Function��	execSQL
// Description:
//	���û��ύ��SQL�����д���.����ǲ�ѯ���,����ѯ���
//	����������ʽ����.ֻ����connDB()�����ɹ�ִ��֮��,����
//	��ִ�гɹ�.����,����δ�������ݿ����.
// Args��
//	CHAR* ָ���û��ύ��SQL�����ַ�ָ��.
// Return Values
//	INT ������1ʱ,����SQL���ɹ�ִ��;
//		������0ʱ,����SQL���ִ��ʧ��.
//////////////////////////////////////////////////////////
INT CDBRedis::execSQL2(const char* format, va_list ap)
{
	INT re=0;
	redisReply *reply;
	reply = (redisReply *)redisvCommand(mRedis, format, ap);
	if (reply)
	{
		freeResult();	//����������һ�εĲ�ѯ���
		re=analystsResult(reply);
		freeReplyObject(reply);
	}
	else
	{
		//add by LJL. 20140924 �������ʧ�ܣ��жϷ�����Ϣ���������Ӳ�ִ�в�ѯ���
		if(mRedis->err)
		{
			//���ؿգ���RedisContext�����ı�����������������
			if(reconnect())
			{
				reply = (redisReply *)redisvCommand(mRedis, format, ap);
				if (reply)
				{
					freeResult();	//����������һ�εĲ�ѯ���
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
			//���ؿգ���û����
			strcpy(m_pcErrMsg , (CHAR*)"redisReply==NULL, redisContext.err=0!");
			re=0;
		}
	}

   return re;
}


INT CDBRedis::execSQL1(const char* pcSQLStmt)
{
	int re;
	//���SQL��������ֱ�ӷ��ش�����Ϣ��
	if (strlen(pcSQLStmt) >= MAX_STMT_LEN)
    {
    	strcpy(m_pcErrMsg , (CHAR*)"Too Long Statement(beyond 1024 bytes)");
		re = 0;
    }

    //���û�����ӵ����ݿ⣬ֱ�ӷ��ش�����Ϣ��
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
		freeResult();	//����������һ�εĲ�ѯ���
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
		//add by LJL. 20140924 �������ʧ�ܣ��жϷ�����Ϣ���������Ӳ�ִ�в�ѯ���
		if(mRedis->err)
		{
			//���ؿգ���RedisContext�����ı�����������������
			if(reconnect())
			{
				reply = (redisReply *)redisCommand(mRedis, pcSQLStmt);
				if (reply)
				{
					freeResult();	//����������һ�εĲ�ѯ���
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
			//���ؿգ���û����
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
			//���õ���set������set״̬
			if(strcasecmp(reply->str,"OK") == 0)
			{
				//���سɹ�
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
			//������һ���ַ�����ֵ
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
			//������һ������ֵ
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
				//�������飬��Ϊ���ж�����ؼ�¼
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
					//��������������Ϊ�ǿ�ֵ
					TField  fieldTemp;
					fieldTemp.isNull = TRUE;
					fieldTemp.type = T_STRING;
					fieldTemp.value.stringValue[0]=0;
					pRowTemp->arrayField[0] = fieldTemp;
					countNilElement++; //ͳ�ƿ�Ԫ�ظ��� by ZhangZeng
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
			else //����Ԫ��ȫ��Ϊ����Ϊ��Чƥ�� by ZhangZeng
			{
				m_pSelectResult-> rowNum = 0;
			}
			re=1;
		}
		else if(reply->type == REDIS_REPLY_NIL)
		{
			//���ؿգ������ƥ����
			m_pSelectResult-> rowNum  = 0;
			re=1;
		}
		else if(reply->type == REDIS_REPLY_ERROR)
		{
			//����ʧ��
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
// Function��	getRowCount
// Description:
// Return Values
//	INT ����ִ����������
//////////////////////////////////////////////////////////
INT CDBRedis::getRowCount()
{
   return m_pSelectResult-> rowNum;
}


//��鵽���ݿ�������Ƿ�����������رգ���ͼ������
BOOL CDBRedis::ping()
{
	if(alreadyInit)
	{
		if(execSQL1("ping"))  return TRUE;
	}
	
	return FALSE;
}
