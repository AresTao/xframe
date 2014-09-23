#include "generalthread.h"
#include <stdio.h>

TGeneralThread::TGeneralThread(const char * tname, int tLogType, int tLogLevel, UINT inst)
	:ThreadIf(tname),mLogType(tLogType),mLogLevel(tLogLevel),mInstID(inst)
{
    //env.logLevel = tLogLevel;
    //env.logType = tLogType;
}

TGeneralThread::~TGeneralThread()
{
}

void TGeneralThread::prethreadstart()
{
	onstart();	//subclass init info
	mThreadPID=getPID();
	mThreadName=getName();
    tenv.threadName = mThreadName.c_str();
    tenv.logType=mLogType;
    tenv.logLevel=mLogLevel;
	threadEnvSet((void *)&tenv);	//设置线程全局的env变量，把env设置进去
}



