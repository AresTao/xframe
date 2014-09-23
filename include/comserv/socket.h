/******************************************************************************
 *Copyright(c) 2005-2008,by BeiJing Telestar Network Technology Company Ltd.(MT2)
 *All rights reserved

 *FileName:     csocket.h
 *System:       UniFrame 
 *SubSystem:    Common
 *Author:       Long Xiangming 
 *Date��         2003.3.24
 *Version��      1.0
 *Description��
 CSocket�ඨ�塣�������ͨ�š����̼�ͨ�Ź��ܡ���Ҫ���β�ͬ����ϵͳͨ������
 *
 * Last Modified: 
 2003.05.17, ��ɳ�ʼ�汾  
 By He Jiangling 

 2003.05.28, ���ע��
 By He Jiangling  

 2003.05.31,��ӷ���������
 ����������
 BOOL    m_bNoBlock;   
 ����������
 INT     GetFlag();
 BOOL    IsNoBlock();    
 void    SetNoBlock(BOOL bIsNoBlock);
 By He Jiangling     

 2003.06.03,���ͷ�ļ�comcommu.h
 By He Jiangling 

 2003.06.19,��ӻ���ļ��������ĺ����Ͳ��Թ�����ͷ�ļ�
 By He Jiangling

 2003.06.21,�޸�Recv()��ReadSocket()����������
 By He Jiangling

 2003.06.24,�޸�Send()��SendMessage()��SendMsgQueue()����������
 By He Jiangling

 2003.07.09. Ϊ�����CSocketServer�࣬��ԭ����˽�б�����
 ��������Ϊ��protected ���͡�
 By Xue Haiqiang.

 2003.07.11,Ϊ��CSocketSSF���CSocket��ļ̳У�
 ��Pack()��~CSocket()��Send()��Recv()��������Ϊvirtual ���͡�
 By He Jiangling.            

 2003.07.30������INT CSocket::PeekSocket();
 By Xue Haiqiang.

 2003.08.29,��ӷ�����󳤶ȶ���
 By He Jiangling

 2003.09.09,�޸�socket���������õķ���
 By He Jiangling 

 2003.01.13,���������ע���еĲ��淶
 By He Jiangling,�ύǰ�汾�ţ�1.1.1.1

 2003.03.28,ע�͵����Ͷ�����صı����ͷ������޸��źŲ�׽��ʽ
 By He Jiangling,�ύǰ�汾�ţ�1.2  

 2006.03.02, �޸�openServer����ֵ��BOOL��ΪINT), ���ӷ�������ʽ
 By Long Xiangming.

 2006.05.31, ����reConnect()����,��������Ϊ�ͻ��˵�socket�Ͽ�ʱ,����openClient

 * note:��ϸ˵��������������ϵͳ����˵����--����ͨ��1.2��    
 *******************************************************************************/
#ifndef __CSOCKET_H_
#define __CSOCKET_H_
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //include inet_addr()  
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#else
#include <winsock2.h> //include timeval
#endif

#include <fcntl.h>
#include <time.h>

#include "comtypedef.h"
#include "comconst.h"
#include "unibuffer.h"
#include "comtypedef_vchar.h"
#include "fdset.h"

//constant define.

#define QueueLength     5
#define ZERO            0x00
#define ONE             0xff

//Modified by Long Xiangming. 2007.9.3
//Modified by Long Xiangming. 2007.9.16
//#define      SendBufSize      MaxMsgLength //80K
#define SendBufSize         4096000 //2048000 //4M.����һ����Ϣƽ��Ϊ1K,�����ͬʱ����4000����Ϣ��
//#define       RecvBufSize      8192
//#define       RecvBufSize      MaxMsgLength //80k
#define RecvBufSize          SendBufSize //
#define LOCAL_PORT_NUM  18
#define SOCKET_CODE_MAXLENGTH   20476
//typedef      I32  TCommState;
/*
 #define      tc_primitive      0x00
 #define    shutdown_request   0x01
 #define    shutdown_indication   0x02
 #define      routine_check      0x03
 #define      sms_management      0x04
 #define      ctrl_management     0x05
 */

typedef enum SStateMachine /*��Ѱ��ͷ�Զ���״̬ */
{
   Wait =0, /*��ʼ״̬ */
   Begin =1, /*������0��*/
   Appear =2, /*������01�� */
   Prepare =3, /*������0111111�� */
   Found =4 /*������01111110�� */
} TStateMachine;

typedef enum SWrState /*д״̬ */
{
   NoMsgInQueue=0, /*��Ϣ����������Ϣ */
   MsgInQueue=1 /*��Ϣ����������Ϣ */
} TWrState;

enum TCommRole
{
   socket_invalid,
   socket_server,
   socket_client
};

typedef struct SQueueItem QueueItem;

struct SQueueItem /*��Ϣ�����е���Ϣ��*/

{
   CHAR* pcMsg; /*��Ϣ�������ָ�� */
   CHAR* pcMsgPointer; /*�ƶ�ָ��*/
   UINT iLengthToWrite;/*Ҫд���ֽ���*/
   QueueItem* pNext; /*��Ϣ�����һ��ָ�� */
};

_CLASSDEF(CSocket)
;

class CSocket
{
public:
   CSocket();
   virtual ~CSocket();
   //����ֵ��
   //-1:ʧ��
   // 0:�����ɹ���δ����
   // 1:�����ӳɹ�
   INT openServer(CHAR* pcServerAddr, INT iServerPort, BOOL bNoBlock=TRUE); //�򿪷����socket

   //�򿪿ͻ���socket. 
   //Modified By Long Xiangming. 2007.03.29
   //����iClientPort�������������0,���ɲ���ϵͳѡ���ͻ��˶˿ڣ������Ϊ0,��ʹ�ô˿ͻ��˶˿�
   //Added by Long Xiangming. 2007.11.09 (PSA-SMPPģ���õ������������ڶ��IPʱ�õ����Ϻ�������������ʱ���ִ�����) 
   //����pcClientAddr�������������NULL,���ɲ���ϵͳѡ��һ��IP��ַ;�����ΪNULL,��ʹ�ô˿ͻ���IP
   virtual BOOL openClient(CHAR* pcServerAddr, INT iServerPort,
         CHAR* pcClientAddr=NULL, INT iClientPort=0,
         struct timeval *timeout=NULL);

   //added by Long Xiangming. 2006.03.02
   virtual BOOL reAccept();//���¼�������
   //added by Long Xiangming. 2006.05.31
   virtual BOOL reConnect();//�������ӵ�Server

   BOOL hasBufferedCode();
/*
 * return result:
 * 1���ɹ�������������
 * 0��������0���ֽ�. �Է�socket�ѶϿ�
 * -1��socket����
 * -2�������ֽ�������Ҫ����
 * -3������ʹ�ã�ԭ��Ϊ������ͷʧ�ܣ�
 * -4��code����δ�����ַ�ռ�
 * -5��ֱ�Ӷ�ȡ���޶��磩ʱ����
 * -9��socketû�н���
 *
 *   ֻ�з���1��ʾ��ȡ��ȷ��
 *   �������0��-9��Ӧ�ùرջ��Ƴ���socket���������������ӻ������
      ���������ùܡ�
 */
   virtual INT recvCode(CCode& code); //������Ϣ������

// Return result��
//      INT
//       1-----���ͳɹ�
//       -1----дsocketʧ��
//       -2----��socketд����ֽ���������ֵ����ͬ(���浽sendBuff��)
//       -3----(����)
//       -5----дsocket sendbuff ��
//        0 ---- �Զ�socket�����رջ򱾶�Socket�Ѿ������ر�(m_socketState==CLOSE)��
//       -9----�Զ�Socket�����Ѿ��Ͽ�������������·����
   virtual INT sendCode(CCode &code); //������Ϣ��������

   BOOL isSocketStateClose() {
      return (m_socketState==CLOSE) ? TRUE : FALSE;
   }

   void setSocketStateClose();

   //����һ���麯��������ʹ�ö�̬��
   virtual BOOL setListenPort(CHAR* pcListenAddr, INT iListenPort);

   BOOL closeSocketConnection(); //�ر�socket
   BOOL closeSocket(); //�ر�socket

   BOOL isNoBlock(); //ȡ��socket�ļ���ǰ״̬�Ƿ�������ģʽ

   void setNoBlock(BOOL bIsNoBlock); //����socket�Ƿ��Է�����ģʽ����ͨ��

   void setNeedFindHead(BOOL isNeedFindHead) {
      m_bIsNeedFindHead=isNeedFindHead;
   }

   INT getFd() { return m_iSockfd; }
   INT getWaitFd() { return m_iSockWait;}
   virtual void setFd(int fd) { m_iSockfd = fd; if(fd>0) m_socketState=OPEN; } 
   void setWaitFd(int waitFd) { m_iSockWait=waitFd; }
   
   sockaddr_in& getClientSockAddr() {
      return client;
   }
   CHAR* c_str_remote(); //�õ��Զ˵ĵ�ַ:�˿�

   void setSendBufSize(INT iSendBufSize);//���÷��ͻ�������С

   void setRecvBufSize(INT iRecvBufSize);//���ý��ջ�������С


   TCommRole getCommRole() {
      return m_socketRole;
   }
   void setCommRole(TCommRole commRole) {
      m_socketRole = commRole;
   }

   void setAddrReuse(BOOL isAddrReuse) {
      m_addrReuseFlag = isAddrReuse;
   }

   void setShortConnectionMode()
   {
      m_shortConnectionMode = 1;
   }

   BOOL isReconnected(); //��������ǰ���ӳɹ��ҵ�����reconnect�ɹ�֮�󣬲�ΪTRUE

   void serverInfo(CStr*);
   void clientInfo(CStr*);
   void connectionInfo(CStr*);
protected:
   /////////////////////////////////////////////// 
   INT getFlag(); //ȡ��socket�ļ�״̬��־
   INT peekSocket(); //ͨ����socket�ж�ȡһ���ֽڣ����ж�socket״̬
   void clearBuffer();
   int _readFromBuff(void *dst, const void *src, unsigned int length);
   int _recvSocketIntoBuff(void *dst, const void *src, unsigned int length);
   INT recvCodeByFindHead(CCode& code); //��Ҫ������Ϣ�����
   INT recvCodeDirect(CCode& code); //����Ҫ������Ϣ�������ֱ�ӽ���


   virtual void pack(CCode &code, CCode& Packet); //���
   BOOL hasUnsentCode();
   INT  preSendProcess(CCode &code);//added by Yubo Chow on Oct 10, 2008
   INT  sendUnsentCode(CCode &code);//added by ybzhou.  2008.10.10
   INT  sendMessage(CCode &code); //������Ϣ
   INT backupToSendBuff(CHAR* codeContent,UINT codeLength,INT codeFlag,CHAR* funcName);
                                                            //added by lxm. 2008.10.13
   int  _sendBuffToSocket(void *dst, const void *src, unsigned int length);//added by lxm. 2008.10.14
   int _writeToBuff(void *dst, const void *src, unsigned int length);

   //ִ��������SET������
   void _setSendBufSize(INT iSendBufSize);//���÷��ͻ�������С
   void _setRecvBufSize(INT iRecvBufSize);//���ý��ջ�������С

   void setClientLinger(BOOL isLinger); //���ÿͻ���Linger

   void setServerLinger(BOOL isLinger); //���÷����Linger

   void setClientAlive(BOOL isAlive); //���ÿͻ���Alive

   void setServerAlive(BOOL isAlive); //���÷����Alive//modified by lxm. 2008.12.01

   struct timeval m_conAttemptTimeout; //���ӳ��Գ�ʱʱ�䡣client �������� server ʱ�ĳ�ʱʱ��
   virtual BOOL openClientResult(int rt);
   virtual BOOL socketInit(CHAR* pcServerAddr, INT iServerPort,
         CHAR* pcClientAddr, INT iClientPort);
   INT m_iSockfd; //socket fd.
   struct sockaddr_in server; //server internet addr.
   struct sockaddr_in client; //client internet addr.
   CHAR m_localServerAddr[30];
   CHAR m_localClientAddr[30]; //added by Long Xiangming. 2007.11.09
   CHAR m_remoteServerAddr[30];
   INT m_localServerPort;
   INT m_remoteServerPort;
   INT m_localClientPort;
   TCommState m_socketState; //socket state : OPEN / CLOSE
   TCommRole m_socketRole; //��ɫ:��ΪServer����Client
   BOOL m_bIsNeedFindHead;

   //WrState       WriteState; //����ʱ�ж���Ϣ������������Ϣ 

#ifndef WIN32
   struct sigaction act; //�����źŴ�������Ϣ�Ľṹ
   socklen_t m_iSocklen; //socketͨ�ŵ�ַ�ṹ����
#else
   INT m_iSocklen; //socketͨ�ŵ�ַ�ṹ����
#endif

   INT m_iSockWait; //accept() used.
   TBuffer<CSocket>* pSendMsgBuf;//modified by lxm. 2008.10.13
   TBuffer<CSocket> *pRecvMsgBuf;
   BOOL recvBufHasMessage;
   //QueueItem*    pSendQueueHead; //���Ͷ���ͷָ��

   //QueueItem*    pSendQueueTail; //���Ͷ���βָ��
   BOOL m_addrReuseFlag; //��ַ���ñ�ǡ� //added by Long Xiangming.2006.06.30
   //ȱʡΪ1
   BOOL _reserved1; 

   BOOL m_bNoBlock; //����Send��Recvʱ������ģʽ��ȱʡΪTrue,Ϊ������

   //CCode     PrePackage;    //����֮ǰ�Ĳ���0Ԥ����   

   CCode Package; //����֮ǰ�Ĵ��Ԥ����

   CCode RecvCode; //���յ���code����
   
   CCode *codeToReceive;

   //added by Long Xiangming. 2007.9.19
   //socket buff �������á�
   UINT m_socketSendBufSize;
   UINT m_socketRecvBufSize;

   //
   CHAR m_remoteAddrStr[128];//c_str_remote����

   UINT m_connectedCounter; //���ͻ���ʱ�����ӳɹ��Ĵ���

   fd_set fd_writeset;
   BOOL b_writeNeedSelect;//added by Yubo Chow on Oct 10, 2008
   timeval m_resendSelectWaitTime;
   int m_shortConnectionMode;
};

class CUri
{
public:
   CUri();
   CUri(const char* host,const int port,const char* sessionId);
   CUri(sockaddr_in addr);

   CUri(const CUri&);
   CUri& operator=(const CUri&);
   bool operator==(const CUri& r) const;
   bool operator<(const CUri& r) const;

   //ģ���Ƚϡ���ֻ�Ƚ�host��port)
   bool approxEqualsTo(const CUri& r) const; 

   virtual ~CUri();
   
   const char* host() const;
   const int   port() const;
   const char* sessionId() const;

   void  setHost(const char*);
   void  setPort(const int);
   void  setSessionId(const char*);

   const char* c_str() const; //���������string

private:
   char mHost[128];
   int  mPort;
   char mSessionId[80];
};

class CTcpConnection : public CSocket
{
public:
   CTcpConnection();
   ~CTcpConnection();

   //localPortΪ0ʱ���ɲ���ϵͳѡ�����ض˿ڣ�����ʹ�ô˶˿���Ϊ�ͻ��˶˿�
   bool connect(const char* remoteHost, int remotePort, char* localHost, int localPort, BOOL needFindHead=FALSE);
   void setFd(int fd); 
   void buildFdSet(CFdSet&);
   int process(CFdSet&);

   // derived class should implement these two function
   virtual void processSend(); 
   virtual int processRecv(); 

   virtual bool close() { return closeAnyway(); }

   int  sockfd();

   int  receive(CCode&);
   int  send(CCode&);

   virtual int needSend() { return 0; }

   bool closeAnyway();
   virtual void printState(CStr& str);

   CUri& remoteUri();

   int& linkId() { return mLinkId; }
   int connId() { return getFd(); }
    
protected:
   CUri mRemoteUri;
   int mLinkId;
   int recvCounter;
   int sendCounter;
};

class CTcpListener : public CSocket
{
public:
   CTcpListener(BOOL needFindHead=FALSE);
   ~CTcpListener();

   bool open(char* localAddr, int localPort);
   void buildFdSet(CFdSet& fdSet); 
   int accept(CFdSet& fdSet); // 1: accepted a connection, 0: no connection
   void dupConnection(CTcpConnection* tcpConnection);
   bool close();

   void printState(CStr& str);   
};

typedef std::list<CTcpConnection*> CTcpConnectionList;

class CTcpConnectionManager
{
public:
   CTcpConnectionManager();
   virtual ~CTcpConnectionManager();

   void addConnection(CTcpConnection*);

   CTcpConnection* find(const CUri &remoteUri);
   CTcpConnection* findByLinkId(UINT linkId);
   CTcpConnection* findByConnId(UINT connid);

   bool close(const CUri &remoteUri); //�ر�һ��connection����delete֮��
   bool closeByLinkId(UINT linkId);
   bool closeByConnId(UINT connId);
   void closeConnectionsWithProblem();
   void closeAll(); //�ر�����Connection,���Ƴ�����

   void printState(CStr& str);
   void printConnectionsState(CStr& str);

   void buildFdSet(CFdSet&);
   int procFdSet(CFdSet&);

private:
   CTcpConnectionList mConnections;
};
#endif

