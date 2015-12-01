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
         *���캯��
         */
        CDBRedis(void);

        /*
         *��������
         */
        ~CDBRedis(void);

        /*
         *INT connDB(CHAR* uid,CHAR* pwd,CHAR* dbsv)
         *CHAR* uid:ָ���û������ַ�ָ��;
         *CHAR* psw:ָ���û�������ַ�ָ;
         *CHAR* dbsv:��Ҫ���ӵ����ݿ������.
         * Ĭ������£�����Ҫ�û������룬ֻ������ָ����IP�Ͷ˿ڼ���
         */
        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);
        INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);

        void disConnDB(void);

        /*
         *INT ExecSQL(CHAR *pcSQLStmt)
         * CHAR* pcSQLStmt����Ϊָ����Ҫִ�е�SQL����ַ�����ָ��
         */
        INT execSQL1(const char* pcSQLStmt);
        INT execSQL2(const char* format, va_list ap);

        INT getRowCount();

        PTSelectResult getSelectResult();

        //����Ĵ��麯�����ò���
        int isSelectStmt(const char* sql) {return 0;};
        INT getNextRow(struct slctRslt** ppstruSlctRslt) {return 0;};
        INT getIntoPara(CHAR* pcParaType,...) {return 0;};
        void beginTrans(BOOL bIsReadOnly = FALSE) {};
        void commit() {};
        void rollback() {};


        /*
         * return: TRUE: �����������������ɹ��� FALSE: ���ӹر�������ʧ��
         */
        virtual BOOL ping();

        const char* type() { return "REDIS"; }
        CDB::DBType  type_i() { return CDB::DB_REDIS; }

    private:
        INT reconnect(); 	//�������ݿ�
        INT connect();	 	//�������ݿ�

        INT analystsResult(redisReply *reply);	//����redis���ؽ��

        redisContext * mRedis;

        //���ݿ�IP�Ͷ˿�
        CStr mDBAddr;
        INT	 mDBPort;

        //���ӳ�ʱʱ��
        struct timeval mTimeout;

        BOOL  alreadyInit; //���ӹ�һ�κ󣬴�ֵ��ΪTRUE.�Ա��ܵ���reconnect

};

#endif
