#include "msghelper_com.h"
#include "cmd5.h"


//Convert Message Type
int COMMsgHelper::log_Level(CStr& s)
{
	if(s=="LOG_DEBUG")
	return  0;
	else if(s=="LOG_INFO")
	return 1;
	else if(s=="LOG_NOTICE")
	return 2;
	else if(s=="LOG_WARNING")
	return 3;
	else if(s=="LOG_ERR")
	return 4;
	else if(s=="LOG_CRIT")
	return 5;
	else if(s=="LOG_ALERT")
	return 6;
	else if(s=="LOG_EMERG")
	return 7;
	else return 1;
}

void COMMsgHelper::addMsgBody(PTUniNetMsg uniMsg, CStr& msg)
{
  TComMsg* msgbody=NULL;
  if(uniMsg==NULL) return;
  if(uniMsg->hasMsgBody())
  {
        delete uniMsg->msgBody;
	uniMsg->msgBody=NULL;
  }
 
  msgbody=new TComMsg();
  uniMsg->msgBody = msgbody;
  uniMsg->setMsgBody();
  msgbody->comMsg=msg;
}

void COMMsgHelper::addCtrlMsgHdr(PTUniNetMsg uniMsg, INT st, INT pr, INT fa)
{
  TComCtrlMsg* comctrl=NULL;
  if(uniMsg==NULL) return;
  if(uniMsg->hasCtrlMsgHdr())
  {
	delete uniMsg->ctrlMsgHdr;
	uniMsg->ctrlMsgHdr=NULL;
  }
  
  comctrl = new TComCtrlMsg();
  uniMsg->ctrlMsgHdr=comctrl;
  uniMsg->setCtrlMsgHdr();
  comctrl->status = st; 
  comctrl->pri 	  = pr; 
  comctrl->facility = fa; 
}

TComCtrlMsg* COMMsgHelper::getComMsgCtrlHdr(PTUniNetMsg unimsg)
{
	if(!unimsg) return NULL;
        if(!unimsg->hasCtrlMsgHdr()) return NULL;
	
  	TComCtrlMsg* comctrl=NULL;
        comctrl=dynamic_cast<TComCtrlMsg*>(unimsg->ctrlMsgHdr);
         if(!comctrl) return NULL;	
	return comctrl;
}



TComMsg*  COMMsgHelper::getComMsgBody(PTUniNetMsg unimsg)
{
	if(!unimsg) return NULL;
	if(!unimsg->hasMsgBody()) return NULL;  

	TComMsg* combody=NULL;
	combody=dynamic_cast<TComMsg*>(unimsg->msgBody);
	if(!combody) return NULL;
	
	return combody;
}
