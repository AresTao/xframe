#ifndef __COMTYPEDEF_FCHAR_H__
#define __COMTYPEDEF_FCHAR_H__

#include "comtypedef.h"
#include <strstream>

using namespace std;

_CLASSDEF(CVarChar8)
_CLASSDEF(CVarChar16)
_CLASSDEF(CVarChar32)
_CLASSDEF(CVarChar64)
_CLASSDEF(CVarChar128)
_CLASSDEF(CVarChar256)
_CLASSDEF(CVarChar)

class CFixedChar
{
private:
	INT  m_length; //´Ë×Ö·û´®µÄÊµ¼Ê³¤¶È£¬ÒÔ'\0'½áÎ²ÅÐ¶Ï
	INT  m_size;   //´Ë×Ö·ûÊý×éµÄ¹Ì¶¨×î´ó³¤¶È

	CHAR *m_content;

public:

	CFixedChar();
	CFixedChar(INT size);
	~CFixedChar(); 
	CFixedChar(const CFixedChar &r);
	CFixedChar &operator=(const CFixedChar &r);
	CFixedChar &operator=(const CHAR* r);
	CFixedChar &operator=(const CVarChar8 &r);
	CFixedChar &operator=(const CVarChar16 &r);
	CFixedChar &operator=(const CVarChar32 &r);
	CFixedChar &operator=(const CVarChar64 &r);
	CFixedChar &operator=(const CVarChar128 &r);
	CFixedChar &operator=(const CVarChar256 &r);
	CFixedChar &operator=(const CVarChar &r);

	BOOL operator == (const CFixedChar&);

	BOOL setSize(INT size);//ÖØÐÂ·ÖÅä¿Õ¼ä
	INT size() const ;	//´Ë×Ö·ûÊý×éµÄ¹Ì¶¨×î´ó³¤¶È£¬±à½âÂëÓÃ
	INT length() const ;	//´Ë×Ö·ûÊý×éµÄÊµ¼Ê³¤¶È£¬ÒÔ'\0'½áÎ²ÅÐ¶Ï

	CHAR* c_str() const; 
	void set(const CHAR*,INT);
	void set(const CHAR*);

	INT encode( CHAR* &buff ) const;
	INT decode( CHAR* &buff );

	void print(ostrstream &st);

};

#endif
