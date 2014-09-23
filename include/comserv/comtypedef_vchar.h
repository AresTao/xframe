 /******************************************************************************
 *Copyright(c)2005, by MT2
 *All rights reserved

 *FileName:   comtypedef_vchar.h
 *System:     UniFrame
 *SubSystem:
 *Author:     Li Jinglin
 *Date��     2005.11.14
 *Version��     4.0
 *Description��
      CVarCharXX�Ķ���

 *
 * Last Modified:
    2005-11-13 v1.1 ���  �����ԭ����commsgtype.hͷ�ļ���
   2005-11-18 v1.2 ���  ��Ҫusing namespace std;
   2005-11-23 v1.3 ���  �޶�strstreamͷ�ļ�����֤vc6���ܹ�����ȷ����;
   2005-11-29 v1.4 ��� ��ͷ�ļ��޸�Ϊ<strstream >�����ֱ����� ���ܱ���ͨ�� 
   2006-04-19 v1.5 �������� ��ENCODE��DECODE�Ⱥ��Ƶ�msg.h�С�
   2006-04-26 v1.6 �������� �޸�==���������ʵ�֣���friend��Ϊ��Ƕ������ʽ��
                            ����size()ʵ���еĴ���  
    2009-02-28 ������. ��������c_str(),length()�ȷ�����������ʹ�ã�
               ����operator=���������ַ�����CVarCharxxֱ�Ӹ�ֵ��    

 * ԭʼ��¼
 * Description��   defined common str type used by msg.
 *               ����뺯��ʵ�����ļ� commsgtype.c ��
 *  2003.11.13, ��ɳ�ʼ�汾
 *     By         Shuang Kai
 *
 *Last Modified:
 *  2003.11.26,��ɸ��ɱ䳤�ַ�������ķ�����ʵ��
 *       By      Peng Jin
 *    2003.12.23, ������using namespace std; �У��޸��������ǰ׺��BYTE��m_uc��ͷ�� CHAR ��m_c��ͷ
       By       Shuangkai
 *  2003.12.24, ����CVarChar���GetVarCharLen��Ա������Ϊ����Ա����
 *      By      PengJin
 *  2004.02.09, ���ظ�CVarChar���"=="�����
 *      By      PengJin
 *  2004.03.02, �������print�����Ĳ������ʹ�ostringstream�޸�Ϊostrstream
 *      By      PengJin
 *   2004.03.23 by PengJin
 *            ��ΪHP�Ĳ��ֲ�Ʒ��ISUPЭ��ջ������֧��-AA����ѡ��ʽ����ļ��е������ռ��Ƴ�
 *   2004.04.07 by Shuangkai
 *            �޸� ENCODE DECODE �ĺ궨��
 *   2004.04.09 by Yeli
 *            �����е� GetVarCharContent ��������Ч����ֵ�� return NULL; ��Ϊ return 0;
 *            ���� g++ ����ʱ�� warning
 *   2004.04.13 by PengJin
 *            �����е�SetVarCharContent(CHAR*,INT)��m_cVarCharContent����Ч�����ַ�����һ�ַ�����Ϊ�ս��
 *  2004.06.05 by Shuangkai
 *            ��������Ĺ��캯�������������� m_cVarCharContent[0] = '\0';
 *  2007.07.29 by Long Xiangming.
 *              Ϊÿ��������SetVarCharContent(CHAR*)����
 * 
 **********************************************************************/



#ifndef  _COMTYPEDEF_VCHAR_H
#define  _COMTYPEDEF_VCHAR_H

#include <strstream>    //added by ljl 2005-11-29 ostrstream��׼�⣬VC��G++��MinGW��֧��  
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include "comtypedef.h"

_CLASSDEF(CFixedChar)
using namespace std;

/* ˵����
   1. CVarChar8,CVarChar16,...,CVarChar256��CVarChar���Զ���ɱ䳤�ַ����顣
      ����CVarChar8����ַ�����Ϊ8��CVarChar16��󳤶�Ϊ16.�������ơ� ��CVarChar256��󳤶�Ϊ255����Ϊ�ڲ�len����ΪBYTE�����ȡֵΪ255����
     ��CVarChar��Ҫ���ڶ��峬��255���ַ���������󳤶Ȳ��ܳ���MAX_VCHAR_LENGTH(comconst.h���壬ĿǰΪ8192��
   2. CVarCharXX �� CVarChar ֧����'\0'��β���ַ�����ͬʱҲ֧���м���'\0'���ַ����飬����UCS-2������ַ���
   3. ��ֵ��ʽ��
      �����ָ�ֵ��ʽ��1������set() 2�� ֱ��ʹ��=�Ÿ�ֵ��
     CVarCharXX��CVarChar�����˲�����=, ��ֵ������CHAR*, CFixedChar,�Լ�CVarChar8,...,CVarChar256,CVarChar.
     
     ���ǣ�����CVarCharNN = CVarCharMM��NN����>=MM��
     ���磬CVarChar32 c1; CVarChar16 c2;
     c1 = c2; ������ģ���c2 = c1���ǲ�����ġ���������ʹ��c2 = c1.c_str()�ķ�ʽ���и�ֵ������c2��󳤶ȵĲ��ֻᱻ�ضϣ�

     ���⣬�������ֵ�ĳ��ȳ���������󳤶ȣ���ô����Ĳ��ֱ��ضϣ����ڵ�length+1���ַ�λ�ûᲹ'\0'����˵���c_str()�Խ����һ���н�β������Ч�ַ�����
     
     
     ����ֵ��CHAR* ����ʱ��
      ����UCS-2���м���ܳ���'\0'���ַ����飬��ֵʱ����ʹ��set(CHAR* str,INT len)��len������ʽ�Ĵ��롣
     ��������'\0'��β���ַ���������set(CHAR* str)���ɣ�����ֱ��ʹ�� = �Ÿ�ֵ��
     
  4. ȡֵ��ʽ��
     1������ַ�����ָ�룺c_str(); �൱��ԭ����GetVarCharContentPoint().
    2) ����ַ������ȣ�length()���൱��ԭ����GetVarCharLen();
   
  5. �����ʱ�������size()���ֽ�. 
     ����CVarCharXX: size = length() + 1; (ǰ����һ��BYTE������Ϊ���ȱ�־��
     ����CVarChar:   size = length() + 4; (ǰ����һ��INT������Ϊ���ȱ�־��


  eg.
     CHAR* s[20];
    CVarChar16 vchar;
    strcpy(s,"123456");

    �������¸�vchar��ֵ��
    vchar = s;
    ��vchar.set(s);
    ��vchar.set(s,strlen(s));
    ��vchar.SetVarCharContent(s);
    ��vchar.SetVarCharContent(s,strlen(s));

    ��ʱ������vchar.length()����6��

     ���ǣ����s�м����'\0'������UCS-2�ȱ���)�����£�
    s[0]='1'; s[1]= '\0'; s[2]='2';
    vchar.set(s,3);
    ��ʱ������vchar.length()����3. vchar��ǰ�����ַ��ֱ�Ϊ'1','\0','2'. ��4���ַ�Ϊ��β��'\0';
 */


/**
 * Class��      CVarChar8
 * Author:            Shuang Kai
 * Parent Class��
 * Date:             2003.11.13
 * Description:     �ɱ䳤�ַ�������-CVarChar8��
 * Last Modified:
    2003.11.26,         ��ɳ�ʼ���塣
                 By Peng Jin
 * Notes:
    ��������8���ַ�������Ϊ�ַ������

 */

class CVarChar;
class CVarChar8
{
private:
   BYTE m_ucVarCharLen;      //��������Ч�ַ��ĳ���
   CHAR *m_cVarCharContent;    //��СΪ9,������8����Ч�ַ�����9���ַ�Ϊ'\0'


public:

   CVarChar8();
   CVarChar8(const CVarChar8 &r);
   CVarChar8 &operator=(const CVarChar8 &r);
   CVarChar8 &operator=(const CHAR* r); //added by lxm.2009-02-26�� ����ֱ�Ӹ�ֵΪ�ַ���
   CVarChar8 &operator=(const CFixedChar &r);
   CVarChar8 &operator=(const CVarChar &r);
   ~CVarChar8(){if(m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
    
   //��Windows�µ���COMPARE_FORCE_VCHAR��ʱ�����������friend��ʽ�ĳ���Ƕ����ʽ��
   //friend BOOL operator == (const CVarChar8&, const CVarChar8&);
   BOOL operator == (const CVarChar8&);

    INT GetVarCharLen() const;
    void SetVarCharLen(INT);

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-26
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);

    INT size(  ) const ;//���ڱ���룬��length()��1���ֽ�

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);
};


/**
 * Class��      CVarChar16
 * Author:            Shuang Kai
 * Parent Class��
 * Date:             2003.11.13
 * Description:     �ɱ䳤�ַ�������-CVarChar16��
 * Last Modified:
    2003.11.26,         ��ɳ�ʼ���塣
                 By Peng Jin
 * Notes:
    ��������16���ַ�������Ϊ�ַ������

 */


class CVarChar16
{
private:
   BYTE m_ucVarCharLen;
   CHAR *m_cVarCharContent;

public:


    CVarChar16();
   ~CVarChar16(){if(m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
   CVarChar16(const CVarChar16 &r);
   CVarChar16 &operator=(const CVarChar16 &r);
   CVarChar16 &operator=(const CHAR* r);
   CVarChar16 &operator=(const CFixedChar &r);
   CVarChar16 &operator=(const CVarChar8 &r);
   CVarChar16 &operator=(const CVarChar &r);

   //friend BOOL operator == (const CVarChar16&, const CVarChar16&);
   BOOL operator == (const CVarChar16&);

    INT GetVarCharLen() const;
    void SetVarCharLen(INT);

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-27
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);

    INT size(  ) const ;

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);

};


/**
 * Class��      CVarChar32
 * Author:            Shuang Kai
 * Parent Class��
 * Date:             2003.11.13
 * Description:     �ɱ䳤�ַ�������-CVarChar32��
 * Last Modified:
    2003.11.26,         ��ɳ�ʼ���塣
                 By Peng Jin
 * Notes:
    ��������32���ַ�������Ϊ�ַ������

 */

class CVarChar32
{
private:
   BYTE m_ucVarCharLen;
   CHAR *m_cVarCharContent;

public:

    CVarChar32();
   ~CVarChar32(){if(m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
   CVarChar32(const CVarChar32 &r);
   CVarChar32 &operator=(const CVarChar32 &r);
   CVarChar32 &operator=(const CHAR *r);
   CVarChar32 &operator=(const CFixedChar &r);
   CVarChar32 &operator=(const CVarChar8 &r);
   CVarChar32 &operator=(const CVarChar16 &r);
   CVarChar32 &operator=(const CVarChar &r);

   //friend BOOL operator == ( const CVarChar32&, const CVarChar32&);
   BOOL operator == (const CVarChar32&);

    INT GetVarCharLen() const;
    void SetVarCharLen(INT);

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-27
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);


    INT size(  ) const ;

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);

};


/**
 * Class��      CVarChar64
 * Author:            Shuang Kai
 * Parent Class��
 * Date:             2003.11.13
 * Description:     �ɱ䳤�ַ�������-CVarChar64��
 * Last Modified:
    2003.11.26,         ��ɳ�ʼ���塣
                 By Peng Jin
 * Notes:
    ��������64���ַ�������Ϊ�ַ������

 */

class CVarChar64
{
private:
   BYTE m_ucVarCharLen;
   CHAR *m_cVarCharContent;

public:

    CVarChar64();
   ~CVarChar64(){if(m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
   CVarChar64(const CVarChar64 &r);
   CVarChar64 &operator=(const CVarChar64 &r);
   CVarChar64 &operator=(const CHAR* r);
   CVarChar64 &operator=(const CFixedChar &r);
   CVarChar64 &operator=(const CVarChar8 &r);
   CVarChar64 &operator=(const CVarChar16 &r);
   CVarChar64 &operator=(const CVarChar32 &r);
   CVarChar64 &operator=(const CVarChar &r);

   //friend BOOL operator == (const CVarChar64&, const CVarChar64&);
   BOOL operator == (const CVarChar64&);

    INT GetVarCharLen() const;
    void SetVarCharLen(INT);

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-27
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);


    INT size(  ) const ;

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);

};


/**
 * Class��      CVarChar128
 * Author:            Shuang Kai
 * Parent Class��
 * Date:             2003.11.13
 * Description:     �ɱ䳤�ַ�������-CVarChar128��
 * Last Modified:
    2003.11.26,         ��ɳ�ʼ���塣
                 By Peng Jin
 * Notes:
    ��������128���ַ�������Ϊ�ַ������

 */

class CVarChar128
{
private:
   BYTE m_ucVarCharLen;
   CHAR *m_cVarCharContent;


public:

   CVarChar128();
   ~CVarChar128(){if(m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
   CVarChar128(const CVarChar128 &r);
   CVarChar128 &operator=(const CVarChar128 &r);
   CVarChar128 &operator=(const CHAR* r);
   CVarChar128 &operator=(const CFixedChar &r);
   CVarChar128 &operator=(const CVarChar8 &r);
   CVarChar128 &operator=(const CVarChar16 &r);
   CVarChar128 &operator=(const CVarChar32 &r);
   CVarChar128 &operator=(const CVarChar64 &r);
   CVarChar128 &operator=(const CVarChar &r);

   //friend BOOL operator == ( const CVarChar128&,  const CVarChar128&);
   BOOL operator == (const CVarChar128&);

    INT GetVarCharLen() const;
    void SetVarCharLen(INT);

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-27
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);


    INT size(  ) const ;

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);

};


/**
 * Class��      CVarChar256
 * Author:            Shuang Kai
 * Parent Class��
 * Date:             2003.11.13
 * Description:     �ɱ䳤�ַ�������-CVarChar256��
 * Last Modified:
    2003.11.26,         ��ɳ�ʼ���塣
                 By Peng Jin
 * Notes:
    ��������256���ַ�������Ϊ�ַ������

 */

class CVarChar256
{
private:
   //INT m_ucVarCharLen;
   BYTE  m_ucVarCharLen;
   CHAR *m_cVarCharContent;

public:

   CVarChar256();
   ~CVarChar256(){if(m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
   CVarChar256(const CVarChar256 &r);
   CVarChar256 &operator=(const CVarChar256 &r);
   CVarChar256 &operator=(const CHAR* r);
   CVarChar256 &operator=(const CFixedChar &r);
   CVarChar256 &operator=(const CVarChar8 &r);
   CVarChar256 &operator=(const CVarChar16 &r);
   CVarChar256 &operator=(const CVarChar32 &r);
   CVarChar256 &operator=(const CVarChar64 &r);
   CVarChar256 &operator=(const CVarChar128 &r);
   CVarChar256 &operator=(const CVarChar &r);

   //friend BOOL operator == (const CVarChar256&, const CVarChar256&);
   BOOL operator == (const CVarChar256&);

    INT GetVarCharLen() const;
    void SetVarCharLen(INT);

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-26
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);


    INT size() const ;

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);

};


/**
 * Class��      CVarChar
 * Author:            LJL
 * Parent Class��
 * Date:             2007.1.9
 * Description:     �ɱ䳤�ַ�������-CVarChar��
 * Last Modified:
 * Notes:
       ���޳������������Ϊ�ַ�ָ��ķ�װ�����ڻ�������һ��Ӧ��
*/

class CVarChar
{
private:
   //ֻ��CVarChar��m_ucVarCharLenΪINT���ͣ�����CVarChar8...CVarChar256�ȶ�ʹ��BYTE���͡�
   INT m_ucVarCharLen;
   CHAR *m_cVarCharContent;

public:

    CVarChar();
   ~CVarChar(){if (m_cVarCharContent!=NULL) delete[] m_cVarCharContent;}
   CVarChar(const CVarChar &r);
   CVarChar &operator=(const CVarChar &r);
   CVarChar &operator=(const CHAR* r);
   CVarChar &operator=(const CFixedChar &r);
   CVarChar &operator=(const CVarChar8 &r);
   CVarChar &operator=(const CVarChar16 &r);
   CVarChar &operator=(const CVarChar32 &r);
   CVarChar &operator=(const CVarChar64 &r);
   CVarChar &operator=(const CVarChar128 &r);
   CVarChar &operator=(const CVarChar256 &r);

   BOOL operator == (const CVarChar&);

    INT GetVarCharLen() const;      //��û��������ݳ���
    void SetVarCharLen(INT);         //���Խ������ܳ��Ļ������ض̣����ǲ��ܽ��̻���������

    CHAR GetVarCharContent(INT);
    void SetVarCharContent(INT,CHAR);

    CHAR*  GetVarCharContentPoint() const;
    void SetVarCharContent(const CHAR*,INT);   //���û���������
    void SetVarCharContent(const CHAR*);

    //added by LXM. 2009-02-27
    CHAR* c_str() const; //�൱��GetVarCharContentPoint
    INT  length() const;//�൱��GetVarCharLen
    void set(const CHAR*,INT);//�൱��SetVarCharContent(const CHAR*,INT);
    void set(const CHAR*);//SetVarCharContent(const CHAR*);


   INT size(  ) const ;      //������� vchar ���볤��

   INT encode( CHAR* &buff ) const;
   INT decode( CHAR* &buff );

   void print(ostrstream &st);
};

class CStr
{
   public:
      CStr();
      CStr(const char* str);
      CStr(const CStr& a);
      CStr(int value);
      ~CStr();

      void operator=(const char* str);
      void operator=(int value);
      void operator=(const CStr& str);
      int  operator==(const char* str) const;
      int  operator==(const CStr& str) const;
      int  operator!=(const char* str) const;
      int  operator!=(const CStr& str) const;

      virtual int  operator>(const char* str) const;
      virtual int  operator>(const CStr& str) const;
      virtual int  operator<(const char* str) const;
      virtual int  operator<(const CStr& str) const;
      virtual int  operator<=(const char* str) const;
      virtual int  operator<=(const CStr& str) const;
      virtual int  operator>=(const char* str) const;
      virtual int  operator>=(const CStr& str) const;

      CStr& operator+=(const CStr& d);
      CStr& operator+=(const char*);
      CStr& operator+=(const char c);

      CStr& operator<<(const char*);
      CStr& operator<<(int);
      CStr& operator<<(CStr&);

      int split(CStr* argvArray, int maxArgc) const;
      int split(CStr* argvArray, int maxArgc, const char* sep) const;
      int merge(CStr* argvArray, int argc);

      void setByte(char c, int pos);
      char getByte(int pos);
      const char* cut(int length);
      void cut(const char* sep, CStr& result);
      const char* trim();
      const char* trimAll(); // delete all char not isalnum()
      void clear();

      HASH_CODE hash();

      // return the content and clean this
      char* steal(int& len);

      // the most short complement integer
      CStr& intCatShort(int i);

      // fix 4 byte integer
      CStr& intCat4(int i);

      // 1xxxxxxx 1xxxxxxx ... 0xxxxxxx
      CStr& intCat7(int i);

      // unicode integer to utf8 format string
      CStr& intCatUtf8(int i);

      CStr& nCat(const char* str, int length);
      CStr& fCat(const char* fmt, ...);
      CStr& fCatV(const char* fmt, va_list args);
      CStr& fCatBin(const char* content, int length);
      CStr& fCatBin2(const char* content, int length, int indent);
      CStr& fCatShortBin(const char* content, int length);
      CStr& fCatTitle(const char* title, char fillChar, int length);
      CStr& fCatChars(char fillChar, int length);
      CStr& fileCat(const char* fileName);
      CStr& replace(const char* pattern, const char* replace);

      CHAR* GetVarCharContentPoint() const;
      void SetVarCharContent(INT num, CHAR character);
      void SetVarCharContent(const CHAR*,INT);
      void getFieldName(const char** p, int& arrayIndex);
      const char* c_strList() const;
      inline const char* c_str() const
      {
         m_buffer[m_length]=0;
         return m_buffer;
      }
      int toInt() const {  return atoi(m_buffer); }
      inline int length() const { return m_length; }
      int empty() const {return length()==0;}

      CStr& toLower();
      CStr& toUpper();
      CStr& htmlEncode();
      CStr& uricEncode();
      CStr& uricDecode();
      CStr& iconv(const char* from, const char* to);

      void print(ostrstream &st);
      virtual int decode(char* &buf);
      virtual int encode(char* &buf) const;
      virtual int size() const;
      void set(const char*, int length);

   protected:
      void resize(int);

      int m_length;
      int m_capacity;
      char* m_buffer;
#define MaxCStrBufferLen 32
      char _m_buffer[MaxCStrBufferLen];
};
HASH_CODE _GetHash(CStr);

class CStrShort : public CStr
{
public:
   int decode(char* &buf);
   int encode(char* &buf) const;
   int size() const;
};

class CStrInt : public CStr
{
public:
   int  operator>(const char* str) const;
   int  operator<(const char* str) const;
   int  operator<=(const char* str) const;
   int  operator>=(const char* str) const;

   int  operator>(const CStr& str) const;
   int  operator<(const CStr& str) const;
   int  operator<=(const CStr& str) const;
   int  operator>=(const CStr& str) const;
};

//���������ͣ�����ʱ��Ҫ�ж�size�Ƿ�Խ�磨�յ���code�е�length���ܳ���������󳤶�)
//comtypedef_vchar.C��ʹ��
#ifndef CHECK_DECODE_VCHAR_SIZE
#define CHECK_DECODE_VCHAR_SIZE(length,max_array_size)\
{\
   if(length>max_array_size || length<0) \
   {\
      UniERROR("decode CVarChar%d error: The length(%d) is out of the max array size(%d).",max_array_size,length,max_array_size);\
      length = 0;\
      return 0;\
   }\
}
#define CHECK_DECODE_VCHAR_SIZE_U(length,max_array_size)\
{\
   if(length>max_array_size) \
   {\
      UniERROR("decode CVarChar%d error: The length(%d) is out of the max array size(%d).",max_array_size,length,max_array_size);\
      length = 0;\
      return 0;\
   }\
}
#endif

//����ʱ���������鶨��ʱ�ĳ��ȣ��ж�length�Ƿ�Խ��
#ifndef CHECK_DECODE_ARRAY_SIZE
#define CHECK_DECODE_ARRAY_SIZE(length,array_deflen,array_varname,array_type)\
{\
   if(length>array_deflen || length<0) \
   {\
      UniERROR("decode %s %s[] array error: The length(%s=%d) is out of the max array size(%d).",#array_type,#array_varname,#length,length,array_deflen);\
      length = 0;\
      return 0;\
   }\
}
#endif

//���������ͣ�����ʱ��Ҫ�ж�size�Ƿ�Խ�磨�յ���code�е�length���ܳ���������󳤶�)
//�����������󳤶�ΪMAX_ARRAY_SIZE(1024)
#ifndef CHECK_MAX_ARRAY_SIZE
#define CHECK_MAX_ARRAY_SIZE(length,decodename)\
{\
   if(length>MAX_ARRAY_SIZE || length<0) \
   {\
      UniERROR("decode array(%s) error: The length(%d) is out of the max array size(%d).",#decodename,length,MAX_ARRAY_SIZE);\
      length = 0;\
      return 0;\
   }\
}
#define CHECK_MAX_ARRAY_SIZE_U(length,decodename)\
{\
   if(length>MAX_ARRAY_SIZE) \
   {\
      UniERROR("decode array(%s) error: The length(%d) is out of the max array size(%d).",#decodename,length,MAX_ARRAY_SIZE);\
      length = 0;\
      return 0;\
   }\
}
#endif

#define ENCODE_INT( BUFFER, VAR ) \
{\
   UINT iTemp = htonl(VAR);\
   memcpy( BUFFER, &iTemp, sizeof(VAR) ); \
   BUFFER += sizeof(VAR);\
}

#define DECODE_INT( VAR, BUFFER ) \
{\
   UINT iTemp = 0;\
   memcpy( &iTemp, BUFFER, sizeof(VAR) ); \
   VAR = ntohl(iTemp);\
   BUFFER += sizeof(VAR);\
}

#define ENCODE( BUFFER, VAR ) \
{\
   memcpy( BUFFER, &VAR, sizeof(VAR) ); \
   BUFFER += sizeof(VAR);\
}

#define DECODE( VAR, BUFFER ) \
{\
   memcpy( &VAR, BUFFER, sizeof(VAR) ); \
   BUFFER += sizeof(VAR);\
}

#define ENCODE_ARRAY( BUFFER, ARRAY, LENGTH ) \
{\
   memcpy( BUFFER, ARRAY, sizeof(ARRAY[0])*LENGTH ); \
   BUFFER += sizeof(ARRAY[0])*LENGTH;\
}

#define DECODE_ARRAY( ARRAY, BUFFER, LENGTH )\
{\
   CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAY);\
   memcpy( ARRAY, BUFFER, sizeof(ARRAY[0])*LENGTH ); \
   BUFFER += sizeof(ARRAY[0])*LENGTH;\
}
// for length is unsigned char
#define DECODE_ARRAY_U( ARRAY, BUFFER, LENGTH )\
{\
   memcpy( ARRAY, BUFFER, sizeof(ARRAY[0])*LENGTH ); \
   BUFFER += sizeof(ARRAY[0])*LENGTH;\
}
#endif

