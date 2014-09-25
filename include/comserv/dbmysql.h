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
		/*
		 *构造函数
		 */
		CDBMysql(void);

		/*
		 *析构函数
		 */
		~CDBMysql(void);

		/*
		 *INT connDB(CHAR* uid,CHAR* pwd,CHAR* dbsv)
		 *CHAR* uid:指向用户名的字符指针;
		 *CHAR* psw:指向用户口令的字符指;
		 *CHAR* dbsv:所要连接的数据库服务名.
		 */
		INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);

		//--added by Long Xiangming. 2006.11.29
		//当MySQL服务器单独部署时，需要使用远程连接方式
		INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);

		void disConnDB(void);

		/*
		 *INT ExecSQL(CHAR *pcSQLStmt)
		 * CHAR* pcSQLStmt参数为指向所要执行的SQL语句字符串的指针
		 */
		INT execSQL1(const char* pcSQLStmt);

		int existTable(const char* tableName);
		int existTableField(const char* tableName, const char* fieldName);
		int isSelectStmt(const char* sql);

		/*
		 * INT getNextRow(struct slctRslt** ppstruSlctRslt);
		 * struct slctRslt** 输出参数,指向取出数据行的第一个数据单元的指针
		 */

		INT getNextRow(struct slctRslt** ppstruSlctRslt);

		INT getRowCount();

		PTSelectResult getSelectResult();

		/*
		 *void freeRowMem(struct slctRslt* pstruSlctRslt)
		 *struct slctRslt* 指向要释放空间的行的行首结构的指针
		 */
		void freeRowMem(struct slctRslt* pstruSlctRslt);

		/*
		 * INT getIntoPara(CHAR* pcParaType,...);
		 * CHAR* pcParaType 指明pcParaType后面所跟的参数类型.'s'为
		 * CHAR类型,'d'为*INT类型,'u'为*UINT类型,'f'为
		 * *FLOAT类型.例如,GetIntoPara("udsf",uPara,dPara,pcPara,
		 * fPara).串"udsf"说明后面的参数从左到右分别为*UINT类型,
		 * *INT类型,*CHAR类型和*float类型.
		 */
		INT getIntoPara(CHAR* pcParaType,...);

		/*
		 *开始一个事务处理。
		 *bIsReadOnly为TRUE,表示该事务为只读事务。
		 *只读事务可以保证在整个事务过程中数据保持一致性（其他进程
		 *执行commit语句不会对该事务造成影响，但是该事务只能执行查询
		 *语句，而不能对数据库对象进行修改。如果需要修改数据对象，又
		 *要保证数据的一致性，需要执行SQL语句锁定该数据对象。）
		 */
		void beginTrans(BOOL bIsReadOnly = FALSE);

		/*
		 *提交一个事务处理。
		 */
		void commit();        

		void rollback();  	


		virtual BOOL ping();
		const char* type() { return "MYSQL"; }
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
