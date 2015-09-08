/*****************************************************************************
 * msgdef_com.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Thu May 22 18:20:03 2014
.
 * 
 ******************************************************************************/
#include "msgdef_com.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TComCtrlMsg
/////////////////////////////////////////////
PTCtrlMsg TComCtrlMsg::clone()
{
	PTComCtrlMsg amsg = new TComCtrlMsg();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->status                    = status;
	amsg->pri                       = pri;
	return amsg;
}
TComCtrlMsg& TComCtrlMsg::operator=(const TComCtrlMsg &r)
{
	status                    = r.status;
	pri                       = r.pri;
	return *this;
}

BOOL TComCtrlMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TComCtrlMsg,msg)

	COMPARE_FORCE_INT(TComCtrlMsg,status)
	COMPARE_FORCE_INT(TComCtrlMsg,pri)

	COMPARE_END
}

INT TComCtrlMsg::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TComCtrlMsg::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	ENCODE_INT( buf , status )
	ENCODE_INT( buf , pri )

	return size();
}

INT TComCtrlMsg::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	DECODE_INT( status, buf )
	DECODE_INT( pri, buf )

	return size();
}

BOOL TComCtrlMsg::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TComCtrlMsg,status)
	FILL_FORCE_INT(TComCtrlMsg,pri)

	FILL_FIELD_END
}
void TComCtrlMsg::print(ostrstream& st)
{
	st << "TComCtrlMsg" << endl;
	st << getIndent() << "orgAddr    = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$status    = ";
	st << status << endl;
	st << getIndent() << "$pri       = ";
	st << pri << endl;
}

int TComCtrlMsg::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "status")
	{
		value = status; type = 2; return 1;
	}
	else if(fieldName == "pri")
	{
		value = pri; type = 2; return 1;
	}
	UniERROR("TComCtrlMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TComCtrlMsg::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "status")
	{
		status = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "pri")
	{
		pri = (INT)(value.toInt()); return 1;
	}
	UniERROR("TComCtrlMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TComMsg
/////////////////////////////////////////////
PTMsgBody TComMsg::clone()
{
	PTComMsg amsg = new TComMsg();
	amsg->comMsg                    = comMsg;
	return amsg;
}
TComMsg& TComMsg::operator=(const TComMsg &r)
{
	comMsg                    = r.comMsg;
	return *this;
}

BOOL TComMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TComMsg,msg)

	COMPARE_FORCE_VCHAR(TComMsg,comMsg)

	COMPARE_END
}

INT TComMsg::size()
{
	INT tmpSize = 0;

	tmpSize += comMsg.size();

	return tmpSize;
}

INT TComMsg::encode(CHAR* &buf)
{
	comMsg.encode(buf);

	return size();
}

INT TComMsg::decode(CHAR* &buf)
{
	comMsg.decode(buf);

	return size();
}

BOOL TComMsg::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TComMsg,comMsg)

	FILL_FIELD_END
}
void TComMsg::print(ostrstream& st)
{
	st << "TComMsg" << endl;
	st << getIndent() << "$comMsg = ";
	//st << "\""; comMsg.bprint(st); st << "\"" << endl;
	st << "\""; comMsg.print(st); st << "\"" << endl;
}

int TComMsg::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "comMsg")
	{
	//	value.set(comMsg.GetVarCharContentPoint(), comMsg.GetVarCharLen()); type = 4; return 1;
		value.set(comMsg.GetVarCharContentPoint(), comMsg.length()); type = 4; return 1;
	}
	UniERROR("TComMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TComMsg::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "comMsg")
	{
		comMsg.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TComMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

