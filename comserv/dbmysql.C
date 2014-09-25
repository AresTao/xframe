#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include "dbmysql.h"

#include "comtypedef_vchar.h"
CDBMysql::CDBMysql():CDB()
{
	strcpy(m_struDBInfo.DBName,"test");
	strcpy(m_struDBInfo.DBAddr ,"localhost");
	m_struDBInfo.DBPort=0; //use the default port of mysql.

	mysql_init(&m_struMYSQL);
	mysql_options(&m_struMYSQL, MYSQL_OPT_RECONNECT, (char *)"1");

	m_pMysqlRes = NULL;
	alreadyInit = FALSE;
}

CDBMysql::~CDBMysql()
{
	if (m_pMysqlRes != NULL) mysql_free_result(m_pMysqlRes);
	m_pMysqlRes = NULL;

	if (m_bConnFlag) disConnDB();
	mysql_thread_end();
}

INT CDBMysql::connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv)
{
	return connDB(uid,pwd,dbsv,"localhost",0);

}

INT  CDBMysql::connDB(const CHAR* uid,const CHAR* pwd,const CHAR* dbsv,const CHAR* host, INT port)
{
	if (m_bConnFlag) disConnDB();

	m_bConnFlag = FALSE;

	if ((strlen(uid) >= MAX_UID_LEN)||
			(strlen(pwd) >= MAX_PWD_LEN)||
			(strlen(dbsv) >= MAX_DBSV_LEN))
	{
		strcpy(m_pcErrMsg , (CHAR*)"Too long uid or pwd or dbsv");
		return 0;
	}

	if (mysql_real_connect(&m_struMYSQL, host, uid, pwd, dbsv, port, NULL, 0))
	{
		m_bConnFlag = TRUE;
		mysql_autocommit(&m_struMYSQL,1);
		strcpy(m_pcDBsv,dbsv);
		strcpy(m_pcUsr,uid);
		strcpy(m_pcPwd,pwd);
		strcpy(m_struDBInfo.DBAddr,host);
		m_struDBInfo.DBPort=port;
		strcpy(m_struDBInfo.DBName,dbsv);
		alreadyInit = TRUE;
		return 1;
	}
	else
	{
		strcpy(m_pcErrMsg , (CHAR*)mysql_error(&m_struMYSQL));
		return 0;
	}
}

INT CDBMysql::reconnect()
{
	if(!alreadyInit) return 0; //至少应连接过一次

	if (m_bConnFlag) disConnDB();
	return connDB(m_pcUsr,m_pcPwd,m_pcDBsv,m_struDBInfo.DBAddr,m_struDBInfo.DBPort);

}

void CDBMysql::disConnDB()
{
	if (!m_bConnFlag)
	{
		return;
	}

	mysql_close(&m_struMYSQL);

	m_bConnFlag = FALSE;
}

int CDBMysql::isSelectStmt(const char* stmt)
{
	return strncasecmp(stmt,(CHAR*)"SELECT",6) == 0 || 
		strncasecmp(stmt,(CHAR*)"SHOW",4) == 0 ||
		strncasecmp(stmt,(CHAR*)"DESCRIBE",8) == 0
		;
}

INT CDBMysql::execSQL1(const char* pcSQLStmt)
{
	int re;
	if (strlen(pcSQLStmt) >= MAX_STMT_LEN)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Too Long Statement(beyond 1024 bytes)");
		re = 0;
	}

	else if (!m_bConnFlag)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Have not connected to the database or the connection lost now!");
		UniERROR(m_pcErrMsg);
		re = 0;
	}

	else if ((
				(strncasecmp(pcSQLStmt, "COMMIT", 6) == 0)||
				(strncasecmp(pcSQLStmt, "ROLLBACK", 8) == 0)
		 )&&(m_bTransaction)
		)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Can not commit or rollback work in a read-only transaction with a SQL statement;");
		re = 0;
	}
	else if (mysql_real_query(&m_struMYSQL, pcSQLStmt, strlen(pcSQLStmt)) == 0)
	{
		strncpy(m_pcSQLStmt,pcSQLStmt,MAX_STMT_LEN);
		if(isSelectStmt(pcSQLStmt))
		{
			m_iQueryFlag = 1;

			//Added by Wang Shufeng,2004,8.23
			//防止发生内存泄漏.
			if (m_pMysqlRes != NULL) mysql_free_result(m_pMysqlRes);
			m_pMysqlRes = NULL;
			m_pMysqlRes = mysql_store_result(&m_struMYSQL);
			freeResult(); //释放上次查询结果内存。
			m_isFirstlyGetResult = TRUE;
		}

		if (m_bTransaction)
		{
			if(m_iQueryFlag)
				m_iProcRowCount += mysql_num_rows(m_pMysqlRes);            
			else
				m_iProcRowCount += mysql_affected_rows(&m_struMYSQL);
		}
		re = 1;
	}
	else
	{
		strcpy(m_pcErrMsg , (CHAR*)mysql_error(&m_struMYSQL));
		re = 0;
	}
	return re;
}


PTSelectResult CDBMysql::getSelectResult()
{
	if(m_isFirstlyGetResult==FALSE) return m_pSelectResult; //一次查询后，可能多次调用getSelectResult(),

	m_isFirstlyGetResult = FALSE;
	MYSQL_FIELD *field = mysql_fetch_fields(m_pMysqlRes);
	UINT num_rows   = mysql_num_rows(m_pMysqlRes);
	if (num_rows == 0)
	{
		strcpy(m_pcErrMsg,"No results return.");
		m_pSelectResult ->rowNum = 0;
		return m_pSelectResult;
	}

	UINT num_fields = mysql_num_fields(m_pMysqlRes);
	if(num_fields>MaxFieldNum)
	{
		UniERROR("CDBMysql::getSelectResult() --> ERROR: num_fields(%d) is more than MaxFieldNum(%d).",num_fields,MaxFieldNum);
		num_fields = MaxFieldNum;
	}

	size_t *lengths;
	MYSQL_ROW row;
	MYSQL_FIELD *fields;

	TRow    *pRowTemp;
	TRow    *pRowSwap;
	TField  fieldTemp;
	while ((row = mysql_fetch_row(m_pMysqlRes)))
	{
		fields = mysql_fetch_fields(m_pMysqlRes);
		lengths = (size_t*)mysql_fetch_lengths(m_pMysqlRes);
		pRowTemp = new TRow;
		for(int i = 0; i < num_fields; i++)
		{
			CHAR* chTmp;
			INT iTmp = 0;
			chTmp = new CHAR[lengths[i]+1];
			memset(chTmp,0,lengths[i]+1);
			if(row[i] != NULL && lengths[i] != 0)
				strncpy(chTmp,row[i],lengths[i]);
			switch (fields[i].type)
			{
				case MYSQL_TYPE_DECIMAL:
					if (lengths[i] == 0)
					{
						fieldTemp.isNull = TRUE;
					}
					else
					{
						fieldTemp.isNull = FALSE;
						fieldTemp.type = T_UINT;
						fieldTemp.value.uintValue  = strtoul(chTmp,0,0);
					}
					break;
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
					if (lengths[i] == 0)
					{
						fieldTemp.isNull = TRUE;
					}
					else
					{
						fieldTemp.isNull = FALSE;
						fieldTemp.type = T_UINT;
						fieldTemp.value.uintValue  = strtoul(chTmp,0,0);
					}
					break;
				case MYSQL_TYPE_LONGLONG:
					if (lengths[i] == 0)
					{
						fieldTemp.isNull = TRUE;
					}
					else
					{
						fieldTemp.isNull = FALSE;
						fieldTemp.type = T_LONGLONG;
#ifdef WIN32
						fieldTemp.value.longlongValue  = atol(chTmp);
#else
						fieldTemp.value.longlongValue  = atoll(chTmp);
#endif
					}
					break;
				case MYSQL_TYPE_FLOAT:
				case MYSQL_TYPE_DOUBLE:
					if (lengths[i] == 0)
					{
						fieldTemp.isNull = TRUE;
					}
					else
					{
						fieldTemp.isNull = FALSE;
						fieldTemp.type = T_FLOAT;
						fieldTemp.value.floatValue = atof(chTmp);
					}
					break;
				default: //for string
					if(lengths[i]>MaxStrAIDLength)
					{
						m_pSelectResult -> rowNum = 0;
						sprintf(m_pcErrMsg,"length of field %s is too long[>%d]!",fields[i].name,lengths[i]);
						delete pRowTemp;
						delete[] chTmp;
						return m_pSelectResult;
					}
					fieldTemp.isNull = FALSE;
					fieldTemp.type = T_STRING;
					strncpy(fieldTemp.value.stringValue,chTmp,strlen(chTmp)+1);
					break;
			}

			delete [] chTmp;
			pRowTemp->arrayField[i] = fieldTemp;
		}//end of for


		if (m_pSelectResult -> rowNum == 0)
		{
			m_pSelectResult-> rowNum  = num_rows;
			m_pSelectResult-> fieldNum = num_fields;
			m_pSelectResult->pRows = pRowTemp;
			pRowTemp->next = NULL;
			pRowSwap = pRowTemp;
			for(int j=0;j<num_fields;j++)
			{
				strcpy(m_pSelectResult->fieldInfos[j].name,fields[j].name);
				m_pSelectResult->fieldInfos[j].type = pRowTemp->arrayField [j].type;
				m_pSelectResult->fieldInfos[j].len  = lengths[j];
			}

		}
		else
		{
			pRowSwap ->next = pRowTemp;
			pRowTemp->next  = NULL;
			pRowSwap = pRowTemp;
		}
	}//end of while

	return m_pSelectResult;
}

INT CDBMysql::getNextRow(struct slctRslt** ppstruSlctRslt)
{
	if (!m_bConnFlag)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Have not connected to DB now!");
		return 0;
	}

	if (m_iQueryFlag == 0)
	{
		strcpy(m_pcErrMsg , (CHAR*)"The ExecSQL operation is not executed,or is not successful,or is not a query operation!");
		return 0;
	}
	MYSQL_FIELD *field = mysql_fetch_fields(m_pMysqlRes);
	MYSQL_ROW row = mysql_fetch_row(m_pMysqlRes);
	if (row == NULL)
	{
		ppstruSlctRslt = NULL;
		return 0;
	}
	else
	{
		UINT num_fields = mysql_num_fields(m_pMysqlRes);
		size_t *lengths;
		lengths = (size_t*)mysql_fetch_lengths(m_pMysqlRes);      
		struct slctRslt* pSlctRslt = NULL;
		struct slctRslt* pPreSlctRslt = NULL;
		for(INT i = 0; i < num_fields; i++)
		{
			pSlctRslt = new struct slctRslt;
			pSlctRslt->ResultLen = lengths[i];
			CHAR* chTmp;
			INT iTmp = 0;
			chTmp = new CHAR[lengths[i]+1];
			memset(chTmp,0,lengths[i]+1);
			strncpy(chTmp,row[i],lengths[i]);
			switch (field[i].type)
			{
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
					if (lengths[i] == 0)
					{
						pSlctRslt->ResultLen = 0;
						pSlctRslt->pvResult = NULL;
					}
					else
					{
						pSlctRslt->ResultLen = sizeof(INT);
						pSlctRslt->pvResult = malloc(sizeof(INT));
						//iTmp = atoi(chTmp);
						//memcpy(pSlctRslt->pvResult,
						//	   (void*)&iTmp,
						//	   pSlctRslt->ResultLen);
						*(INT*)pSlctRslt->pvResult = atoi(chTmp);
					}
					break;
				case MYSQL_TYPE_LONGLONG:
				case MYSQL_TYPE_FLOAT:
					if (lengths[i] == 0)
					{
						pSlctRslt->ResultLen = 0;
						pSlctRslt->pvResult = NULL;
					}
					else
					{
						pSlctRslt->ResultLen = sizeof(FLOAT);
						pSlctRslt->pvResult = malloc(sizeof(FLOAT));
						*(FLOAT*)pSlctRslt->pvResult = (FLOAT)atof(chTmp);
					}
					break;
				case MYSQL_TYPE_DOUBLE:
					if (lengths[i] == 0)
					{
						pSlctRslt->ResultLen = 0;
						pSlctRslt->pvResult = NULL;
					}
					else
					{
						pSlctRslt->ResultLen = sizeof(double);
						pSlctRslt->pvResult = malloc(sizeof(double));
						*(double*)pSlctRslt->pvResult = atof(chTmp);
					}
					break;
				default:
					pSlctRslt->ResultLen = strlen(chTmp);
					pSlctRslt->pvResult = malloc(strlen(chTmp)+1);
					memset(pSlctRslt->pvResult,0,strlen(chTmp)+1);
					strncpy((CHAR*)(pSlctRslt->pvResult),chTmp,strlen(chTmp));
					break;
			}

			//删除临时变量空间
			delete [] chTmp;
			pSlctRslt->pstruNext = NULL;
			if (pPreSlctRslt == NULL)
			{
				*ppstruSlctRslt = pSlctRslt;
				pPreSlctRslt = pSlctRslt;
			}
			else
			{
				pPreSlctRslt->pstruNext = pSlctRslt;
				pPreSlctRslt = pSlctRslt;
			}
		}
		return 1;
	}
}

INT CDBMysql::getRowCount()
{
	if(m_iQueryFlag)
		return mysql_num_rows(m_pMysqlRes);   
	if(m_bTransaction)
		return m_iProcRowCount;
	return mysql_affected_rows(&m_struMYSQL);
}

void CDBMysql::freeRowMem(struct slctRslt* pstruSlctRslt)
{
	struct slctRslt* p = NULL;

	while (pstruSlctRslt != NULL)
	{
		p = pstruSlctRslt;
		pstruSlctRslt = pstruSlctRslt->pstruNext;
		free(p->pvResult);
		delete p;
		p = NULL;
	}
}
INT CDBMysql::getIntoPara(CHAR* pcParaType,...)
{
	if (!m_bConnFlag)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Have not connected to DB now!");
		return 0;
	}
	if (m_iQueryFlag == 0)
	{
		strcpy(m_pcErrMsg , (CHAR*)"The ExecSQL operation is not executed,or is not successful,or is not a query operation!");
		return 0;
	}
	struct slctRslt *p,*q;
	p = NULL;
	q = NULL;
	CHAR *pc;
	pc = NULL;
	INT i = 0;
	INT nullDataCount = 0;
	INT iParaCount = strlen(pcParaType);
	UINT num_fields = mysql_num_fields(m_pMysqlRes);

	if (num_fields > iParaCount)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Too few parameters");
		return -1;
	}

	if (num_fields < iParaCount)
	{
		strcpy(m_pcErrMsg , (CHAR*)"Too many parameters");
		return -1;
	}

	if (getNextRow(&p) == 0)
	{
		return 0;
	}

	q = p;
	va_list marker;
	va_start( marker, pcParaType);
	while ((q != NULL)&&(i < iParaCount))
	{
		switch (pcParaType[i])
		{
			/*
			 *2002.7.17 ADD BY WSF->
			 */
			/*
			 *CHAR类型数据处理
			 */
			case 'c' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, CHAR*)) = CHAR_MAX;//以最大INT表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, CHAR*)) = *((INT*)q->pvResult);    	//非空数据
				break;
				/*
				 *BYTE类型数据处理
				 */
			case 'C' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, BYTE*)) = BYTE_MAX;//以最大INT表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, BYTE*)) = *((INT*)q->pvResult);    	//非空数据
				break;
				/*
				 *SHORT类型数据处理
				 */
			case 'h' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, SHORT*)) = SHRT_MAX;//以最大INT表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, SHORT*)) = *((INT*)q->pvResult);    	//非空数据
				break;
				/*
				 *USHORT类型数据处理
				 */
			case 'H' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, USHORT*)) = USHRT_MAX;//以最大INT表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, USHORT*)) = *((INT*)q->pvResult);    	//非空数据
				break;
			case 'd' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, INT*)) = INT_MAX;//以最大INT表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, INT*)) = *((INT*)q->pvResult);    	//非空数据
				break;
			case 's' :
				pc = va_arg( marker, CHAR*);
				if (q->ResultLen == 0) nullDataCount++;
				strncpy(pc,(CHAR*)q->pvResult,q->ResultLen);
				pc[q->ResultLen] = '\0';
				break;
			case 'u' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, INT*)) = UINT_MAX;//以最大UINT表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, UINT*)) = *((UINT*)q->pvResult);//非空数据
				break;
				/*
				 *FLOAT类型数据处理
				 */
			case 'f' :
				if (q->ResultLen ==0 )
				{
					*(va_arg( marker, FLOAT*)) = FLT_MAX;//以最大浮点数表示空数据
					nullDataCount++;
				}
				else *(va_arg( marker, FLOAT*)) = *((FLOAT*)q->pvResult);//非空数据
				break;
		}
		i++;
		q = q->pstruNext;
	}

	freeRowMem(p);
	if (nullDataCount == 0)	return 1;
	else return(NULLBASE+nullDataCount);
}

void CDBMysql::beginTrans(BOOL bIsReadOnly)
{
	if (!m_bConnFlag)
	{
		return;
	}
	m_iProcRowCount = 0;
	mysql_autocommit(&m_struMYSQL,0);
	m_bTransaction = TRUE;
}

void CDBMysql::commit()
{
	if (!m_bConnFlag)
	{
		return;
	}
	m_bTransaction = FALSE;
	//m_bReadOnlyTrans = FALSE;
	m_iProcRowCount = 0;
	mysql_commit(&m_struMYSQL);
	mysql_autocommit(&m_struMYSQL,1);
}

void CDBMysql::rollback()
{
	if (!m_bConnFlag)
	{
		return;
	}
	m_bTransaction = FALSE;
	//m_bReadOnlyTrans = FALSE;
	m_iProcRowCount = 0;
	mysql_rollback(&m_struMYSQL);
	mysql_autocommit(&m_struMYSQL,1);
}

BOOL CDBMysql::ping()
{
	if(!alreadyInit)
	{
		UniERROR("ERROR in CDBMysql::ping()-->the db is not init yet. You must call CDB::instance() first!\n");
		return FALSE;
	}
	if(mysql_ping(&m_struMYSQL)==0)
		return TRUE;
	else
	{
		BOOL rt = FALSE;
		sprintf(m_pcErrMsg,"%s %s","Database connection lost.", (CHAR*)mysql_error(&m_struMYSQL));
		UniERROR(m_pcErrMsg);
		CHAR temp[256];
		sprintf(temp,"%s","Now trying to reconnect to the database......");
		if(reconnect())
		{
			strcat(temp," succeed.");
			UniINFO(temp);
			rt = TRUE;
		}
		else
		{
			strcat(temp," failed. ");
			strcat(temp,m_pcErrMsg);
			UniERROR(temp);
			rt = FALSE;
		}
		return rt;
	}
}

int CDBMysql::existTable(const char* tableName)
{
	int re = execSql("show tables");
	if(re != 1)
	{
		UniERROR("EXEC SQL: 'show tables' return %d", re);
		return 0;
	}
	TSelectResult* selectResult = getSelectResult();
	if(selectResult == NULL)
	{
		UniERROR("EXEC SQL: 'show tables' result is NULL");
		return 0;
	}
	if(selectResult->fieldNum != 1)
	{
		UniERROR("EXEC SQL: 'show tables' invalid field number %d", selectResult->fieldNum);
		return 0;
	}
	TRow* row = selectResult->pRows;
	while(row != NULL)
	{
		if(!row->arrayField[0].isNull && row->arrayField[0].type == T_STRING && strcasecmp(tableName, row->arrayField[0].value.stringValue) == 0)
			return 1;
		row = row->next;
	}
	return 0;
}

int CDBMysql::existTableField(const char* tableName, const char* fieldName)
{
	CStr sql = "describe ";
	sql << tableName;
	int re = execSql(sql.c_str());
	if(re != 1)
	{
		UniERROR("EXEC SQL: '%s' return %d", sql.c_str(), re);
		return 0;
	}
	TSelectResult* selectResult = getSelectResult();
	if(selectResult == NULL)
	{
		UniERROR("EXEC SQL: '%s' result is NULL", sql.c_str());
		return 0;
	}
	if(selectResult->fieldNum < 1)
	{
		UniERROR("EXEC SQL: '%s' invalid field number %d", sql.c_str(), selectResult->fieldNum);
		return 0;
	}
	TRow* row = selectResult->pRows;
	while(row != NULL)
	{
		if(!row->arrayField[0].isNull && row->arrayField[0].type == T_STRING && strcasecmp(fieldName, row->arrayField[0].value.stringValue) == 0)
			return 1;
		row = row->next;
	}
	return 0;
}
