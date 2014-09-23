/****************************************************************
 * Copyright (c)2011, by BUPT
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code

 * FileName��      abstracttask.h
 * System:       xframe
 * SubSystem��     Kernel_Task
 * Author:       Li Jinglin
 * Date��        2010.4.4
 * Version��      1.0
 * Description��
      ע�⣬TAbstractTask���ṩ������Ӧ��ʵ�ֵ��ϲ�Application�Ľӿڡ�
	  TAbstractTask�ṩ������process�������̵߳��ã�ʹ��time limit fifo������Ϣ��

********************************************************************/

#ifndef _TABSTRACTTASK_H
#define _TABSTRACTTASK_H

#include <strstream>

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "fifo.h"
#include "framectrl.h"
#include "generalobj.h"
#include "msgdef.h"
#include "msg.h"
#include "fifosorted.h"
#include "env.h"
#include "info.h"
#include "timerqueue.h"

using namespace std;

_CLASSDEF(TFRAMEControl);
_CLASSDEF(TAbstractTask);
class TAbstractTask : public TGeneralObject
{
public:
	TAbstractTask();
	virtual ~TAbstractTask();


	//===============================================================================================
	//�̳е�Ӧ�ÿ�����һ�·�������ʵ��Ӧ���߼�
	//================================================================================================
	//ϵͳ��ʼ��֮����أ����������ļ���Ӧ����չ������Ϣ��ָ�룬Ӧ�����ж�ȡ�����浽������
	virtual BOOL onInit(TiXmlElement*	extend) { return TRUE;}
	//Task ������ɼ��أ�������Ϣ֮ǰ
	virtual BOOL preTaskStart() {return TRUE;}
	//Task ����ر�֮ǰ
	virtual BOOL preTaskShutdown() { return FALSE;}
	//���¼������ã����������ļ���Ӧ����չ������Ϣ��ָ�룬Ӧ�����ж�ȡ�����浽������
	//  cmd:	����������У�extend: ����������ļ�ָ��)
	virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend) { return TRUE;}
	//��ȡTask��ǰ����״̬
	//  cmd:   ����������У�st: �����״̬
	virtual BOOL getStatues(CStr& cmd, CStr& st) {return TRUE;}

	//�����������һ����¡�������������ɶ��󸱱�
	virtual TAbstractTask* clone()=0;
	//ʾ����TAbstractTask* newTask=new TXXTask(mHostId, mTaskId, mFrameCtrl);

	//=================================================================================================
	//������Ϣ������
	//    ������Ϣ����������Ϣ���ͺ���
	//procMsg
	//	��Ϣ��������������Ҫ����֮����ʵ����Ϣ�����ܡ�
	//��������ǽ��յ���msg
	//
	//ʹ��procMsg(TUniNetMsg* msg) Ҫע�⣬�����msg ������Ϊ��Ϣ���ݣ��ٷ��ͳ�ȥ����Ϊ���ָ��ᱻ�����ͷ�
	//     �����Ҫʹ��ת����������procMsg(std::auto_ptr<TMsg>& msg) �Ǹ����ʵ�ѡ����ת����Ϣ��ʱ�򣬵���msg.release() �ͷ��Զ�ָ�����Ϣָ��Ŀ���Ȩ
	virtual void procMsg(TUniNetMsg* msg);
//	{
//        UniINFO("Task recieved Msg\n");
//        UniINFO("%d---->%d\n",msg->oAddr.logAddr,msg->tAddr.logAddr);
//	}
	virtual void procMsg(std::auto_ptr<TUniNetMsg> msg);
//	{
//		TUniNetMsg* pUniNetMsg = msg.get();
//		if(pUniNetMsg) procMsg(pUniNetMsg);
//	}

	//sendMsg
	// 	����һ����Ϣ��
	//�������
	//	Ӧ��Ҫ����TMsg msg����
	// 	msg���󽫻ᱻ�ں��Զ����٣��ϲ�Ӧ����Ҫ��֤ÿ�η�����
	// 	msg��һ���µĶ����Ҳ�����������֮��
	//ʹ��sendMsg(TUniNetMsg* msg) Ҫע�⣬msgһ����Ҫnew��������ָ�룬��Ҫֱ����procMsg �����msg ָ�룬��������ָ����Զ��ͷŵ�
	//    ���ʹ��sendMsg(std::auto_ptr<TMsg> msg) ���򲻱ص��ģ���Ϊ�Զ�ָ�븴�ƻ������ƽ�ָ�����Ȩ
	void  sendMsg(TUniNetMsg* msg);
	void  sendMsg(std::auto_ptr<TUniNetMsg> msg);


	//��ʱ����������
	//
	//setTimer
	//	����һ����ʱ�����˷���ֱ��ʹ�á������������ء�
	//�������:
	//    timerId		Ϊ��ʱ��id��ָʾͨ�������ļ����õĶ�ʱ����Ϣ��
	//    timerDelay	Ϊ��ʱʱ�䣬��λΪ���롣��Я��delay����������ļ����Զ���ȡʱ�ӡ�
	//	para			�û��Զ�����������û�У����贫��˲���
	//����ֵ:
	//	timerKey		��ʱ����Ψһ��ʶ����̵Ĳ�����ͨ���ö�ʱ����ʶִ�С�
	TTimerKey setTimer(TTimeMark timerId, TTimeDelay timerDelay, void* para=NULL);
	TTimerKey setTimer(TTimeMark timerId, void* para=NULL);

	//delTimer
	//	����timerKeyɾ��һ����ʱ����
	BOOL delTimer(TTimerKey timerKey);

	//onTimeOut
	//	 ��ʱ����ʱ��������
	//���������:
	//    timerKey		��ʱ����ʶ��setTimer��ʱ�򷵻صı�ʶ
	//	para			�û��Զ������
	virtual void onTimeOut(TTimerKey timerKey, void* para);


   //////////////////////////////////
   ///     ��Ա������ֱ�ӷ���.
   //////////////////////////////////
   UINT getTaskId(){return mTaskId;}
   UINT getHostId(){return mHostId;}
   UINT getInstId() { return mInstId; }
   CDB* getDB(int i) {return mDB[i];}


public:

	/////////////////////////////////////////
	//	�ڲ�����������ʹ��
	/////////////////////////////////////////

	//��framemngģ����ã��߳�����ǰ�Ķ����ʼ��
	BOOL init(UINT instid, UINT hostid, UINT taskid, TFRAMEControl* framectrl);

	//���̹߳���ģ����ã��̵߳���ѭ��
	void  process();

	//��FrameCtrl ģ����ã�����������Ϣ
	//Ӧ�ÿ���д����ӿڣ���ʵ�����ض�λ��
	virtual void  add(TMsg* msg) { mTaskRxFifo->add(msg); }

protected:
	// �ض��¼�����
	void onStart(UINT transid);
	void onShutdown(UINT transid);
	void onReload(UINT transid, CStr& st);
	void onHeartBeat(UINT transid);
	void onStatues(UINT transid, CStr& st);

    Fifo<TMsg>* 	mTaskRxFifo; 	//���͵�Task����Ϣ���л���

private:

	BOOL  post(TMsg* msg);

	UINT  mTaskId;
    UINT  mHostId;
    UINT  mInstId;

    UINT  mMsgProcCount;	//�ⲿ��Ϣ����
    UINT  mTimerProcCount;	//��ʱ����Ϣ����
    UINT  mInnerProcCount;	//�ڲ���Ϣ����

    TaskTimerQueue* mTTQ;			//�ڲ���ʱ������
	TDBRsc			mDB;		//ÿ������Ĭ�����ò�����5 �����ݿ����ӣ���ͨ��mDB[i] �ķ�ʽ�����ʣ�i �������ļ������õ����ݿ����id
 	TTimerRsc 		mTME;		//ÿ������Ĭ�����ò�����30����ʱ��

	TFRAMEControl* 	mFrameCtrl;	//����������
};

#endif
