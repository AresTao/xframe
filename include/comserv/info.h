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

#ifdef WIN32
#define LOG_DEBUG  	7
#define LOG_INFO	6
#define LOG_NOTICE	5
#define LOG_WARNING	4
#define LOG_ERR		3
#define LOG_CRIT	2
#define LOG_ALERT	1
#define LOG_EMERG	0
#endif

const char cLogLevel[][15]={"[LOG_EMERG]","[LOG_ALERT]","[LOG_CRIT]","[LOG_ERR]","[LOG_WARNING]","[LOG_NOTICE]","[LOG_INFO]","[LOG_DEBUG]"};

enum sLogType
{
    Default = 0,
    Cout = 1,
    Both = 2,
    File = 3
};

enum sLogLevel
{
    Emerg = LOG_EMERG,
    Err = LOG_ERR,
    Info = LOG_INFO,
    Debug = LOG_DEBUG
};
typedef enum sLogType 	LogType;
typedef enum sLogLevel	LogLevel;

void InitLogInfo(LogType type, LogLevel level, int logFileLength=0, const char* logFileName = 0);

void setLevel(LogLevel level);
void setType(LogType type);
void setFileName(char* logFileName);
void setFileLength(int logFileLength);

#ifndef  _SINGLE_THREAD
static  Mutex           g_loginfoMutex;
#endif
static  LogLevel        g_logLevel;
static  LogType         g_logType;
static  char            g_logFileName[256];
static  int             g_logFileMaxLength;     //10~10000 10M~10G

void toAme(const char *s);

void toPrint(const char *s, const char* appName);

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
