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

		BOOL	add(TMsg* msg);	//外部发消息到邋FrameCtrl
		BOOL	post(TMsg* msg);	//向FrameMng发消息
		BOOL	postTo(TMsg* msg);	//各个Task向FrameMng发消息

		TTaskSelector* getTaskSelector();//{return mTaskSelector;}
		void	printStat(){};
		void	printMsgs(TUniNetMsg * Msg);

	public:
		//系统内部调用接口
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
		Fifo<TMsg>*			mFifo; 		//收消息队列缓冲
		Fifo<TMsg>&			mPostFifo;	//发送Event消息和邋OAM 消息的时候，发送到该fifo，该引用由framemng 初始化，用于向framemng 反馈消息

};

#endif


