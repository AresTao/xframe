#ifndef __DBMYSQL_H
#define __DBMYSQL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include "comtypedef.h"
#include "db.h"
class CDBMysql:public CDB
{
    public:   		
        CDBMysql(void);

        ~CDBMysql(void);

        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);

        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);

        void disConnDB(void);
        INT execSQL1(const char* pcSQLStmt);
        INT execSQL2(const char* format, va_list ap);

        int existTable(const char* tableName);
        int existTableField(const char* tableName, const char* fieldName);
        int isSelectStmt(const char* sql);

        INT getNextRow(struct slctRslt** ppstruSlctRslt);

        INT getRowCount();

        PTSelectResult getSelectResult();

	void freeRowMem(struct slctRslt* pstruSlctRslt);
        INT getIntoPara(CHAR* pcParaType,...);

        void beginTrans(BOOL bIsReadOnly = FALSE);

        void commit();        

        void rollback();  	

        virtual BOOL ping();

        const char* type() { return "MYSQL"; }
        CDB::DBType type_i() { return CDB::DB_MYSQL;}
    private:
        INT reconnect(); 

        MYSQL m_struMYSQL;		
        struct DBInfo
        {
            CHAR DBName[256];
            CHAR DBAddr[256];
            INT	 DBPort;
        };
        typedef struct DBInfo DBInfo;
        DBInfo  m_struDBInfo;		

        INT m_iDBCnt;

        MYSQL_RES * m_pMysqlRes; 

        BOOL  alreadyInit; 


};

#endif
