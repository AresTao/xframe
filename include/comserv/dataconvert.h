#ifndef _DATACONVERT_H 
#define  _DATACONVERT_H 

using namespace std;

enum TEncodingType
{
    data_encoding_default=0,
    data_encoding_ascii=1,
    data_encoding_ucs2=8,	
    data_encoding_utf8=9,	
    data_encoding_gb18030=15,	
    data_encoding_gb2312=13,	
    data_encoding_gb13000=14,	
    data_encoding_big5=17,
};

char* itoa_character_set(TEncodingType type_uint);
TEncodingType atoi_character_set(const char* type_char);
int dataEncodingConvert(TEncodingType srcEncoding,char* srcContent,const unsigned int srcLength,TEncodingType destEncoding,char* destContent,unsigned int &destLength);
int dataEncodingPrint(char* srcBuf, int srcLen, TEncodingType encoding, char* destBuf, int& dstLen);

#endif
