/****************************************************************
 * Copyright (c)2011, by BUPT
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code

 * FileName:    framectrl.h $
 * System:       xframe
 * SubSystem:    common
 * Author:       Li Jinglin
 * Date��        2010.4.4
 * Description:
        �ڶ��߳�ģʽ�£�FrameControl ֻ��� Team / Task������Feam / Psa��������ʱ��������Ϣת����
		��ǰʱ���ȡ�ȹ��ܣ������и�ģ���̵߳Ĺ���
		FrameControl ��ά������Task��msgBuffer���������task�ṩframeCtrl��msgBuffer��
		����Task�м����̴߳������ⲿ���̹߳���ģ����ɡ�

		framectrl�ڶ��߳�ģʽ�£���һ���̣߳���ѵ���msgqueue�û��Ƕ���̣߳�ÿ���߳�������msgqueue�ã�
		���ܵ���Ҫ���Բ��ܾ�����

****************************************************************/
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

