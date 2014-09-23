#ifndef __FUNC_H_
#define __FUNC_H_

#ifndef WIN32
#include <values.h>
#include <netinet/in.h> //include htons,htonl,etc.
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> //include usleep
#include <sys/time.h> //include gettimeofday
//for getLocalHostAddress
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/resource.h> //include setrlimit
#else
#include <windef.h>
#include <winsock2.h> //include htons,htonl,etc.
#endif

#include <signal.h>  //include kill(), signal
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comconst.h"
#include "comtypedef.h"
#include "comtypedef_vchar.h"
//*****************************************************
//�ַ���������غ���
//*****************************************************
//�ַ������ִ�Сд�Ƚ�
// src > dst��Դ�ַ�������Ŀ���ַ��� return >0
// src = dst���ַ������ return 0
// src < dst��Դ�ַ���С��Ŀ���ַ��� return <0
int strnotcasecmp( char* src , char* dst );
void strCopy(char* dest,const char* src,int len);

typedef char TUniqueHexID[25];
//�õ�һ��ϵͳȫ��Ψһ���ַ���ID(24���ֽ�,16������ʽ�������ݵ�ǰʱ����㣩
//������һЩ��Ҫȫ��Ψһ��־�ĵط�ʹ��
void get_unique_hexid(char* hexstr);

void intToHex(char* s, UINT n);

//*****************************************************
//ʱ�䴦����غ���
//*****************************************************

//����׼ʱ����ת��Ϊ�Զ���Ľṹ��ʱ��
void timeIntervalFormatted(time_t, TTimeFormatted &);
#ifdef WIN32
void timeIntervalFormatted(DWORD & startTime, DWORD & endTime,TTimeFormatted & timeConverted);
#else
void timeIntervalFormatted(struct timeval &,struct timeval &,TTimeFormatted & );
#endif 

//��ϵͳ�л�õ�ǰʱ��
UINT64 getSystemTime();
UINT64 getTimeMicroSec();
UINT64 getTimeMs();
UINT64 getTimeSecs();

UINT get_currtime_sec();     //�����

// "YYYYMMDDHHMMSS\0"
#define MaxTimeLength 14
// "YYYY-MM-DD HH:MM:SS\0"
#define MaxTimeStringLength 19
typedef char TASCTime[MaxTimeLength+1];
typedef char TTimeString[MaxTimeStringLength+1];

//tm to "YYYYMMDDHHMMSS\0"
void tmToASCTime(tm* tmTime,TASCTime ascTime);
//tm to "YYYY-MM-DD HH:MM:SS\0"
void tmToTimeString(tm* tmTime,TTimeString timeString);
// get the current date. format:YYYYMMDD, 8 bytes ASC.
void getASCDate8(char* currentASCDate8);
// get the current date and time. format:YYYYMMDDHHMMSS, 14 bytes ASC.
void getASCDate8AndTime(char* currentASCDate8AndTime);
// get the current date and time. format:YYMMDDHHMM, 10 bytes ASC.
void getASCDate6AndTime4(char* currentASCDate6AndTime);
// get the current date and time. format:YYMMDDHHMMSS, 12 bytes ASC.
void getASCDate6AndTime6(char* currentASCDate6AndTime);


//��һ���ṹ����ĳ���
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/////////////////////////////////////
////    ����/�߳�/�ź�/������غ���
/////////////////////////////////////
//added by Long Xiangming. 2006.06.30
void   uniSleep(int millisec); //˯��һ��ʱ�䣨���룩������Windows��Linux/Unix���
void   uni_usleep(int usec);   //΢��

typedef  void (*sig_handler)(int);
//�źŴ�����. ������Ӧ���źŲ�׽����,���˳�����ǰ��һЩ������.
void     sig_action(sig_handler handler);
char*    getSigName(int sig);

//added by Long Xiangming. 2008.3.10
//֧��core_dump����(��uniframe��׽�źŵ�ͬʱ)
int		_core_dump(int signo);

//sig_action()���������ò�׽TSignalTable��catchEnabledΪ1���ź�
struct  TSignalTable
{
	int  catchEnabled; //ͨ����ֵ����ϵͳ����׽���źţ�1-��׽��0-����׽
	int  sigaction;    //��׽��Ķ�����1: ��׽��ִ��ָ���������˳�����) 0: ��׽������(SIG_IGN) 2����׽�Ұ�ϵͳȱʡ����(SIG_DFL)
	int  sigid;        //11
	int  signo;        //SIGSEGV
	char signame[64];  //SIGSEGV: Segmentation fault
	int  canCoreDump;  //��ֵ��ʾ���ź��Ƿ������core dump,�Ǵ��źŵ�һ�����ԣ����ɸı�
};  

///////////////////////////////////////
/////    ϵͳ����/Ӳ��/������غ���
/////////////////////////////////////// 
//�õ������������õ�IP��ַ
//����ֵ: -1: ʧ��
//        >=0: ���صĽӿ�(IP��ַ������
//����Ľӿڵ�ַ������ifcfg�����С�TIfCfg�Ķ����comtypedef.h
INT getLocalHostAddress(TIfCfg& ifcfg);

// return a string like the display of unix command `uname -a`
char* uname_a();

//��ȡsocket������windows��Linux���
INT    read_socket_raw(int sockfd,char* buff,int length);
BOOL   close_socket(int sockfd);


#define UNIASSERT(e) {if(!(e)) UniERROR("ASSERT FAILED. file:%s, line:%d",__FILE__,__LINE__);}
#define UNIASSERT1(e,s) {if(!(e)) UniERROR("ASSERT FAILED. file:%s, line:%d, [%s]",__FILE__,__LINE__,s);}
#define UNIASSERT2(e,i) {if(!(e)) UniERROR("ASSERT FAILED. file:%s, line:%d, [%d]",__FILE__,__LINE__,i);}

///////////////////////////////////////////////
///////       File Functions
///////////////////////////////////////////////

// if the file exist, return >0, else return 0
I32 existFile( char* fileName );
char* getFileLine(char *buf, int buf_size, FILE *fp, int* lineNo);
off_t getFileLength(const char* fileName);

#endif

