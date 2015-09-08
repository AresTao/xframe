#ifndef __MSGNAMEDEF_H_
#define __MSGNAMEDEF_H_

#include "comtypedef.h"

//===========================================================================
//--------------------- AppInternal msgid ------------------------------------
const TUniNetMsgName  KERNAL_APP_HEATBEAT  = 11;
const TUniNetMsgName  KERNAL_APP_INIT      = 12;	//ϵͳ��ʼ�� 
const TUniNetMsgName  KERNAL_APP_STARTED   = 13; 	//��ʼ����ɣ�ϵͳ����
const TUniNetMsgName  KERNAL_APP_STATUES   = 14; 	
const TUniNetMsgName  KERNAL_APP_RELOAD    = 15; 
const TUniNetMsgName  KERNAL_APP_SHUTDOWN  = 16; 
												
//--------------------- OAM msgid ------------------------------------
const TUniNetMsgName OAM_SET_REQ    	= 70;
const TUniNetMsgName OAM_SET_ACK    	= 71;
const TUniNetMsgName OAM_GET_REQ		= 72;
const TUniNetMsgName OAM_GET_RESP		= 73;
const TUniNetMsgName OAM_ALERT_NOTIFY	= 74;

#endif
