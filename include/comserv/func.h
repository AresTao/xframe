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
//字符串处理相关函数
//*****************************************************
//字符串不分大小写比较
// src > dst：源字符串大于目的字符串 return >0
// src = dst：字符串相等 return 0
// src < dst：源字符串小于目的字符串 return <0
int strnotcasecmp( char* src , char* dst );
void strCopy(char* dest,const char* src,int len);

typedef char TUniqueHexID[25];
//得到一个系统全局唯一的字符串ID(24个字节,16进制形式）（根据当前时间计算）
//可以在一些需要全局唯一标志的地方使用
void get_unique_hexid(char* hexstr);

void intToHex(char* s, UINT n);

//*****************************************************
//时间处理相关函数
//*****************************************************

//将标准时间间隔转换为自定义的结构化时间
void timeIntervalFormatted(time_t, TTimeFormatted &);
#ifdef WIN32
void timeIntervalFormatted(DWORD & startTime, DWORD & endTime,TTimeFormatted & timeConverted);
#else
void timeIntervalFormatted(struct timeval &,struct timeval &,TTimeFormatted & );
#endif 

//从系统中获得当前时间
UINT64 getSystemTime();
UINT64 getTimeMicroSec();
UINT64 getTimeMs();
UINT64 getTimeSecs();

UINT get_currtime_sec();     //以秒计

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


//求一个结构数组的长度
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/////////////////////////////////////
////    进程/线程/信号/调度相关函数
/////////////////////////////////////
//added by Long Xiangming. 2006.06.30
void   uniSleep(int millisec); //睡眠一段时间（毫秒）。屏蔽Windows和Linux/Unix差别
void   uni_usleep(int usec);   //微秒

typedef  void (*sig_handler)(int);
//信号处理函数. 设置相应的信号捕捉函数,做退出程序前的一些清理工作.
void     sig_action(sig_handler handler);
char*    getSigName(int sig);

//added by Long Xiangming. 2008.3.10
//支持core_dump能力(在uniframe捕捉信号的同时)
int		_core_dump(int signo);

//sig_action()函数将设置捕捉TSignalTable中catchEnabled为1的信号
struct  TSignalTable
{
    int  catchEnabled; //通过此值设置系统将捕捉的信号：1-捕捉，0-不捕捉
    int  sigaction;    //捕捉后的动作：1: 捕捉且执行指定函数（退出程序) 0: 捕捉但忽略(SIG_IGN) 2：捕捉且按系统缺省处理(SIG_DFL)
    int  sigid;        //11
    int  signo;        //SIGSEGV
    char signame[64];  //SIGSEGV: Segmentation fault
    int  canCoreDump;  //此值表示此信号是否会引起core dump,是此信号的一个属性，不可改变
};  

///////////////////////////////////////
/////    系统环境/硬件/网络相关函数
/////////////////////////////////////// 
//得到本机所有配置的IP地址
//返回值: -1: 失败
//        >=0: 返回的接口(IP地址）个数
//具体的接口地址保存在ifcfg参数中。TIfCfg的定义见comtypedef.h
INT getLocalHostAddress(TIfCfg& ifcfg);

// return a string like the display of unix command `uname -a`
char* uname_a();

//读取socket。屏蔽windows和Linux差别。
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
static const uint16_t crc16tab[256]= {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

uint16_t crc16(const char *buf, int len); 
#endif

