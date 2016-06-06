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
extern void DES(U8 * key,U8 * text,U8 * mtext);
extern void DES_1(U8 * key,U8 * text,U8 * mtext);
extern void TripleDES  (U8 * key16,U8 * text,U8 * mtext);
extern void TripleDES_1(U8 * key16,U8 * text,U8 * mtext);

#endif
