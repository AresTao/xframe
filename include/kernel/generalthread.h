#if !defined(_GENERAL_THREAD_H_)
#define _GENERAL_THREAD_H_

#include "threadif.h"
#include "generalobj.h"
#include "env.h"
#include <stdlib.h>

class TGeneralThread : public ThreadIf
{
public:
	TGeneralThread(const char * tname="TGeneralThread", int tLogType = File, int tLogLevel = Info, UINT inst = 0);
	~TGeneralThread();

    	virtual void process() { tosleep(1000);};
	void prethreadstart();
	virtual void onstart() {
		//used to reload infomation before runnning
		};

public:
	UINT 	mThreadPID;
	UINT	mInstID;
	int 	mLogType;
	int 	mLogLevel;
	CStr	mThreadName;
	GeneralThreadEnv tenv;
};

#endif

