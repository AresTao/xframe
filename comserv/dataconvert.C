#include <stdio.h>
#include "func.h"
#include "dataconvert.h"

#ifndef WIN32
extern "C"
{
#include <iconv.h>
}
#endif

char* itoa_character_set(TEncodingType type_uint)
{
	switch (type_uint)
	{
		case data_encoding_ascii:
			return "ASCII";
		case data_encoding_ucs2:
			return "UCS-2BE";
		case data_encoding_utf8:
			return "UTF-8";
		case data_encoding_gb18030:
			return "GB18030";
		case data_encoding_default:
			return "GB18030";		
		default:
			return "UNKNOWN";

	}
}

TEncodingType atoi_character_set(const char* type_char)
{
	if(type_char==NULL) return data_encoding_default;
	char temp[64];
	strcpy(temp,type_char);
	if(strnotcasecmp(temp,"GB18030")==0)
		return data_encoding_gb18030;
	else if(strnotcasecmp(temp,"GBK")==0)
		return data_encoding_gb18030;
	else if(strnotcasecmp(temp,"UCS-2")==0)
		return data_encoding_ucs2;
	else if(strnotcasecmp(temp,"UTF-8")==0)
		return data_encoding_utf8;
	else if(strnotcasecmp(temp,"ASCII")==0)
		return data_encoding_ascii;
	else
		return data_encoding_default;
	
}

int dataEncodingConvert(TEncodingType srcEncoding,char* srcContent,const unsigned int srcLength,TEncodingType destEncoding,char* destContent,unsigned int &destLength)
{
#ifndef WIN32
	iconv_t cd;					  /* conversion descriptor */
	size_t ret_val;				  /* number of conversions */
	size_t inbytesleft=srcLength;
	size_t outbytesleft=destLength; 

	if ((NULL==srcContent)||(NULL==destContent))
	{
		destLength=0;
		return 0;
	}

	/* Initiate conversion -- get conversion descriptor */
	if ((cd = iconv_open(itoa_character_set(destEncoding), itoa_character_set(srcEncoding))) == (iconv_t)-1) 
	{
		perror("iconv_open() error:");
		destLength=0;
		return 0;
	}

	ret_val = iconv(cd, &srcContent, &inbytesleft, &destContent, &outbytesleft);

	if (ret_val<0)
	{
		perror("iconv() error:");
		destLength=0;
		return 0;
	}

	if (iconv_close(cd)<0)
	{
		perror("iconv_close() error:");
	}

	destLength = destLength-outbytesleft;
	return 1;
#else
	return 0;
#endif
}


int dataEncodingPrint(char* srcBuf, int srcLen, TEncodingType encoding, char* destBuf, int& dstLen)
{
	switch(encoding)
	{
		case data_encoding_ucs2:
			if( srcLen%2 > 0 ) return 0;
			dstLen=0;
			for(int i=0;i<srcLen/2;i++)
			{
				sprintf(destBuf,"%%u%02x%02x",(unsigned char)srcBuf[i],(unsigned char)srcBuf[i+1]);
				i=i+2;
				dstLen+=6;
			}
			return 1;
		default:
			return 0;
	}
	return 0;
}



