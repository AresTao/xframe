#ifndef _CFRAMECONTROL_H
#define _CFRAMECONTROL_H

#include <stdio.h>
#include "generalobj.h"
#include "comtypedef.h"
#include "msgdef.h"
#include "msg.h"
#include "fifo.h"
#include "taskselector.h"

#define __FRAMECONTROL_TASKID 1

_CLASSDEF(TGeneralObject);
_CLASSDEF(TFRAMEControl);
_CLASSDEF(AbstractTask);
_CLASSDEF(TTaskSelector);

class TFRAMEControl:public TGeneralObject
{
	public:
		TFRAMEControl(Fifo<TMsg>&	framemngFifo);
		~TFRAMEControl();
		//TFRAMEControl& operator=(TFRAMEControl&);
		void    init(int logT, int logL) { logType=logT; logLevel=logL;};

		BOOL	add(TMsg* msg);	//�ⲿ����Ϣ����FrameCtrl
		BOOL	post(TMsg* msg);	//��FrameMng����Ϣ
		BOOL	postTo(TMsg* msg);	//����Task��FrameMng����Ϣ

		TTaskSelector* getTaskSelector();//{return mTaskSelector;}
		void	printStat(){};
		void	printMsgs(TUniNetMsg * Msg);

	public:
		//ϵͳ�ڲ����ýӿ�
		BOOL	mainLoop();

	private:
		UINT	loopCount;
		UINT	uninetmsgCountS;
		UINT	uninetmsgCountF;
		BOOL	shutdown;

		BOOL	onInit(UINT transid);
		BOOL	onHeartBeat(UINT transid);
		BOOL	onReload(UINT transid);
		BOOL	onShutdown(UINT transid);
		BOOL 	onStatues(UINT transid, CStr& eventinfo);

		TTaskSelector*  		mTaskSelector;
		Fifo<TMsg>*			mFifo; 		//����Ϣ���л���
		Fifo<TMsg>&			mPostFifo;	//����Event��Ϣ����OAM ��Ϣ��ʱ�򣬷��͵���fifo����������framemng ��ʼ����������framemng ������Ϣ

};

#endif


