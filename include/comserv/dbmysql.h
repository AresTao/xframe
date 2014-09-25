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
		 *���캯��
		 */
		CDBMysql(void);

		/*
		 *��������
		 */
		~CDBMysql(void);

		/*
		 *INT connDB(CHAR* uid,CHAR* pwd,CHAR* dbsv)
		 *CHAR* uid:ָ���û������ַ�ָ��;
		 *CHAR* psw:ָ���û�������ַ�ָ;
		 *CHAR* dbsv:��Ҫ���ӵ����ݿ������.
		 */
		INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv = DEFAULT_DB);

		//--added by Long Xiangming. 2006.11.29
		//��MySQL��������������ʱ����Ҫʹ��Զ�����ӷ�ʽ
		INT  connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port);

		void disConnDB(void);

		/*
		 *INT ExecSQL(CHAR *pcSQLStmt)
		 * CHAR* pcSQLStmt����Ϊָ����Ҫִ�е�SQL����ַ�����ָ��
		 */
		INT execSQL1(const char* pcSQLStmt);

		int existTable(const char* tableName);
		int existTableField(const char* tableName, const char* fieldName);
		int isSelectStmt(const char* sql);

		/*
		 * INT getNextRow(struct slctRslt** ppstruSlctRslt);
		 * struct slctRslt** �������,ָ��ȡ�������еĵ�һ�����ݵ�Ԫ��ָ��
		 */

		INT getNextRow(struct slctRslt** ppstruSlctRslt);

		INT getRowCount();

		PTSelectResult getSelectResult();

		/*
		 *void freeRowMem(struct slctRslt* pstruSlctRslt)
		 *struct slctRslt* ָ��Ҫ�ͷſռ���е����׽ṹ��ָ��
		 */
		void freeRowMem(struct slctRslt* pstruSlctRslt);

		/*
		 * INT getIntoPara(CHAR* pcParaType,...);
		 * CHAR* pcParaType ָ��pcParaType���������Ĳ�������.'s'Ϊ
		 * CHAR����,'d'Ϊ*INT����,'u'Ϊ*UINT����,'f'Ϊ
		 * *FLOAT����.����,GetIntoPara("udsf",uPara,dPara,pcPara,
		 * fPara).��"udsf"˵������Ĳ��������ҷֱ�Ϊ*UINT����,
		 * *INT����,*CHAR���ͺ�*float����.
		 */
		INT getIntoPara(CHAR* pcParaType,...);

		/*
		 *��ʼһ��������
		 *bIsReadOnlyΪTRUE,��ʾ������Ϊֻ������
		 *ֻ��������Ա�֤������������������ݱ���һ���ԣ���������
		 *ִ��commit��䲻��Ը��������Ӱ�죬���Ǹ�����ֻ��ִ�в�ѯ
		 *��䣬�����ܶ����ݿ��������޸ġ������Ҫ�޸����ݶ�����
		 *Ҫ��֤���ݵ�һ���ԣ���Ҫִ��SQL������������ݶ��󡣣�
		 */
		void beginTrans(BOOL bIsReadOnly = FALSE);

		/*
		 *�ύһ��������
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
