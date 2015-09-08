/****************************************************************
 * Copyright (c)2011, by BUPT
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code

 * FileName:    taskselector.h $
 * System:       xframe
 * SubSystem:    common
 * Author:       Li Jinglin
 * Date��        2010.4.4
 * Description:
		���ڹ���task������task�ļ��ء�ע�ᡢ��Ϣ�ַ�����ȡ�

****************************************************************/
#if !defined(TaskSelector_H)
#define TaskSelector_H

#include <stdio.h>

#include "comtypedef.h"
#include "unihashtable.h"
#include "clist.h"
#include "fifotimelimit.h"
#include "msgdef.h"

_CLASSDEF(TAbstractTask);
_CLASSDEF(TTaskSelector);

class TTaskSelector
{
   public:
      TTaskSelector();	//new selector��ʱ��Ҫ����FrameCtrl��Task sendTo Fifo
      ~TTaskSelector();
	  void init();

	  BOOL  add(TUniNetMsg* msg);	//������Ϣ
      BOOL  add(std::auto_ptr<TUniNetMsg> msg);
	  UINT 	registerTask(UINT taskid, UINT instid, TAbstractTask* task);
	  void 	shutdownTask(UINT taskid);
	  void 	shutdownInst(UINT instid);
      void 	unregisterTask(UINT taskid);		//unregisterֻ����ts�еĸ��ɷֵ�������Ķ���ʵ���� framemng���й���
	  void 	unregisterInst(UINT instid);

      INT 	getActiveTaskCounter() { return activeTaskCounter;}
      INT 	getRegisteredTaskAmount() { return registeredTaskAmount;}

      void	printState();

   private:
	  BOOL  sendMsgToTask(TUniNetMsg* msg);
	  BOOL  multicastMsgToTask(TUniNetMsg* msg);
	  BOOL  broadcastMsgToTask(TUniNetMsg* msg);

      struct Item
      {
            Item(){};
            //Item(int tid, TAbstractTask* ptu) : taskid(tid), tu(ptu), shuttingDown(false) {}
            Item(UINT tid, UINT insid, TAbstractTask* ptu) : taskid(tid), instid(insid), tu(ptu), shuttingDown(false) {}
			UINT 			instid;
            UINT 			taskid;
            TAbstractTask* 	tu;
			UINT			count;
            BOOL 			shuttingDown;
            BOOL operator==(const Item& rhs) { return instid == rhs.instid; }
      };

	  struct TaskState
	  {
			TaskState(){};
			TaskState(UINT tid, CList<UINT>* ts):taskid(tid),inst(ts), instpos(0){};

			UINT 			taskid;
			UINT 			instpos;
			CList<UINT>* 	inst;
			BOOL operator==(const TaskState& rhs) { return taskid == rhs.taskid; }
	  };
		//���taskid>0, instid=0
		//    ����task �ж��Inst ����˸÷������ص���һ�����ո��ɷֵ�ѡ���Inst ���
		//    �ڱ��ĳ��inst ���� shutdown ״̬��ʱ���򲻷��ظ�ʵ������֤shutdown ��ʵ�������ٴ����µĶԻ�����
		//���taskid>0, instid>0
		//    �Ѿ���ȷinst ������һ���Ǻ�����󣬻���ѡ����Ϊshutdown��ʵ������֤ʵ���ѺõĽ���
      TTaskSelector::Item*	getTask(UINT taskid, UINT instid);

      BOOL	exists(UINT taskid);

      //CHashTable<int, Item> mTaskList;	//�����б�����ΪTaskID������ΪItem������
      CHashTable<UINT, Item*> 		mInstList;	//�����б�����ΪInstanceID������ΪItem������
      CHashTable<UINT, TaskState*> 	mTaskList;  //����TaskId ����������б�����ΪTaskID������ΪItem����������

      INT  	activeTaskCounter;		//�����������
	  INT  	activeInstCounter;
	  INT  	registeredTaskAmount;	//ע�����������
	  INT  	registeredInstAmount;	//ע���ʵ������

      INT  	sendMsgToTaskCount;
      INT  	multicastMsgCount;
      INT 	broadcastMsgCount;
};

#endif
