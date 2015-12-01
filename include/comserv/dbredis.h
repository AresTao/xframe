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
        /*
         *构造函数
         */
        CDBRedis(void);

        /*
         *析构函数
         */
        ~CDBRedis(void);

        /*
         *INT connDB(CHAR* uid,CHAR* pwd,CHAR* dbsv)
         *CHAR* uid:指向用户名的字符指针;
         *CHAR* psw:指向用户口令的字符指;
         *CHAR* dbsv:所要连接的数据库服务名.
         * 默认情况下，不需要用户名密码，只需连接指定的IP和端口即可
         */
        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);
        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);

        void disConnDB(void);

        /*
         *INT ExecSQL(CHAR *pcSQLStmt)
         * CHAR* pcSQLStmt参数为指向所要执行的SQL语句字符串的指针
         */
        INT execSQL1(const char* pcSQLStmt);
        INT execSQL2(const char* format, va_list ap);

        INT getRowCount();

        PTSelectResult getSelectResult();

        //基类的纯虚函数，用不到
        int isSelectStmt(const char* sql) {return 0;};
        INT getNextRow(struct slctRslt** ppstruSlctRslt) {return 0;};
        INT getIntoPara(CHAR* pcParaType,...) {return 0;};
        void beginTrans(BOOL bIsReadOnly = FALSE) {};
        void commit() {};
        void rollback() {};


        /*
         * return: TRUE: 连接正常或者重连成功； FALSE: 连接关闭且重连失败
         */
        virtual BOOL ping();

        const char* type() { return "REDIS"; }
        CDB::DBType  type_i() { return CDB::DB_REDIS; }

    private:
        INT reconnect(); 	//重连数据库
        INT connect();	 	//连接数据库

        INT analystsResult(redisReply *reply);	//分析redis返回结果

        redisContext * mRedis;

        //数据库IP和端口
        CStr mDBAddr;
        INT	 mDBPort;

        //连接超时时间
        struct timeval mTimeout;

        BOOL  alreadyInit; //连接过一次后，此值置为TRUE.以便能调用reconnect

};

#endif
