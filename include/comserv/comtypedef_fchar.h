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
        INT  m_length; 
        INT  m_size;   

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

        BOOL setSize(INT size);
        INT size() const ;	
        INT length() const ;	

        CHAR* c_str() const; 
        void set(const CHAR*,INT);
        void set(const CHAR*);

        INT encode( CHAR* &buff ) const;
        INT decode( CHAR* &buff );

        void print(ostrstream &st);

};

#endif
