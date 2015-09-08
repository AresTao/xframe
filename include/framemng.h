#ifndef _TFRAMEMANAGER_H
#define _TFRAMEMANAGER_H

#include <vector>
#include <cstdio>
#include "msgdef.h"
#include "msg.h"
#include "fifo.h"
#include "clist.h"
#include "unihashtable.h"
#include "env.h"
#include "udpsocket.h"

using namespace std;

_CLASSDEF(TFRAMEManager);
_CLASSDEF(TFRAMEControl);
_CLASSDEF(TKernalThread);
_CLASSDEF(TTaskThread);
_CLASSDEF(TAbstractTask);
_CLASSDEF(TaskTimerQueue);
_CLASSDEF(TEnv);

/********************************************/
// this is for xclient
const int maxLen = 10000;

#pragma pack (1)
struct Command
{
	int type;  //0 present for reload,1 present for status
	int taskId;
	int instId;
	char cmd[100];
	Command():taskId(-1), instId(-1)
	{
		cmd[0] = 0;
	}
};
/********************************************/

struct TTask
{
	UINT			instID;
	UINT			taskID;
	CStr			taskName;
	TAbstractTask* 	task;
	TTaskThread*	thread;
};

//singleton��ʽ��Xframemanager
class TFRAMEManager
{
	//�ⲿ���ýӿ�
	public:
		TFRAMEManager();
		virtual ~TFRAMEManager();
		static TFRAMEManager * instance();

		//����֮ǰ���������ļ���ʼ��
		void Init(UINT appID, char* appName);
		//��ϵͳ����֮ǰ��Ӧ�õ�����ע�ᵽmanager��
		void registerTask(UINT taskID, TAbstractTask* task);
		//ϵͳ����
		void Run();

	private:
		//socket for external watch tool
		CUDPSocket socket;

		TFRAMEControl *     framectrl;
		TKernalThread * kernalThread;

		UINT mMaxInst;


		int mLogType;
		int mLogLevel;

		CStr mSCIP;
		UINT	 mSCPort;
		UINT	 mSCHB;

		UINT mAppID;
		CStr mAppName;
		CStr mHostIP;

		CList<TProcRsc>		mProcList;		//�����ļ������õ�ϵͳ�����б�
		CHashTable<UINT, TAbstractTask *>	mTaskObj;		//Ӧ��ע��Ķ���ʵ���б�<UINT taskID, TAbstractTask* task>

		CList<TTask*> 						mInstList;		//����Inst �����ʵ���б�����ϵͳ����ʱ��˳����غͳ�ʼ��
		CHashTable<UINT, CList<TTask*>*>	mTaskList;		//����Task �����ʵ���б�������ϵͳ����Ϳ���

		Fifo<TMsg> * mFifo; 	//����Ӧ�÷�����Ϣ�Ķ���

		static TFRAMEManager * _instance;
		UINT	mTransID;

		TaskTimerQueue* mTTQ;			//�ڲ���ʱ������
		TTimerRsc 		mTME;		//ÿ������Ĭ�����ò�����30����ʱ��

		BOOL   shutdown;

};

#endif

