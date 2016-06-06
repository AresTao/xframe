#ifndef __DB_H
#define __DB_H

#include "comtypedef.h"
#include "info.h"

const INT  MaxSQLLength		= 8192;

#define DEFAULT_DB		(CHAR*)"unidb"
#define MAX_ITEMS         	100			
#define MAX_VNAME_LEN     	4096		
#define MAX_INAME_LEN     	4096		
#define NULLBASE		10			
#define MAX_STMT_LEN		MaxSQLLength	
#define MAX_UID_LEN		128	
#define MAX_DBSV_LEN		128		
#define MAX_PWD_LEN		32	
#define MAX_THREAD_LNK		3		

#ifndef UINT_MAX
#define UINT_MAX			4294967295	
#endif
#ifndef CHAR_MAX
#define CHAR_MAX			127		
#endif
#ifndef BYTE_MAX
#define BYTE_MAX			255		
#endif
#ifndef SHRT_MAX
#define SHRT_MAX			32767		
#endif
#ifndef USHRT_MAX
#define USHRT_MAX			65535		
#endif
#ifndef INT_MAX
#define INT_MAX				2147483647	
#endif
#ifndef FLT_MAX
#define FLT_MAX				3.402823466e+38F	
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
        value.stringValue[0]=0; 
    }
}TField;

typedef struct FieldInfo
{
    char			 name[20];
    TFieldDataType	 type;
    int 			 len;
}TFieldInfo;

const I32 MaxFieldNum = 80;
typedef struct Row
{
    struct Field	 arrayField[MaxFieldNum];
    struct Row		 *next;
}TRow;

_CLASSDEF(TSelectResult);
struct TSelectResult
{
    int 			 rowNum; 
    short  			 fieldNum;
    struct Row		 *pRows;
    struct FieldInfo fieldInfos[MaxFieldNum];
};


struct slctRslt
{
    void* pvResult;						
    INT ResultLen;						
    struct slctRslt* pstruNext;		
};

_CLASSDEF(CDB);
class CDB
{
    public:
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
        virtual INT connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB)=0;
        virtual INT connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port)=0;

        virtual void disConnDB(void)=0;

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
        virtual INT getNextRow(struct slctRslt** ppstruSlctRslt)=0;

        virtual INT getRowCount()=0;

        virtual PTSelectResult getSelectResult()=0;

        virtual CHAR* getErrMsg();

	virtual void freeRowMem(struct slctRslt* pstruSlctRslt);

	virtual INT getIntoPara(CHAR* pcParaType,...)=0;
        virtual void beginTrans(BOOL bIsReadOnly = FALSE)=0;

        virtual void commit()=0;

        virtual void rollback()=0;
        virtual BOOL ping()=0;
        virtual const char* type() = 0;
        virtual CDB::DBType type_i() =0;
        virtual void printState(CStr& str, int indent);

    protected:
        void freeResult(); 	
        INT mConnectionID;
        I32  m_iDBCnt;
        CHAR m_pcDBsv[256];
        CHAR m_pcUsr[256];
        CHAR m_pcPwd[256];

        CHAR m_pcSQLStmt[MaxSQLLength];
        INT  m_iProcRowCount;					
        CHAR m_pcErrMsg[255];					
        BOOL m_bTransaction;					
        BOOL m_bConnFlag;					
        INT  m_iQueryFlag;

        PTSelectResult m_pSelectResult;
        BOOL m_isFirstlyGetResult;              

        Field* selectOneField(const char* sql);
        int getFieldInt(Field* field, int& value);
};
extern CDB* initDB(CDB::DBType dbType, const CHAR* dbUser, const CHAR* dbPasswd, const CHAR* dbName, const CHAR* dbHost=NULL, INT dbPort=0);

#endif

