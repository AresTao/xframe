//comtypedef_fchar.h
//By LXM. 2009-02-26
#ifndef __COMTYPEDEF_FCHAR_H__
#define __COMTYPEDEF_FCHAR_H__

#include "comtypedef.h"

/**
 * Class��		CFixedChar
 * Author:         	Lxm
 * Parent Class��
 * Date:          	2009.02.26
 * Description:  	�̶������ַ�������-CFixedChar��
   ʹ��˵����
   1. CFixedChar��size�ǹ̶��ģ�ʹ��ǰӦ�õ���setSize()�趨��̶����ȡ�����ʹ��CFixedChar(int Size)���캯����
      CFixedChar���ǻ����size+1���ַ��ڴ�ռ䡣��size+1���ǻᱻ��Ϊ0.
   2. CFixedChar֧����'\0'��β���ַ�����ͬʱҲ֧���м���'\0'���ַ����飬����UCS-2������ַ���
   3. ��ֵ��ʽ��
      �����ָ�ֵ��ʽ��1������set() 2�� ֱ��ʹ��=�Ÿ�ֵ��
	  CFixedChar�����˲�����=, ��ֵ������CHAR*, CFixedChar,�Լ�CVarChar8,...,CVarChar256,CVarChar.
	  ���⣬�������ֵ�ĳ��ȳ�����size()����ô����Ĳ��ֱ��ضϣ���ʵ��lengthΪsize�����ڵ�size+1���ַ�λ�ûᲹ'\0'����˵���c_str()�Խ����һ���н�β������Ч�ַ�����
	  �������ֵ�ĳ���С��size(), ��ô�������'\0'���롣
	  
	  ���·ֱ�˵��:
	  A. ����ֵ��CHAR* ����ʱ��
      ����UCS-2���м���ܳ���'\0'���ַ����飬��ֵʱ����ʹ��set(CHAR* str,INT len)��len������ʽ�Ĵ��롣
	  ��������'\0'��β���ַ���������set(CHAR* str)���ɣ�����ֱ��ʹ�� = �Ÿ�ֵ��
	  eg.
	  CHAR* s="123456";
	  CFixedChar fchar;
	  fchar.setSize(21);//�̶�����Ϊ21.
	  ���¸�ֵ���ǿ��Եģ�
	  fchar.set(s);
	  or: fchar.set(s,strlen(s));
	  or: fchar = s;

      ��ʱ������fchar.size()����21������fchar.length()����6.  
     
	  B. ��ֵ��CVarCharXX��CFixedChar
	  ֱ����=�Ÿ�ֵ������CVarCharXXҲ��֧���м���'\0'���ַ�����ģ���ֵ�����ߵ�length��һ�µġ�
	  eg:
	  CVarChar32 vchar;
	  CFixedChar fchar;
	  fchar.setSize(20);
	  fchar = vchar;
	  
  4. ȡֵ��ʽ��
     1������ַ�����ָ�룺c_str();
	 2) ����ַ������ȣ�length()��
	 3��������̶����ȣ�size()��
	 length <= size;

  5. �����ʱ�����ǻ����size()���ֽڣ�����Ҳ��size()���ֽڡ�

 * Last Modified:
 * Notes:
 		ͨ������Э��ջ����SMGPЭ�顣
 */
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
	INT  m_length; //���ַ�����ʵ�ʳ��ȣ���'\0'��β�ж�
	INT  m_size;   //���ַ�����Ĺ̶���󳤶�

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

	BOOL setSize(INT size);//���·���ռ�
	INT size() const ;	//���ַ�����Ĺ̶���󳤶ȣ��������
	INT length() const ;	//���ַ������ʵ�ʳ��ȣ���'\0'��β�ж�

	CHAR* c_str() const; 
	void set(const CHAR*,INT);
	void set(const CHAR*);

	INT encode( CHAR* &buff ) const;
	INT decode( CHAR* &buff );

	void print(ostrstream &st);

};

#endif
