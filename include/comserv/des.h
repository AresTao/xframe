#ifndef	__DES_h__
#define	__DES_h__

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef U8
#define U8 unsigned char
#endif

#ifndef LPBYTE
#define LPBYTE uchar *
#endif 
///////////////////////////////////////////
//���²���Ϊ������ܲ���
//DESҪ��:
//key[8] text[8] mtext[8]
//singleDES�����㷨
extern void DES(U8 * key,U8 * text,U8 * mtext);
//singleDES�����㷨
extern void DES_1(U8 * key,U8 * text,U8 * mtext);

//3DESҪ��:
//key[16] text[8] mtext[8]
//TripleDES�����㷨 
extern void TripleDES  (U8 * key16,U8 * text,U8 * mtext);
//TripleDES�����㷨
extern void TripleDES_1(U8 * key16,U8 * text,U8 * mtext);

#endif
