#ifndef _GENERALOBJ_H_
#define _GENERALOBJ_H_

#include "comtypedef_vchar.h"
#include "comtypedef.h"
#include "comconst.h"

_CLASSDEF(TGeneralObject);
class TGeneralObject
{
    public:
        TGeneralObject();
        virtual      ~TGeneralObject(){};
        TObjectType  getObjectType();
        CHAR*        getObjectTypeStr();
        void         putObjectType(TObjectType type);
        const CHAR*  getObjectNameStr(){ objectName.c_str();}


        virtual const char*  objectTypeName() { return "general"; }

        int                  logType;
        int                  logLevel;

    protected:
        TObjectType  objectType;     //组件类型，如General, Kernal, Task, PSA
        CStr			objectName;		//组件名字
};

#endif

