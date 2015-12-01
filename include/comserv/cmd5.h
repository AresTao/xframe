#ifndef _MD5_H
#define _MD5_H

#include <string>
#include <cassert>

#include "comtypedef.h"

class CMD5
{
    private:
        UCHAR m_ucDigest[17];
        UCHAR m_hxDigest[33];   //Added by LJL,2009-8-10

        void CvtHex(const UCHAR *Bin, UCHAR *Hex);

    public:
        CMD5();
        ~CMD5();

        UCHAR* GetDigest(UCHAR *pucText, INT iLength, INT type=0);    //Modify by LJL,2009-8-10

        BOOL CheckDigest(UCHAR *pucText, INT iLength, UCHAR *pucDigest, INT type=0);   //Modify by LJL,2009-8-10
};

#endif
