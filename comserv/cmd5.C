#include <memory.h>
#include <openssl/md5.h>

#include "cmd5.h"

CMD5::CMD5()
{
}

CMD5::~CMD5()
{}

UCHAR* CMD5::GetDigest(UCHAR *pucText, INT iLength, INT type)
{
    memset(m_ucDigest, 0, 17);
    MD5(pucText, iLength, m_ucDigest);

    //Modify by LJL,2009-8-10
    if(type)
    {
        CvtHex(m_ucDigest,m_hxDigest);      //Added by LJL,2009-8-10
        return m_hxDigest;
    }
    else
    {
        return m_ucDigest;
    }
}

BOOL CMD5::CheckDigest(UCHAR *pucText, INT iLength, UCHAR *pucDigest, INT type)
{
    assert(pucText != NULL);
    assert(pucDigest != NULL);

    UCHAR *pucOutput = GetDigest(pucText, iLength,type);

    //Modify by LJL,2009-8-10
    INT j=0;
    if(type)
    {
        j=32;
    }
    else
    {
        j=16;
    }

    for (INT i=0;i<j ;i++ )
    {
        if (pucOutput[i] != pucDigest[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

//Added by LJL,2009-8-10
void CMD5::CvtHex(const UCHAR *Bin, UCHAR *Hex)
{
    unsigned short i;
    unsigned char j;

    for (i = 0; i < 16; i++)
    {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
            Hex[i*2] = (j + '0');
        else
            Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0xf;
        if (j <= 9)
            Hex[i*2+1] = (j + '0');
        else
            Hex[i*2+1] = (j + 'a' - 10);
    }
    Hex[32]='\0';
}

