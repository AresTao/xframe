#if !defined(_KERNALTHREAD_H_)
#define _KERNALTHREAD_H_

#include "generalthread.h"

_CLASSDEF(TFRAMEControl);
class TKernalThread: public TGeneralThread
{
    public:
        TKernalThread(TFRAMEControl& kn);
        ~TKernalThread();

        void process();
        void onstart();

    protected:
        TFRAMEControl& mKn;
};

#endif

