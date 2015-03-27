#if !defined __MSGHELPER_COM_H
#define __MSGHELPER_COM_H

#include "msgdef_com.h"

_CLASSDEF(COMMsgHelper);
class COMMsgHelper
{
public:
	static int log_Level(CStr& s);
	static void addMsgBody(PTUniNetMsg uniMsg, CStr& msg);
	static void addCtrlMsgHdr(PTUniNetMsg uniMsg, INT st, INT pr,INT fa);
	static TComCtrlMsg*  getComMsgCtrlHdr(PTUniNetMsg unimsg);
	static TComMsg*      getComMsgBody(PTUniNetMsg unimsg);
    
};


#endif
