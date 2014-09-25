#ifndef INFO_H
#define INFO_H

#ifndef WIN32
#include <syslog.h>
#include <unistd.h>
#endif

#include <iostream>
#include <stdarg.h>

#include "threadif.h"
#include "threadmutex.h"
#include "threadlock.h"
#include "threadcondition.h"
#include "comconst.h"
#include "env.h"

enum sLogType
{
	Default = 0,
	Cout = 1,
	Both = 2,
	File = 3
};

enum sLogLevel
{
	None = 0,
#ifdef WIN32
	Err = 3,
	Info = 6,
	Debug = 7
#else
		Err,
	Info = LOG_INFO,
	Debug = LOG_DEBUG
#endif
};

typedef enum sLogType 	LogType;
typedef enum sLogLevel	LogLevel;

//��ʼ����־�������������ʽ
//	�����ʽ��	type
//				CInfo::LogType::Cout   ��Ļ
//				CInfo::LogType::File   �ļ�
//	��־����	level
//				CInfo::LogLevel::Err   ������־
//				CInfo::LogLevel::Info  �ճ�������Ϣ��־
//				CInfo::LogLevel::Debug ������Ϣ��־
//	��־�ļ���С��	��λM��Ĭ��500M
//	��־�ļ�ǰ׺��	logFileName����־�ļ�ǰ׺��Ĭ��UNI.INFO����־������ʽ��UNI.INFO.20110210.AppName.log��
//					����ļ�����Ԥ�ڵĴ�С���򽫾ɵ��ļ������޸�Ϊ UNI.INFO.20110210.AppName.log.ʱ��
void InitLogInfo(LogType type, LogLevel level, int logFileLength=0, const char* logFileName = 0);

//������־�������
//	��־����	level
//				CInfo::LogLevel::Err   ������־
//				CInfo::LogLevel::Info  �ճ�������Ϣ��־
//				CInfo::LogLevel::Debug ������Ϣ��־
void setLevel(LogLevel level);
void setType(LogType type);
void setFileName(char* logFileName);
void setFileLength(int logFileLength);


//ȫ������
#ifndef  _SINGLE_THREAD
static  Mutex           g_loginfoMutex;
#endif
static  LogLevel        g_logLevel;
static  LogType         g_logType;
static  char            g_logFileName[256];
static  int             g_logFileMaxLength;     //10~10000 10M~10G
//g_logFileName �� g_logFileMaxLength ֻ����������ʱ������


//���ame�����ӣ�Ҫ�ж��ǲ��������ame���ⲿ�ֱȽϸ��ӣ�Ҫ�����߳�id���ָ���̵߳�log
//�Ժ����޸�Ϊ�����߳�id����tty���
void toAme(const char *s);

//������ļ�
void toPrint(const char *s, const char* appName);

//�������Ļ
void toCout(const char *s);

#define LogPRINT(args_)\
{\
	std::ostrstream uniLogStr;\
	GeneralThreadEnv* pEnv=(GeneralThreadEnv*)threadEnvGet();\
	if(pEnv==NULL)\
	{\
		uniLogStr args_ << std::ends << std::endl;\
		if(g_logType>=Both) toPrint(uniLogStr.str(), NULL);\
		if(g_logType<=Both) toCout(uniLogStr.str());\
	}\
	else\
	{\
		uniLogStr << "[" << pEnv->threadName << "]" args_ << std::ends << std::endl;\
		if(pEnv->logType>=Both)\
		{\
			toPrint(uniLogStr.str(), pEnv->threadName);\
		}\
		if(pEnv->logType<=Both) toCout(uniLogStr.str());\
	}\
	uniLogStr.freeze(0);\
}

#define LogPRINT_S(args_)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr args_ << std::ends << std::endl;\
	if(g_logType>=Both) toPrint(uniLogStr.str(), NULL);\
	if(g_logType<=Both) toCout(uniLogStr.str());\
	uniLogStr.freeze(0);\
}

//	��־�������
//		UniDEBUG 	������Ϣ���
//		UniINFO		������Ϣ���
//		UniERROR	������Ϣ���
//		UniDEBUG( "error info %s, error info2 %d", mTemp1, mTemp2);
//		LogDEBUG( << "error info" << mTemp1 << "error info2" << mTemp2 <<std::endl);

#define LogDEBUG(args_)\
{\
	GeneralThreadEnv* pEnv=(GeneralThreadEnv*)threadEnvGet();\
	if(pEnv!=NULL) { if(pEnv->logLevel>=Debug) LogPRINT(<< "DEBUG:" args_) }\
	else if(g_logLevel>=Debug) { LogPRINT(<< "DEBUG:" args_) }\
}

#define LogINFO(args_)\
{\
	GeneralThreadEnv* pEnv=(GeneralThreadEnv*)threadEnvGet();\
	if(pEnv!=NULL) {if(pEnv->logLevel>=Info) LogPRINT(<< "INFO:" args_) }\
	else if(g_logLevel>=Info) { LogPRINT(<< "INFO:" args_) }\
}

#define LogWARNING(args_)  LogPRINT(<< "WARNING:" args_)
#define LogERROR(args_)    LogPRINT(<< "ERROR:"  args_)
#define LogCRITICAL(args_) LogPRINT(<< "CRITICAL:"  args_)

#define LogDEBUG_S(args_)\
	if(g_logLevel>=Debug) { LogPRINT_S(<< "DEBUG:" args_) }
#define LogINFO_S(args_)\
	if(g_logLevel>=Info) { LogPRINT_S(<< "INFO:" args_) }
#define LogWARNING_S(args_)  LogPRINT_S(<< "WARNING:" args_)
#define LogERROR_S(args_)    LogPRINT_S(<< "ERROR:"  args_)
#define LogCRITICAL_S(args_) LogPRINT_S(<< "CRITICAL:"  args_)



#define UniINFO(args_...)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr << "[" << __FILE__ << ":" << __LINE__ << "] " << std::ends;\
	_UniINFO(uniLogStr.str(), args_);\
	uniLogStr.freeze(0);\
}

#define UniDEBUG(args_...)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr << "[" << __FILE__ << ":" << __LINE__ << "] " << std::ends;\
	_UniDEBUG(uniLogStr.str(), args_);\
	uniLogStr.freeze(0);\
}

#define UniERROR(args_...)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr << "[" << __FILE__ << ":" << __LINE__ << "] " << std::ends;\
	_UniERROR(uniLogStr.str(), args_);\
	uniLogStr.freeze(0);\
}

#define UniINFO_(args_)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr << "[" << __FILE__ << ":" << __LINE__ << "] " << std::ends args_;\
	_UniINFO(uniLogStr.str(),"%s");\
	uniLogStr.freeze(0);\
}

#define UniERROR_(args_)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr << "[" << __FILE__ << ":" << __LINE__ << "] " << std::ends args_;\
	_UniINFO(uniLogStr.str(),"%s");\
	uniLogStr.freeze(0);\
}

#define UniDEBUG_(args_)\
{\
	std::ostrstream uniLogStr;\
	uniLogStr << "[" << __FILE__ << ":" << __LINE__ << "] " << std::ends args_;\
	_UniDEBUG(uniLogStr.str(),"%s");\
	uniLogStr.freeze(0);\
}

void _UniDEBUG(const char* pfx, const char* fmt, ...);
void _UniERROR(const char* pfx, const char* fmt, ...);
void _UniINFO(const char* pfx, const char* fmt, ...);


#endif
