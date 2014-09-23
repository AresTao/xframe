
#include "abstracttask.h"
#include "info.h"
#include "comconst.h"
#include "framectrl.h"

TAbstractTask::TAbstractTask():TGeneralObject()
{
    mTaskRxFifo = new Fifo<TMsg>();
	if(mTaskRxFifo==NULL)
	{
		UniERROR("ERROR!!!! Init Task error! taskRxFifo=NULL");
	}
	mTTQ = new TaskTimerQueue(*mTaskRxFifo);
	if(mTTQ==NULL)
	{
		UniERROR("ERROR!!!! Init Task error! taskTimerQueue=NULL");
	}

	mMsgProcCount=0;
	mTimerProcCount=0;
	mInnerProcCount=0;
}

TAbstractTask::~TAbstractTask()
{
	if(mTaskRxFifo!=NULL) 	delete mTaskRxFifo;
	if(mTTQ!=NULL) 			delete mTTQ;
}

BOOL TAbstractTask::init(UINT instid, UINT hostid, UINT taskid, TFRAMEControl* framectrl)
{
	UniINFO(" Init Task: \n");
    	UniINFO("   taskId(logAddr): %d", taskid);
    	UniINFO("   hostId(phyAddr): %d", hostid);
    	UniINFO("   instId: %d", instid);
	mInstId=instid;
	mHostId=hostid;
	mTaskId=taskid;
	mFrameCtrl = framectrl;

	TTaskEnv env(mTaskId);
	env.load();

	CStr task_env;
	env.list(task_env);
	UniINFO(" Task env: %s", task_env.c_str());

	logType=env.getLogType();
	logLevel=env.getLogLevel();
	env.getTaskInfo(objectType, objectName);
	mTME.clear();
	env.getTimerRsc(mTME);
	mDB.clear();
	if(env.getDBRsc(mDB))
		mDB.conn();
	return onInit(env.extend);
}


void TAbstractTask::process()
{
    //TODO:��鶨ʱ�����߼�
	unsigned int nextstep = mTTQ->process();
	if(nextstep>1000) nextstep=1000;	//��ʱ�����ص���һ����ʱ��ʱ��ʱ�������������1��

	std::auto_ptr<TMsg> msg=std::auto_ptr<TMsg>(mTaskRxFifo->getNext(nextstep));	//����Ϣ������ȴ�ʱ���Ƕ�ʱ����һ����ʱʱ����
	if(msg.get())
	{
		TUniNetMsg* pUniNetMsg = NULL;
		pUniNetMsg=dynamic_cast<TUniNetMsg*>(msg.get());
		if(pUniNetMsg)
		{
			//procMsg(pUniNetMsg);
			msg.release();
			procMsg(std::auto_ptr<TUniNetMsg>(pUniNetMsg));

			mMsgProcCount++;
			return;
		}
		TTimeOutMsg* pTimeOutMsg = NULL;
		pTimeOutMsg=dynamic_cast<TTimeOutMsg*>(msg.get());
		if(pTimeOutMsg)
		{
			onTimeOut(pTimeOutMsg->timerKey, pTimeOutMsg->timerPara);
			//ע�ⲻҪɾ������Ϣ������Ϣ���Զ�ɾ������Ϊ���� auto_ptr��

			mTimerProcCount++;
			return;
		}
		TEventMsg* pEventMsg = NULL;
		pEventMsg=dynamic_cast<TEventMsg*>(msg.get());
		if(pEventMsg)
		{
			switch(pEventMsg->eventID)
			{
				case KERNAL_APP_INIT:
					onStart(pEventMsg->transID);
					break;
				case KERNAL_APP_HEATBEAT:
					onHeartBeat(pEventMsg->transID);
					break;
				case KERNAL_APP_RELOAD:
					onReload(pEventMsg->transID, pEventMsg->eventInfo);
					break;
				case KERNAL_APP_SHUTDOWN:
					onShutdown(pEventMsg->transID);
					break;
				case KERNAL_APP_STATUES:
					onStatues(pEventMsg->transID, pEventMsg->eventInfo);
				default:
					break;
			}
			mInnerProcCount++;
			return;
		}

	}
}


void TAbstractTask::sendMsg(TUniNetMsg* msg)
{
    //����Ϣ������У���kernel���д���
	msg->sender.taskType=objectType;
	msg->sender.taskID=getTaskId();
	msg->sender.instID=getInstId();

	msg->oAddr.logAddr=getTaskId();
	msg->oAddr.phyAddr=getHostId();
	msg->oAddr.taskInstID=getInstId();

	//std::ostrstream st;
	//msg->print(st);
	//UniDEBUG("Content of the message:%s",st.str());
	//st.freeze(0);
	mFrameCtrl->add(msg);
}


void  TAbstractTask::sendMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* pUniNetMsg = msg.release();	//�ͷ��Զ�ָ�룬������Ϣ����
	if(pUniNetMsg) sendMsg(pUniNetMsg);
}

TTimerKey TAbstractTask::setTimer(TTimeMark timerId, TTimeDelay timerDelay, void* para)
{
	return mTTQ->add(timerId, timerDelay, para);
}

TTimerKey TAbstractTask::setTimer(TTimeMark timerId, void* para)
{
	if(timerId>=0 && timerId<__TASK_TIMER_NUM)
		return setTimer(mTME[timerId].timerId, mTME[timerId].timerDelay, para);
	else
		return 0;
}

void TAbstractTask::onTimeOut(TTimerKey timerKey, void* para)
{
    UniDEBUG("Task %d Time out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",mTaskId);

	//��Ҫɾ����ʱ������ʱ֮�󣬶�ʱ���Զ�ɾ����
	//�û�Ҳ��Ҫ����ɾ��para����ʱ����Ϣ������ʱ����Զ�ɾ��
}

BOOL TAbstractTask::delTimer( TTimerKey timerKey)
{
	mTTQ->remove(timerKey);	//�û��Զ��������Զ�ɾ��
    return TRUE;
}

BOOL  TAbstractTask::post(TMsg* msg)
{
	msg->sender.taskType=objectType;
	msg->sender.taskID=getTaskId();
	msg->sender.instID=getInstId();
	return mFrameCtrl->postTo(msg);
}

void TAbstractTask::onStart(UINT transid)
{
	BOOL ret=preTaskStart();

	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_INIT;
		msg->transID=transid;
		if(ret)	msg->status=1;	//1=active
		else msg->status=0;		//0=inactive
	    post(msg);
	}
}

void TAbstractTask::onShutdown(UINT transid)
{

	BOOL ret=preTaskShutdown();

	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_SHUTDOWN;
		msg->transID=transid;
		if(ret)	msg->status=1;	//1=shutdown
		else msg->status=0;		//0=still active

		post(msg);
	}

}

void TAbstractTask::onReload(UINT transid, CStr& cmd)
{
// set logLevel DEBUG     ������Ϣ�������Ϊ����
// set logLevel INFO      ������Ϣ�������Ϊ��Ϣ
// set logLevel ERROR     ������Ϣ�������Ϊ����
// set logLevel default	  ������Ϣ�������Ϊ�����ļ�Ĭ��
// reload all			  ���¼�����������
// reload log             ���¼�����־�������
// reload db              ���¼������ݿ�����
// reload timer           ���¼��ض�ʱ������
	TTaskEnv env(mTaskId);
	GeneralThreadEnv* pEnv=NULL;
	pEnv=(GeneralThreadEnv*)threadEnvGet();	//�����߳�����

	CStr scmd[5];
	cmd.split(scmd,5);
	if(scmd[0]=="set")
	{
		if(scmd[1]=="logLevel")
		{
			if(scmd[2]=="DEBUG")
			{
				if(pEnv)
				{
					pEnv->logLevel = Debug;
    			}
			}
			else if(scmd[2]=="INFO")
			{
				if(pEnv)
				{
					pEnv->logLevel = Info;
    			}
			}
			else if(scmd[2]=="ERROR")
			{
				if(pEnv)
				{
					pEnv->logLevel = Err;
    			}
			}
			else if(scmd[2]=="default")
			{
				if(pEnv)
				{
					pEnv->logLevel = logLevel;
    			}
			}
		}
	}
	else if(scmd[0]=="reload")
	{
		env.load();
		if(scmd[1]=="all" || scmd[1]=="log")
		{
			logType=env.getLogType();
			logLevel=env.getLogLevel();
			if(pEnv)
			{
				pEnv->logLevel = logLevel;
    			pEnv->logType = logType;
			}
		}
		if(scmd[1]=="all" || scmd[1]=="db")
		{
			mDB.clear();
			if(env.getDBRsc(mDB))
				mDB.conn();
		}
		if(scmd[1]=="all" || scmd[1]=="timer")
		{
			mTME.clear();
			env.getTimerRsc(mTME);
		}
	}

	BOOL ret=reloadTaskEnv(cmd, env.extend);

	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_RELOAD;
		msg->transID=transid;
		if(ret) msg->status=1;	//1=reload success
		else msg->status=0; 	//0=reload error
		post(msg);
	}
	return;
}

void TAbstractTask::onHeartBeat(UINT transid)
{
	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_HEATBEAT;
		msg->transID=transid;
		msg->status=1;	//1=active

		post(msg);
	}
	return;
}

void TAbstractTask::onStatues(UINT transid, CStr& cmd)
{
	CStr sts;

	sts<<"TaskID=";
	sts<<mTaskId;
	sts<<", HostID=";
	sts<<mHostId;
	sts<<", InstID=";
	sts<<mInstId;
	sts<<":\r\n";
	sts<<" Proc UniMsg Count=";
	sts<<mMsgProcCount;
	sts<<", TimerOut Count=";
	sts<<mTimerProcCount;
	sts<<", InnerMsg Count=";
	sts<<mInnerProcCount;
	sts<<"\r\n";

	getStatues(cmd, sts);

	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_STATUES;
		msg->transID=transid;
		msg->status=1;	//1=get statues
		msg->eventInfo=sts;
		post(msg);
	}
	return;

}

void TAbstractTask::procMsg(TUniNetMsg* msg)
{
    UniDEBUG("Task recieved Msg\n");
    UniDEBUG("%d---->%d\n",msg->oAddr.logAddr,msg->tAddr.logAddr);
}

void TAbstractTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
    TUniNetMsg* pUniNetMsg = msg.get();
    if(pUniNetMsg) procMsg(pUniNetMsg);
}

