#ifndef __DBREDIS_H
#define __DBREDIS_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hiredis.h"
#include "comtypedef.h"
#include "db.h"

class CDBRedis:public CDB
{
    public:   		
        CDBRedis(void);

        ~CDBRedis(void);

        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);
        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);

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

        const char* type() { return "REDIS"; }
        CDB::DBType  type_i() { return CDB::DB_REDIS; }

    private:
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
