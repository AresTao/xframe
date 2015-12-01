#if !defined(_TASKTHREAD_H_)
#define _TASKTHREAD_H_

#include "kernalthread.h"
#include "abstracttask.h"


class TTaskThread: public TGeneralThread
{
    public:
        TTaskThread(TAbstractTask& task, const char* tname, int tLogType = File, int tLogLevel = Info, UINT inst = 0);
        ~TTaskThread();

        virtual void onstart();
        void process();

    protected:
        TAbstractTask&  mTask;

};

#endif

