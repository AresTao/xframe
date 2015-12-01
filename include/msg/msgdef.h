#if !defined __MSGDEF_H
#define __MSGDEF_H

#include <strstream>

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msg.h"
#include "msgdatadef.h"
#include "msgutil.h"
#include "msgnamedef.h"

_CLASSDEF(CGFSM)
    
    using namespace std;


    typedef UINT  TMsgType;           //��Ϣ���͵��������Ͷ���. ��TMsgType����ͬ.
    typedef BYTE  TDialogType;        //�Ի����͵��������Ͷ���

    const TDialogType DIALOG_MESSAGE      =0;	//֪ͨ����Ϣ�����ᴴ���Ի���һ��һ��
    const TDialogType DIALOG_BEGIN        =1;	//�����Ի���Ϣ
    const TDialogType DIALOG_CONTINUE     =2;	//�Ի�����Ϣ
    const TDialogType DIALOG_END          =3;	//�����Ի���Ϣ
    const TDialogType DIALOG_BROADCAST    =4;	//�㲥����Ϣ��������Task��ʵ���ַ�
    const TDialogType DIALOG_MULTICAST    =5;	//�ಥ����Ϣ������ͬTask��ʵ���ַ�

    /*======================================================*/
    //ͨ�ÿ�ѡ��������������жϺ�
    //ʹ�øú���Ҫע���������⣺
    //1����Ϣͷ����Ϣ�����б�����������Сд��ĸ��ͷ�������ִ����ڶ�������֮��ĵ�������ĸ��д�����磺oAddr��tAddr
    //2����ѡ����һ������Ϊ optionSet
    //3����ѡ����Flag��ʶ����Ϊ ����ѡ������_flag�� �����磺oAddr_flag, tAddr_flag
    /*=======================================================*/
#ifndef HAS_OPTION_PARA
#define HAS_OPTION_PARA(ParaName)\
        (optionSet & ParaName##_flag)
#endif

#ifndef SET_OPTION_PARA
#define SET_OPTION_PARA(ParaName);\
        optionSet |= ParaName##_flag;
#endif

    /*======================================================*/
    // ��������Ϣ����꣬�Կ�����Ϣͷ����Ϣ�����
    // ��msglib_xx.C�б�����
    /*======================================================*/
#ifndef DECODE_CTRLHDR
#define DECODE_CTRLHDR(CTRLMSG)\
        if(msg->optionSet & TUniNetMsg::ctrlMsgHdr_flag )\
{\
    P##CTRLMSG  ctrlMsg = new CTRLMSG;\
    ctrlMsg->decode(buf);\
    msg->ctrlMsgHdr = ctrlMsg;\
}
#endif

#ifndef DECODE_MSGBODY
#define DECODE_MSGBODY(MSGBODY)\
        if(msg->optionSet & TUniNetMsg::msgBody_flag )\
{\
    P##MSGBODY msgBody = new MSGBODY;\
    msgBody->decode(buf);\
    msg->msgBody = msgBody;\
}
#endif

    //==========================================================
    // ��Ϣ�ඨ��

    /*======================================================*/
    //ͨ����Ϣ����
    //  ����SSCʹ�õ���Ϣ���������������ֹ��ɣ�ͨ����Ϣͷ��������Ϣͷ����Ϣ��
    //������Ϣͷ��������Ự��������Ҫ�õ���һЩ�ؼ���������һЩ��ʶ��
    //��IDֵ��
    /*=======================================================*/
    _CLASSDEF(TUniNetMsg);
    _CLASSDEF(TCtrlMsg);
    _CLASSDEF(TMsgBody);

    class TUniNetMsg :public TMsg
{
    public:
        UINT              optionSet;    //��ѡ������ָʾ

        TMsgAddress       oAddr;        //Դ��ַ��ȱʡ������д
        TMsgAddress       tAddr;        //Ŀ�ĵ�ַ��������дĿ�Ķ��߼���ַ��дĿ����ģ��/ģ���ʶ��
        TDialogType       dialogType;     //DIALOG_BEGIN�Ի���ʼ, DIALOG_CONTINUE�Ի�������, DIALOG_END�Ի�����
        TUniNetMsgType    msgName;      //��Ϣ��
        TMsgType          msgType;      //��Ϣ����

        PTCtrlMsg         ctrlMsgHdr;   //������Ϣͷ
        PTMsgBody         msgBody;      //��Ϣ��

        enum
        {
            ctrlMsgHdr_flag  =0x00000001,  //����������Ϣͷ
            msgBody_flag     =0x00000004   //������Ϣ��
        };

        inline              TUniNetMsg();
        inline              TUniNetMsg(const TUniNetMsg &r);
        virtual inline      ~TUniNetMsg();
        inline              TUniNetMsg &operator=(const TUniNetMsg &r);

        inline UINT         hasCtrlMsgHdr();
        inline UINT         hasMsgBody();
        inline UINT         setCtrlMsgHdr();
        inline UINT         setMsgBody();

        inline CHAR*        getMsgNameStr();
        inline PTMsg    	  clone();         
        inline PTMsgPara    cloneCtrlMsg();

        BOOL                operator==(TUniNetMsg&);

        INT                 encode(CHAR* &buf);
        INT                 decode(CHAR* &buf);
        INT                 size();
        void                print(ostrstream& st);

        //���������msglib.C�б����ã�ֻ�������TMsg����Ϣͷ(oAddr,tAddr,dialogType,msgName,msgType������)��
        //��ctrlMsg��msgBody����Ҫ���������������ɣ���msglib.C)
        virtual BOOL        decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

};

/*======================================================*/
//������Ϣͷ
//  ������Ϣͷ��������Ự��������Ҫ�õ���һЩ�ؼ���������һЩ��ʶ��
//��IDֵ��
//
//���Ŀǰ������Ϣͷ�е�indexID������������Ϣ�������Ҫ��������ӿڿ���������չ
//������Ϣͷ����չ�ķ���Ϊ�̳�TCtrlMsg��
//
//TxxCtrlMsg :public TCtrlMsg
//{
//public:
//  xxId=;
//}
//
//ͬʱ���ڹ���TUniNetMsg��ʱ�򣬿�����Ϣͷʹ��TxxCtrlMsg���ɵĶ���
/*=======================================================*/
class TCtrlMsg :public TMsgPara
{
    public:
        UINT              optionSet;    //��ѡ������ָʾ
        TMsgAddress	    orgAddr;

        inline TCtrlMsg()
        {
            optionSet=0;
        };

        virtual ~TCtrlMsg(){};  //�������������������ģ����ܱ�֤��������ʱ���ܹ������������������
        enum {
            orgAddr_flag  =0x00000001,  //???
            session_flag  =0x00000002
        };
        inline UINT       hasOrgAddr();
        inline UINT       setOrgAddr();
        virtual CHAR*     getMsgNameStr(){ return getMsgName();}
        virtual CHAR*     getMsgName(){ return "UniNet CtrlMsg";}

        virtual INT       encode(CHAR* &buf);
        virtual INT       decode(CHAR* &buf);
        virtual BOOL      decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);
        virtual INT       size();
        virtual void      print(ostrstream& st);

        virtual PTCtrlMsg clone()=0; //��������ķ���ָ������ǻ����
        PTMsgPara         cloneMsg(){ return clone();}
        virtual BOOL      operator==(TMsgPara&){ return FALSE;}
};

/*======================================================*/
//��Ϣ��
//  ��Ϣ������Ϣ�ĺ������ݣ�ÿһ����Ϣ��������һЩ����
//
//  ÿ����Ϣ��Ķ��嶼����̳�TMsgBody����ʵ����������Ĵ��鷽����
//
//TxxMsg :public TMsgBody
//{
//public:
//  xxId=;
//}
//
//ͬʱ���ڹ���TUniNetMsg��ʱ����Ϣ��ʹ��TxxMsg���ɵĶ���
/*=======================================================*/
class TMsgBody :public TMsgPara
{
    public:
        UINT              optionSet;    //��ѡ������ָʾ           //Modify by LJL 2006-3-21

        TMsgBody()
        {
            optionSet=0;        //Modify by LJL 2006-3-21
        };

        virtual ~TMsgBody(){}; ////�������������������ģ����ܱ�֤��������ʱ���ܹ������������������

        virtual INT        encode(CHAR* &buf)=0;
        virtual INT        decode(CHAR* &buf)=0;
        virtual BOOL       decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm){return FALSE;}
        virtual INT        size()=0;
        virtual void       print(ostrstream& st)=0;
        virtual CHAR*      getMsgNameStr(){ return getMsgName();}
        virtual CHAR*      getMsgName(){ return "UniNet Msg";}

        virtual PTMsgBody  clone()=0;
        virtual BOOL       operator==(TMsgPara&){return FALSE;}

        PTMsgPara          cloneMsg(){ return clone();}

};


/*******************************************************************/
// clone ��¡����
//  ��¡������������Ϊ�������Ϣ��ĸ��ƣ�
//  ��¡����û������
//  ��¡���������Ϊ �������͵�ָ�룬��ָ��ָ�����������͵������
//    ������������ȫһ�¡�
/*******************************************************************/

/*======================================================*/
//��Ϣ�ṹ�ľ���ʵ��
/*======================================================*/
//TUniNetMsg��ʵ��
inline TUniNetMsg::TUniNetMsg()
{
    dialogType=DIALOG_BEGIN;
    msgName=0;
    msgType=1;    //ȡֵΪ0ʱ��ʾ����ϢΪ��ҪKernel�������Ϣ����SP��ҵ�������Ϣ
    optionSet=0;  //ȱʡ������������Ϣͷ����Ϣ��

    ctrlMsgHdr=NULL;
    msgBody=NULL;

}

inline TUniNetMsg::TUniNetMsg(const TUniNetMsg &r)
{
    sender=r.sender;

    oAddr=r.oAddr;
    tAddr=r.tAddr;
    dialogType=r.dialogType;
    msgName=r.msgName;
    msgType=r.msgType;

    optionSet=r.optionSet;

    if(hasCtrlMsgHdr())
    {
        ctrlMsgHdr=r.ctrlMsgHdr->clone();
    }
    else
    {
        ctrlMsgHdr=NULL;
    }

    if(hasMsgBody())
    {
        msgBody=r.msgBody->clone();
    }
    else
    {
        msgBody=NULL;
    }

}


inline TUniNetMsg::~TUniNetMsg()
{
    if(hasCtrlMsgHdr() && NULL!=ctrlMsgHdr)
    {
        delete ctrlMsgHdr;
    }

    if (hasMsgBody() && NULL!=msgBody)
    {
        delete msgBody;
    }
    ctrlMsgHdr=NULL;
    msgBody=NULL;
}


inline TUniNetMsg & TUniNetMsg::operator=(const TUniNetMsg &r)
{
    sender = r.sender;

    oAddr=r.oAddr;
    tAddr=r.tAddr;
    dialogType=r.dialogType;
    msgName=r.msgName;
    msgType=r.msgType;

    optionSet=r.optionSet;

    if(hasCtrlMsgHdr())
    {
        if (ctrlMsgHdr!=NULL)
        {
            delete ctrlMsgHdr;
            ctrlMsgHdr=NULL;
        }
        if(r.ctrlMsgHdr!=NULL) ctrlMsgHdr=r.ctrlMsgHdr->clone();
    }
    else
    {
        ctrlMsgHdr=NULL;
    }

    if(hasMsgBody())
    {
        if (msgBody!=NULL)
        {
            delete msgBody;
            msgBody=NULL;
        }
        if(r.msgBody!=NULL) msgBody=r.msgBody->clone();
    }
    else
    {
        msgBody=NULL;
    }
    return *this;
}

inline CHAR* TUniNetMsg::getMsgNameStr()
{
    if(NULL==msgBody)
    {
        if(msgName == KERNAL_APP_INIT)
            return "KERNAL_APP_INIT";
        return "unknown";
    }
    return msgBody->getMsgNameStr();
}

inline UINT TUniNetMsg::hasCtrlMsgHdr()
{
    return optionSet&ctrlMsgHdr_flag;
}

inline UINT TUniNetMsg::hasMsgBody()
{
    return optionSet&msgBody_flag;
}

inline UINT TUniNetMsg::setCtrlMsgHdr()
{
    return optionSet |= ctrlMsgHdr_flag;
}

inline UINT TUniNetMsg::setMsgBody()
{
    return optionSet |= msgBody_flag;
}


inline PTMsg  TUniNetMsg::clone()
{
    PTUniNetMsg msg = new TUniNetMsg();
    *msg = *this;
    //CtrlHdr��MsgBody�Ѿ��ڿ������캯�����渳��ֵ�ˣ������ٸ�ֵ�ͻ��ڴ�й©
    return msg;
}


inline PTMsgPara  TUniNetMsg::cloneCtrlMsg()
{
    if(hasCtrlMsgHdr())
    {
        if(ctrlMsgHdr!=NULL)
        {
            return ctrlMsgHdr->clone();
        }
    }
    return NULL;
}

inline UINT TCtrlMsg::hasOrgAddr()
{
    return optionSet&orgAddr_flag;
}

inline UINT TCtrlMsg::setOrgAddr()
{
    return optionSet |=orgAddr_flag;
}

#endif
