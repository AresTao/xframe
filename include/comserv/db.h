#ifndef __DB_H
#define __DB_H

#include "comtypedef.h"
#include "info.h"

const INT  MaxSQLLength			= 8192;

#define DEFAULT_DB			(CHAR*)"unidb"//ȱʡ���ݿ�SID
#define MAX_ITEMS         	100			//�ڲ�ѯ����п���һ�β�ѯ���������
#define MAX_VNAME_LEN     	4096		//ÿ���ֶε����bytes
#define MAX_INAME_LEN     	4096		//ѡ�������������bytes
#define NULLBASE			10			//GetIntoPara()������ȡ����ֵʱ������ֵ��ӵĻ�����
//���磬�����4����ֵ����ô����ֵ��ΪNULLBASE+4;
#define MAX_STMT_LEN		MaxSQLLength	//����SQL������󳤶ȡ�
#define MAX_UID_LEN			128			//���ݿ��û��������bytes
#define MAX_DBSV_LEN		128			//���ݿ�����������bytes
#define MAX_PWD_LEN			32			//���ݿ��������bytes
#define MAX_THREAD_LNK		3			//���ݿ���������߳���

/*
 *��������������͵����ֵ���ڴ����ݿ����ȡ��������
 *������ڸ��������͵����ֵʱ����Ϊ������Ϊ��ֵ����
 *���GetIntoPara()�������ԣ�
 */
#ifndef UINT_MAX
#define UINT_MAX			4294967295	//UINT�������ݵĿ�ֵ��ʾ
#endif
#ifndef CHAR_MAX
#define CHAR_MAX			127			//CHAR�������ݵĿ�ֵ��ʾ
#endif
#ifndef BYTE_MAX
#define BYTE_MAX			255			//BYTE�������ݵĿ�ֵ��ʾ
#endif
#ifndef SHRT_MAX
#define SHRT_MAX			32767		//SHORT�������ݵĿ�ֵ��ʾ
#endif
#ifndef USHRT_MAX
#define USHRT_MAX			65535		//USHORT�������ݵĿ�ֵ��ʾ
#endif
#ifndef INT_MAX
#define INT_MAX				2147483647	//INT�������ݵĿ�ֵ��ʾ
#endif
#ifndef FLT_MAX
#define FLT_MAX				3.402823466e+38F	//FLOAT�������ݵĿ�ֵ��ʾ
#endif

enum TFieldDataType
{
    T_INT,
    T_UINT,
    T_LONGLONG,
    T_FLOAT,
    T_STRING
};

typedef struct Field
{
    TFieldDataType	type;
    BOOL    		isNull;
    union
    {
        I32			    intValue;
        UINT            uintValue;
        I64 			longlongValue;
        FLOAT           floatValue;
#define MaxStrAIDLength 4000
        char			stringValue[MaxStrAIDLength];
    }value;

    Field()
    {
        isNull = FALSE;
        value.intValue = 0;
        value.stringValue[0]=0; //added by lxm.2008.12.08
    }
}TField;

typedef struct FieldInfo
{
    char			 name[20];
    TFieldDataType	 type;
    int 			 len;
}TFieldInfo;

const I32 MaxFieldNum = 80;//20;
typedef struct Row
{
    struct Field	 arrayField[MaxFieldNum];
    struct Row		 *next;
}TRow;

_CLASSDEF(TSelectResult);
struct TSelectResult
{
    int 			 rowNum; //modify short to int. by Long Xiangming. 2007.11.16
    short  			 fieldNum;
    struct Row		 *pRows;
    struct FieldInfo fieldInfos[MaxFieldNum];
};

// SelectResult �� slctRslt ����ͬ. �����Ҫ����SelectResult.
// slctRsltֻ��Ϊ������ǰ������(UniNet 3.X ��ǰ������, ����𽥷�����

struct slctRslt
{
    void* pvResult;						//��ѯ�����Ԫ
    INT ResultLen;						//�������
    struct slctRslt* pstruNext;			//��һ��
};

_CLASSDEF(CDB);
class CDB
{
    public:
        //move to comtypedef.h
        enum DBType
        {
            DB_MYSQL = 0,
            DB_ORACLE = 1,
            DB_SQLITE = 2,
            DB_REDIS = 3,
            DB_REDIS_CLUSTER = 4
        };

        CDB(void);
        INT getConnectionID() {return mConnectionID;};

        virtual ~CDB(void);
        /*
         *INT ConnDB(CHAR* uid,CHAR* pwd,CHAR* dbsv)
         *CHAR* uid:ָ���û������ַ�ָ��;
         *CHAR* psw:ָ���û�������ַ�ָ;
         *CHAR* dbsv:��Ҫ���ӵ����ݿ������.
         */
        virtual INT connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB)=0;
        //host: ���ݿ������������ַ�� port: ���ݿ�������˿ں�
        //�˷����������ݿ��������������Ϊһ̨����ʱ��
        //MySQL������Ҫʹ�����ַ�ʽ��(Oracle��ʹ��������ͨ�������ļ�db���ָ��ΪԶ�����ӣ�������connDB����ʽָ��host)
        virtual INT connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port)=0;

        virtual void disConnDB(void)=0;

        /************************************************
         *INT ExecSQL(CHAR *pcSQLStmt)
         * CHAR* pcSQLStmt����Ϊָ����Ҫִ�е�SQL����ַ�����ָ��
         *************************************************/
        virtual INT execSQLFmt(const char *, ...);
        virtual INT execSQL(const char *pcSQLStmt);

        virtual INT execSQL1(const char* pcSQLStmt)=0;
        virtual INT execSQL2(const char* format, va_list ap)=0;

        virtual int execSql(const char* sql) { return execSQL((char*)sql); }
        virtual int existTable(const char* tableName) { return 1; };
        virtual int existTableField(const char* tableName, const char* fieldName) { return 1; };
        virtual int isSelectStmt(const char* sql) = 0;
        int count(const char* tableName, const char* condition = NULL);
        int selectInt(const char* sql, int& result);
        int selectStr(const char* sql, CStr& result);
        int selectIntList(const char* sql, CList<int>& result);
        int selectStrList(const char* sql, CList<CStr>& result);
        /********************************************
         *INT GetNextRow(struct slctRslt** ppstruSlctRslt);
         *struct slctRslt** �������,ָ��ȡ�������еĵ�һ�����ݵ�Ԫ��ָ��
         *************************************************/
        virtual INT getNextRow(struct slctRslt** ppstruSlctRslt)=0;

        /* ��ȡ�����е�����  */
        virtual INT getRowCount()=0;

        /* ��ȡ����ֵ */
        virtual PTSelectResult getSelectResult()=0;

        /* ��ȡִ�н���Ĵ�����ʾ */
        virtual CHAR* getErrMsg();

        /*
         *void FreeRowMem(struct slctRslt* pstruSlctRslt)
         *struct slctRslt* ָ��Ҫ�ͷſռ���е����׽ṹ��ָ��
         */
        virtual void freeRowMem(struct slctRslt* pstruSlctRslt);

        /*
         * INT GetIntoPara(CHAR* pcParaType,...);
         * CHAR* pcParaType ָ��pcParaType���������Ĳ�������.'s'Ϊ
         * CHAR����,'d'Ϊ*INT����,'u'Ϊ*UINT����,'f'Ϊ
         * *FLOAT����.����,GetIntoPara("udsf",uPara,dPara,pcPara,
         * fPara).��"udsf"˵������Ĳ��������ҷֱ�Ϊ*UINT����,
         * *INT����,*CHAR���ͺ�*float����.
         */
        virtual INT getIntoPara(CHAR* pcParaType,...)=0;

        /*
         *��ʼһ��������
         *bIsReadOnlyΪTRUE,��ʾ������Ϊֻ������
         *ֻ��������Ա�֤������������������ݱ���һ���ԣ���������
         *ִ��commit��䲻��Ը��������Ӱ�죬���Ǹ�����ֻ��ִ�в�ѯ
         *��䣬�����ܶ����ݿ��������޸ġ������Ҫ�޸����ݶ�����
         *Ҫ��֤���ݵ�һ���ԣ���Ҫִ��SQL������������ݶ��󡣣�
         */
        virtual void beginTrans(BOOL bIsReadOnly = FALSE)=0;

        /*
         *�ύһ��������
         */
        virtual void commit()=0;

        /*
         *�ع�һ��������
         */
        virtual void rollback()=0;

        /*
         * ��鵽�������������Ƿ����ڹ�����������ر��ˣ��Զ�����һ�������ӡ�

         * ������кܳ�ʱ�䲻ʹ�����ݿ⣬���ݿ���������ܽ����ӹرա�
         * �����ڳ�ʱ������ʱ����Ҫ���������Ƿ�ر������Ӳ������б�Ҫ�������ӡ�
         * ��UniFrame�У��˺�����Kernel�Զ����ã��ϲ�Ӧ��������ô˺�����

         * return: TRUE: �����������������ɹ��� FALSE: ���ӹر�������ʧ��
         */
        virtual BOOL ping()=0;
        virtual const char* type() = 0;
        virtual CDB::DBType type_i() =0;
        virtual void printState(CStr& str, int indent);

    protected:
        void freeResult(); 	//�ͷ�m_pSelectResult��pRows���ڴ�
        //�˷�����ÿ�β�ѯǰ���Զ�����
        INT mConnectionID;
        I32  m_iDBCnt;
        CHAR m_pcDBsv[256];
        CHAR m_pcUsr[256];
        CHAR m_pcPwd[256];

        CHAR m_pcSQLStmt[MaxSQLLength];
        INT  m_iProcRowCount;					//�������������ۼƴ�������
        CHAR m_pcErrMsg[255];					//���ڱ��������Ϣ
        BOOL m_bTransaction;					//��־�Ƿ�Ϊ������
        BOOL m_bConnFlag;						//��־���ݿ�����״̬
        INT  m_iQueryFlag;

        PTSelectResult m_pSelectResult;
        BOOL m_isFirstlyGetResult;              //ÿ�β�ѯ����true

        Field* selectOneField(const char* sql);
        int getFieldInt(Field* field, int& value);
};
/* ��ʼ�����ݿ�
 *  ���������
 *		DBType dbType
 *		CHAR* dbUser  ���ݿ��û���
 *		CHAR* dbPasswd ���ݿ��û�����
 *		CHAR* dbName   ���ݿ�����sid��oracle����sqlite�Ǳ������ݿ�ǰ׺�����������ݿ��������Ϊÿ�����ӽ���һ�������Ŀ⣩
 *		CHAR* dbHost��INT dbPort   ���ݿ��ַ + �˿ڣ�һ��MySQLʹ�ã�Oracle��Ҫ��װ�ͻ�������sid, SQLite�Ǳ������ݿ⣩
 *******************************************************/
extern CDB* initDB(CDB::DBType dbType, const CHAR* dbUser, const CHAR* dbPasswd, const CHAR* dbName, const CHAR* dbHost=NULL, INT dbPort=0);

#endif

