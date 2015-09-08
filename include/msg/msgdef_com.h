#ifndef __MSGDEF_COM_H_
#define __MSGDEF_COM_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "comtypedef_fchar.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"
//#include "msgdef_uninet.h"
#include "msgdef.h"
_CLASSDEF(CGFSM);


#include <time.h>
#include <strstream>
using namespace std;


// TComMsg
const TUniNetMsgName COM_MESSAGE_REQ = 100;
const TUniNetMsgName COM_MESSAGE_RSP = 101;

class MSGDEF_COM_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case COM_MESSAGE_REQ: return "COM_MESSAGE_REQ";
			case COM_MESSAGE_RSP: return "COM_MESSAGE_RSP";
			default: return defaultName;
		};
	};
};


_CLASSDEF(TComCtrlMsg)
class TComCtrlMsg:public TCtrlMsg
{
	public:
		INT            status;
		INT            pri;
		INT            facility;
		const char* validPara() { return "status,pri,facility"; }

		inline         TComCtrlMsg();

		CHAR*          getMsgName(){ return (char*)"TComCtrlMsg";};
		TComCtrlMsg    &operator=(const TComCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_COM_H_IDName::n(id, "TComCtrlMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TComMsg)
class TComMsg:public TMsgBody
{
	public:
		CStr           comMsg;
		const char* validPara() { return "comMsg"; }

		inline         TComMsg();

		CHAR*          getMsgName(){ return (char*)"TComMsg";};
		TComMsg        &operator=(const TComMsg &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_COM_H_IDName::n(id, "TComMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TComCtrlMsg::TComCtrlMsg()
{
	status                    = 0;
	pri                       = 0;
}

inline TComMsg::TComMsg()
{
}


#endif
 
