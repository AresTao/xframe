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
#include "msgdef_com.h"
#include "msghelper_com.h"

using namespace std;
enum LOGTYPE { MSGINFO=0, COUNTINFO};

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

        void  postEvent(TEventMsg* msg);
        void  postEvent(std::auto_ptr<TEventMsg> msg);

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

        void sysLog(CStr &id, int logLevel,CStr &logInfo, int logType=MSGINFO);
        void sysLog(const char* id, int logLevel, const char* logInfo, int logType=MSGINFO);
        void sysLog(const char* id, int logLevel, CStr &logInfo, int logType=MSGINFO);
        //void print(const char* fmt, ...);

        //////////////////////////////////
        ///     ��Ա������ֱ�ӷ���.
        //////////////////////////////////
        UINT getTaskId(){return mTaskId;}
        UINT getHostId(){return mHostId;}
        UINT getInstId() { return mInstId; }
        CStr getTaskName() { return mTaskName; }
        CDB* getDB(int i) {return mDB[i];}


        CStr& getBuildInfo() {return buildInfo;}
        inline void setBuildInfo(char* baseversion, char* buildversion, char* buildby, char* buildtime)
        {
            getBuildInfo().fCat("BaseVersion: %s\r\n",baseversion);
            getBuildInfo().fCat("BuildVersion:%s\r\n",buildversion);
            getBuildInfo().fCat("BuildBy:     %s\r\n",buildby);
            getBuildInfo().fCat("BuildTime:   %s\r\n",buildtime);
        }


    public:

        time_t currentTime;
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
        void onStart(TEventMsg* msg);
        void onShutdown(TEventMsg* msg);
        void onReload(TEventMsg* msg, CStr& st);
        void onHeartBeat(TEventMsg* msg);
        void onStatues(TEventMsg* msg, CStr& st);

        Fifo<TMsg>* 	mTaskRxFifo; 	//���͵�Task����Ϣ���л���

    private:

        BOOL  post(TMsg* msg);

        CStr buildInfo;

        UINT  mTaskId;
        UINT  mHostId;
        UINT  mInstId;
        CStr  mTaskName;

        UINT  mMsgProcCount;	//�ⲿ��Ϣ����
        UINT  mTimerProcCount;	//��ʱ����Ϣ����
        UINT  mInnerProcCount;	//�ڲ���Ϣ����

        TaskTimerQueue* mTTQ;			//�ڲ���ʱ������
        TDBRsc			mDB;		//ÿ������Ĭ�����ò�����5 �����ݿ����ӣ���ͨ��mDB[i] �ķ�ʽ�����ʣ�i �������ļ������õ����ݿ����id
        TTimerRsc 		mTME;		//ÿ������Ĭ�����ò�����30����ʱ��

        TFRAMEControl* 	mFrameCtrl;	//����������
        int		logToSyslog;
        int		syslogTaskID;
};

#define _SETBUILDINFO setBuildInfo(BASEVERSION, BUILDVERSION, BUILDBY, BUILDTIME);


#endif
