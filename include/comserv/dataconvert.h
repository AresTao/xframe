#ifndef _DATACONVERT_H 
#define  _DATACONVERT_H 

using namespace std;

//���ݱ�������
enum TEncodingType
{
	data_encoding_default=0,
	data_encoding_ascii=1,
	data_encoding_ucs2=8,	//�൱��unicode16
	data_encoding_utf8=9,	//�Զ��壨ֻ���м���Ϣ�д��ڡ���SMPPʱ�ᱻת����gbk��ucs2��
	data_encoding_gb18030=15,	//GBK=GB18030, 18030��ͬ��GBK������������ʱ����ת����������������15��
	data_encoding_gb2312=13,	//�Զ��壬GBK���Ӽ�������������ʱ����ת����������������15��
	data_encoding_gb13000=14,	//�Զ��壬GBK���Ӽ�������������ʱ����ת����������������15��
	data_encoding_big5=17,
};

char* itoa_character_set(TEncodingType type_uint);
TEncodingType atoi_character_set(const char* type_char);
/////////////////////////////
// �ַ����뷽ʽת��
/////////////////////////////
//���������
//	TEncodingType srcEncoding��	Դ��������
//   char*         srcContent ��		Դ�ַ�������
//	unsigned int 	    srcLength:			Դ�ַ�������

//	TEncodingType destEncoding��	��Ҫת���ɵ�Ŀ���������
//	char*  		destContent: 		Ŀ�껺������ָ��
//    unsigned int 	   	&destLength:  		Ŀ�껺�����Ĵ�С

//���������
//  char*         destContent:  ת������ַ���������ָ��
//  unsigned int 		   &destLength:  ת������ַ�������

//����ֵ��
//      1:  �ɹ�
//      0: ʧ��
//	   ע��: ת��ʧ��destLength�ᱻ��Ϊ0
int dataEncodingConvert(TEncodingType srcEncoding,char* srcContent,const unsigned int srcLength,TEncodingType destEncoding,char* destContent,unsigned int &destLength);

/////////////////////////////
// �ַ���ӡ
/////////////////////////////
int dataEncodingPrint(char* srcBuf, int srcLen, TEncodingType encoding, char* destBuf, int& dstLen);

#endif
