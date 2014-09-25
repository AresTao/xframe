#include "framemng.h"
#include "framectrl.h"
#include "taskthread.h"
#include "kernalthread.h"
#include "comtypedef.h"
#include "env.h"
#include "info.h"

TFRAMEManager* TFRAMEManager::_instance;
TFRAMEManager::TFRAMEManager()
{

	mFifo=NULL;
	//´´½¨ÄÚ²¿¿ØÖÆÏûÏ¢ÊÕ·¢¶ÓÁÐ
	mFifo = new Fifo<TMsg>();

	//´´½¨framectrl
	framectrl=NULL;
	kernalThread=NULL;
	framectrl= new TFRAMEControl(*mFifo);
	kernalThread =new TKernalThread(*framectrl);

	mTTQ = new TaskTimerQueue(*mFifo);

	mTaskList.clear();
	mTaskObj.clear();
	mInstList.clear();

	mSCIP="127.0.0.1";
	mSCPort=9000;
	mSCHB=10000;

	//kernal ÊÇ0£¬ÆäËûµÄtask ¶¼´Ó1 ¿ªÊ¼
	mMaxInst=0;

	mTransID=0;

	shutdown=FALSE;

}


TFRAMEManager::~TFRAMEManager()
{
	if(kernalThread) delete kernalThread;
	if(framectrl) delete framectrl;

	TTask* task=NULL;
	mInstList.reset();
	while(mInstList.next(task))
	{
		if(task->thread) delete task->thread;
		if(task->task) delete task->task;
	}

	socket.closeSocket();
}


void TFRAMEManager::registerTask(UINT taskID, TAbstractTask* task)
{
	if(taskID>0 && task!=NULL) mTaskObj.put(taskID, task);
}

void TFRAMEManager::Init(UINT appID, char* appName)
{
	TKernalEnv  env;
	env.load();

	mLogType=env.getLogType();
	mLogLevel=env.getLogLevel();
	mAppID=env.getHostID();
	if(appID>0) mAppID=appID;	//ÒÔÓ¦ÓÃ´«Êä½øÀ´µÄID Îª×¼
	mAppName=env.getAppName();
	if(appName) mAppName=appName;	//ÒÔÓ¦ÓÃ´«Êä½øÀ´µÄÃû×Ö Îª×¼

	env.getSCInfo(mSCIP, mSCPort, mSCHB);
	env.getProcList(mProcList);
	env.getTimerRsc(mTME);

	CStr kernal_env;
	env.list(kernal_env);
	UniINFO("FrameMng: Kernal env: \n %s ", kernal_env.c_str());

	framectrl->init(mLogType, mLogLevel);

	TProcRsc 		proc;
	TTaskThread*	tt=NULL;
	TAbstractTask*	obj=NULL;
	TAbstractTask*	task=NULL;
	TTask* 			t=NULL;
	CList<TTask*>*  tlist=NULL;
	BOOL ret=mProcList.front(proc);
	while(ret)
	{
		if(mTaskObj.get(proc.taskID, obj))	//°´ÕÕÆô¶¯¶ÓÁÐÖÐµÄË³ÐòÌáÈ¡ÈÎÎñ¶ÔÏó
		{
			if(obj!=NULL)
			{
				tlist=new CList<TTask*>();
				if(tlist)
				{
					for(int i=0;i<proc.threadNum;i++)	//°´ÕÕÆô¶¯¶ÓÁÐÖÐÉèÖÃµÄÏß³ÌÊýÁ¿³õÊ¼»¯Ïß³Ì
					{
						task=obj->clone();	//Í¨¹ýclone »ñµÃ¸´ÖÆµÄtask Ö¸Õë
						if(task!=NULL)
						{
							mMaxInst++;		//µ±Ç°µÄinst + 1
							task->init(mMaxInst, mAppID, proc.taskID, framectrl);	//³õÊ¼»¯task
							framectrl->getTaskSelector()->registerTask(proc.taskID, mMaxInst, task);	//×¢²áµ½taskselector

							tt=NULL;
							tt=new TTaskThread(*task, proc.taskName.c_str(),task->logType, task->logLevel, mMaxInst);
							if(tt)
							{
								t=new TTask();
								t->taskID=proc.taskID;
								t->instID=mMaxInst;
								t->taskName=proc.taskName;
								t->task=task;
								t->thread=tt;

								mInstList.push_back(t);		//½«task ÈÎÎñºÍÏàÓ¦µÄÏß³Ì¶ÔÏó°´Ë³Ðò±£´æµ½ÊµÀýÁÐ±í
								tlist->push_back(t);		//½«task ÈÎÎñºÍÏàÓ¦µÄÏß³Ì¶ÔÏó±£´æµ½¹ÜÀíÁÐ±í

								UniINFO("FrameMng: Init taskid=%d, instid=%d, taskname=%s !", t->taskID, t->instID, t->taskName.c_str());
							}
						}
					}
					mTaskList.put(proc.taskID, tlist);	//½«task ÈÎÎñºÍÏàÓ¦µÄÏß³Ì¶ÔÏó±£´æµ½ÈÎÎñ¹ÜÀíÁÐ±í
				}
				else
				{
					UniERROR("FrameMng: Init task list error!");
				}
			}
		}
		ret=mProcList.next(proc);	//ÌáÈ¡ÏÂÒ»¸ö
	}

	//init socket
	BOOL success = socket.openServer(NULL, mSCPort);	//Ê¹ÓÃscPORT¼àÌýËùÓÐµØÖ·ÉÏµÄ¶Ë¿Ú
	if(!success)
	{
		UniERROR("FrameMng: Bind command server error! port=%d", mSCPort);
	}
	else
	{
		UniINFO("FrameMng: Bind command server! port=%d", mSCPort);
	}

}

TFRAMEManager * TFRAMEManager::instance()
{
	if(!_instance)
	{
		_instance = new TFRAMEManager();
	}
	return _instance;
}

void TFRAMEManager::Run()
{
	TEventMsg* msg=NULL;

	UniINFO("FrameMng: Starting ......");

	//ÏÈÆô¶¯kernal
	mTransID++;
	msg=new TEventMsg();
	msg->eventID=KERNAL_APP_INIT;
	msg->transID=mTransID;
	msg->status=0;
	msg->sender.taskType=0;
	msg->sender.taskID=0;
	msg->sender.instID=0;
	framectrl->add(msg);
	kernalThread->run();


	//°´Ë³ÐòÆô¶¯¸÷¸öÈÎÎñ
	TTask* task=NULL;
	mInstList.reset();
	mInstList.current(task);
	while(task)
	{
		mTransID++;
		msg=new TEventMsg();
		msg->eventID=KERNAL_APP_INIT;
		msg->transID=mTransID;
		msg->status=0;
		msg->sender.taskType=0;
		msg->sender.taskID=0;
		msg->sender.instID=0;
		task->task->add(msg);
		task->thread->run();
		task->thread->detach();
		msg=NULL;
		task=NULL;
		if(!mInstList.next(task))
			break;
	}

	char mSockBuffer[maxLen];

	CCode code;
	code.content = NULL;
	while(!shutdown)
	{

		UINT64 nextstep = mTTQ->process();
		if(nextstep>1000) nextstep=1000;	//¶¨Ê±Æ÷·µ»ØµÄÏÂÒ»¸ö¶¨Ê±Æ÷Ê±¼äÊ±¼ä¼ä¸ô£¬×î³¤²»³¬¹ý1Ãë

		std::auto_ptr<TMsg> msg=std::auto_ptr<TMsg>(mFifo->getNext(nextstep));	//ÔÚÏûÏ¢¶ÓÁÐ×î³¤µÈ´ýÊ±¼äÊÇ¶¨Ê±Æ÷ÏÂÒ»¸ö³¬Ê±Ê±¼ä¼ä¸ô
		if(msg.get())
		{
			TEventMsg* pEventMsg = NULL;
			pEventMsg=dynamic_cast<TEventMsg*>(msg.get());
			if(pEventMsg)
			{
				CCode scode;
				switch(pEventMsg->eventID)
				{
					case KERNAL_APP_INIT:
						if(pEventMsg->status)
						{
							UniINFO("FrameMng: init task ok: taskid=%d, instid=%d",pEventMsg->sender.taskID,  pEventMsg->sender.instID);
						}
						else
						{
							UniERROR("FrameMng: init task error: taskid=%d, instid=%d",pEventMsg->sender.taskID,  pEventMsg->sender.instID);
						}
						break;
					case KERNAL_APP_HEATBEAT:

						break;
					case KERNAL_APP_RELOAD:
						char fstr[1024];
						if(pEventMsg->status == 1)
						{
							sprintf(fstr, "taskid: %d, instid: %d env reload ok", pEventMsg->sender.taskID, pEventMsg->sender.instID);
							scode.content = fstr;
						}
						else
						{
							sprintf(fstr, "taskid: %d, instid: %d env reload failed", pEventMsg->sender.taskID, pEventMsg->sender.instID);
							scode.content = fstr;
						}
						scode.length = strlen(scode.content);
						socket.sendCode(scode);
						UniINFO(scode.content);
						break;
					case KERNAL_APP_SHUTDOWN:
						break;
					case KERNAL_APP_STATUES:
						scode.content = (char *)pEventMsg->eventInfo.c_str();
						scode.length = strlen(scode.content);
						socket.sendCode(scode);
						UniINFO("watch status of taskid:%d, instid:%d", pEventMsg->sender.taskID, pEventMsg->sender.instID);
						UniINFO(scode.content);
						break;
					default:
						break;
				}
			}

		}

		//check if recieved new command
		memset(mSockBuffer, 0, sizeof(mSockBuffer));
		code.content = mSockBuffer;
		code.length = 0;
		int recvstatus = socket.recvCode(code);
		if(recvstatus == 1 &&code.length >0)
		{
			Command * cmd = (Command *) code.content;
			//debug out
			//cout<< "taskid: "<< cmd->taskId<<endl;
			//cout<< "instid: "<< cmd->instId<<endl;
			//cout<< "command: "<< cmd->cmd<<endl;

			TEventMsg* cmdmsg=NULL;
			task=NULL;
			mInstList.reset();
			mInstList.current(task);
			while(task)
			{
				//find the task
				if(cmd->taskId == -1 || (task->taskID == cmd->taskId && cmd->instId == -1) || (task->taskID == cmd->taskId && task->instID == cmd->instId))
				{
					mTransID++;
					cmdmsg=new TEventMsg();
					switch(cmd->type)
					{
						case 0:
							cmdmsg->eventID= KERNAL_APP_RELOAD;
							break;
						case 1:
						default:
							cmdmsg->eventID= KERNAL_APP_STATUES;
							break;
					}
					cmdmsg->transID=mTransID;
					cmdmsg->status=0;
					cmdmsg->eventInfo += cmd->cmd;
					cmdmsg->sender.taskType=0;
					cmdmsg->sender.taskID=0;
					cmdmsg->sender.instID=0;
					task->task->add(cmdmsg);
					cmdmsg=NULL;
				}

				task=NULL;
				mInstList.next(task);
			}
		}
	}
}


