#ifndef __DEFS_
#define __DEFS_

#define _DATATYPEDEF(type,name) \
            typedef type name; \
            typedef name* P##name;\
            typedef name& R##name;\
            typedef name*& RP##name;

#define _ENUMTYPEDEF(name) \
            typedef name* P##name;\
            typedef name& R##name;\
            typedef name*& RP##name;

#define _ARRAYTYPEDEF(type,name,size) \
            typedef type name[size];\
            typedef name* P##name;\
            typedef name& R##name;\
            typedef name*& RP##name;

#define _CLASSDEF(name) \
            class name; \
            typedef name  * P##name;\
            typedef name  & R##name;\
            typedef name  *  & RP##name;\
            typedef const name  * PC##name;\
            typedef const name  & RC##name;

#endif

