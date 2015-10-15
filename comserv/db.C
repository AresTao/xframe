#include <stdarg.h>
//#ifdef WIN32
//#include <winsock2.h>
//#else
//#include <socket.h>
//#endif

#include "db.h"
#include "dbmysql.h"
#include "dbredis.h"
#include "dbredis_cluster.h"
//#include "dboracle.h"
//#include "dbsqlite.h"

static INT DBConnectionID=0;

CDB::CDB()
{
	m_iDBCnt = 1;
	m_pSelectResult = new TSelectResult(); //整个程序运行期内使用此同一指针.
	m_pSelectResult->pRows = NULL;
	m_pSelectResult	-> rowNum = 0;
	m_bConnFlag = FALSE;
	m_bTransaction = FALSE;

	m_iProcRowCount = 0;

	strcpy(m_pcErrMsg ,"There is no error");
	strcpy(m_pcDBsv,(CHAR* )"");
	strcpy(m_pcUsr,(CHAR* )"");
	strcpy(m_pcPwd,(CHAR* )"");

	mConnectionID=DBConnectionID+1;

	return;
}

CDB::~CDB()
{
	freeResult(); //释放m_pSelectResult内部的rows的内存
	delete m_pSelectResult;
	m_pSelectResult = NULL;
}

INT CDB::execSQL(const char* pcSQLStmt)
{
   timeval execBeginTime;

   int re;

   m_iQueryFlag = 0;
   re = execSQL1(pcSQLStmt);
   return re;
}

INT CDB::execSQLFmt(const char *format, ...)
{
	INT re;
    va_list ap;
    va_start(ap,format);
    re = execSQL2(format,ap);
    va_end(ap);
    return re;
}



// 当应用处理SQL语句失败时调用该函数返回错误信息
// Return Values
//	CHAR* 指向错误信息的指针
//////////////////////////////////////////////////////////
CHAR* CDB::getErrMsg()
{
	return m_pcErrMsg;
}

//////////////////////////////////////////////////////////
//	释放查询结果占用的内存空间
// Args：
//	struct slctRslt* 指向要释放空间的行的行首结构的指针
void CDB::freeRowMem(struct slctRslt* pstruSlctRslt)
{
	struct slctRslt* p = NULL;
	/*
	 *删除pstruSlctRslt指针执行的返回数据行。
	 */
	while (pstruSlctRslt != NULL)
	{
		p = pstruSlctRslt;
		pstruSlctRslt = pstruSlctRslt->pstruNext;
		delete p->pvResult;
		delete p;
		p = NULL;
	}
}

//	释放查询结果占用的内存空间
void CDB::freeResult()
{
	TRow *row = m_pSelectResult->pRows;
	while(row!=NULL)
	{
		TRow *p = row;
		row = row->next;
		delete p;
	}
	m_pSelectResult->pRows=NULL; // 必须置空，否则将出现野指针。下次再调用此函数将出断错误（double free)。
	m_pSelectResult	-> rowNum = 0;
	return;
}

//////////////////////////////////////////////////////////
// 初始化数据库连接
//////////////////////////////////////////////////////////
CDB* initDB(CDB::DBType dbType, const CHAR* dbUser, const CHAR* dbPasswd, const CHAR* dbName, const CHAR* dbHost, INT dbPort)
{
	INT rt = 0;
	CDB* uniDB;
	if(dbType == CDB::DB_MYSQL)
	{
		uniDB = new CDBMysql();
		UniINFO("new DBMySql ok.");
		rt = uniDB->connDB(dbUser, dbPasswd, dbName, dbHost, dbPort);
	}
/*	else if(dbType == CDB::DB_REDIS)
	{
		uniDB = new CDBRedis();
		UniINFO("new DBRedis ok.");
		rt = uniDB->connDB(dbUser, dbPasswd, dbName, dbHost, dbPort);
	}*/
	else if(dbType == CDB::DB_REDIS)
	{
		uniDB = new CDBRedisCluster();
		UniINFO("new DBRedisCluster ok.");
		rt = uniDB->connDB(dbUser, dbPasswd, dbName, dbHost, dbPort);
                if (rt < 0)
                    UniERROR("DBRedisCluster setup error");
	}
	/*else if(dbType == CDB::DB_ORACLE)
	{
		uniDB = new CDBOracle();
		UniINFO(<<"new DBOracle ok.");
		rt = uniDB->connDB(dbUser, dbPasswd, dbName);
	}
	else if(dbType == CDB::DB_SQLITE)
	{
		uniDB = new CDBSqlite();
		UniINFO(<<"new CDBSqlite ok.");
		//added by LXM. 2010-07-01
		CHAR sqliteDbName[100];
		if(dbName!=NULL) //由系统自动生成数据库名，每个数据库连接都不相同。
		{
			sprintf(sqliteDbName,"db.sqlite.%s.%d",dbName,uniDB->getConnectionID());
		}
		rt = uniDB->connDB(dbUser, dbPasswd, sqliteDbName);
	}*/
	else
	{
		UniERROR("DBType %d is not supported yet. The program will exit now...", dbType);
		exit(0);
	}

	if(rt>0)
	{
		UniINFO("Connecting db %s@%s:%s succeed. dbType:%d ", dbName, dbUser, dbHost, dbType);
	}
	else
	{
		UniERROR("Connecting db %s@%s:%s error. dbType:%d, Error info:%s ",dbName, dbUser, dbHost, dbType,
				 uniDB->getErrMsg() );
		return NULL;
	}

	return uniDB;
}


void CDB::printState(CStr& str, int indent)
{
   str.fCat("(%s)", type());
}

Field* CDB::selectOneField(const char* sql)
{
   int re = execSql(sql);
   if(re != 1)
   {
      UniERROR("EXEC SQL: '%s' failed, %s", sql, getErrMsg());
      return NULL;
   }
   TSelectResult* selectResult = getSelectResult();
   if(selectResult == NULL)
   {
      UniERROR("EXEC SQL: '%s' result is NULL", sql);
      return NULL;
   }
   if(selectResult->fieldNum < 1 || selectResult->pRows == NULL)
   {
      return NULL;
   }
   if(selectResult->pRows->arrayField[0].isNull)
   {
      return NULL;
   }
   return &(selectResult->pRows->arrayField[0]);
}

int CDB::count(const char* tableName, const char* condition)
{
   CStr sql;
   sql << "select count(*) from " << tableName;
   if(condition != NULL)
      sql << "  where " << condition;

   struct Field* field = selectOneField(sql.c_str());
   if(field == NULL)
   {
      UniERROR("EXEC SQL: '%s', no valid field selected ", sql.c_str());
      return -1;
   }

   int count;
   switch(field->type)
   {
   case T_INT:
      count = field->value.intValue;
      break;
   case T_UINT:
      count = field->value.uintValue;
      break;
   case T_LONGLONG:
      count = field->value.longlongValue;
      break;
   case T_FLOAT:
      count = (int)field->value.floatValue;
      break;
   default:
      UniERROR("EXEC SQL: '%s' field type is %d", sql.c_str(), field->type);
      return -1;
   }
   return count;
}

int CDB::getFieldInt(struct Field* field, int& result)
{
   switch(field->type)
   {
   case T_INT:
      result = field->value.intValue;
      return 1;
   case T_UINT:
      result = field->value.uintValue;
      return 1;
   case T_LONGLONG:
      result = field->value.longlongValue;
      return 1;
   case T_FLOAT:
      result = (int)field->value.floatValue;
      return 1;
   default:
      return 0;
   }
}

int CDB::selectInt(const char* sql, int& result)
{
   struct Field* field = selectOneField(sql);
   if(field == NULL)
      return 0;
   int re = getFieldInt(field, result);
   if(!re)
      UniERROR("EXEC SQL: '%s' result type is %d, not int", sql, field->type);
   return re;
}

int CDB::selectStr(const char* sql, CStr& result)
{
   struct Field* field = selectOneField(sql);
   if(field == NULL)
      return 0;

   if(field->type == T_STRING)
   {
      result = field->value.stringValue;
      return 1;
   }
   UniERROR("EXEC SQL: '%s' result type is %d, not string", sql, field->type);
   return 0;
}

int CDB::selectIntList(const char* sql, CList<int>& result)
{
   int re = execSql(sql);
   if(re != 1)
   {
      UniERROR("EXEC SQL: '%s' failed, %s", sql, getErrMsg());
      return 0;
   }
   TSelectResult* selectResult = getSelectResult();
   if(selectResult != NULL && selectResult->fieldNum > 0)
   {
      TRow *row = m_pSelectResult->pRows;
      while(row!=NULL)
      {
         struct Field* field = &(row->arrayField[0]);
         int value;
         int re = getFieldInt(field, value);
         if(!re)
         {
            UniERROR("EXEC SQL: '%s' result type is %d, not int", sql, field->type);
            return 0;
         }
         result.insert(value);
         row = row->next;
      }
   }
   return 1;
}

int CDB::selectStrList(const char* sql, CList<CStr>& result)
{
   int re = execSql(sql);
   if(re != 1)
   {
      UniERROR("EXEC SQL: '%s' failed, %s", sql, getErrMsg());
      return 0;
   }
   TSelectResult* selectResult = getSelectResult();
   if(selectResult != NULL && selectResult->fieldNum > 0)
   {
      TRow *row = m_pSelectResult->pRows;
      while(row!=NULL)
      {
         struct Field* field = &(row->arrayField[0]);
         if(field->type != T_STRING)
         {
            UniERROR("EXEC SQL: '%s' result type is %d, not string", sql, field->type);
            return 0;
         }
         CStr value = field->value.stringValue;
         result.insert(value);
         row = row->next;
      }
   }
   return 1;
}
