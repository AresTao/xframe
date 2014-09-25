#ifndef   _MD5_H
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

		//pucText : 输入的字节数组的其实地址；
		//iLength : 输入的字节数组的长度； 
		//    如果 pucText 是一个字符数组（C风格的字符串），iLength不包括结尾处的'\0'

		//获得摘要
		//   摘要有两种方式，内部使用数字摘要(16byte字符串，每Byte是一个二进制数字取值)，网络传送字符串摘要(32byte字符串，每一个Byte是一个AscII码)
		//   type=0 数字摘要
		//   type=1 字符串摘要
		UCHAR* GetDigest(UCHAR *pucText, INT iLength, INT type=0);    //Modify by LJL,2009-8-10

		//检查收到的摘要是否匹配
		//   type=0，输入的是数字摘要
		//   type=1，输入的是字符串摘要
		BOOL CheckDigest(UCHAR *pucText, INT iLength, UCHAR *pucDigest, INT type=0);   //Modify by LJL,2009-8-10
};

#endif
