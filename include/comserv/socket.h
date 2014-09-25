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
#define SendBufSize         4096000 //2048000 //4M.假设一个消息平均为1K,则可以同时发送4000个消息。
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

typedef enum SStateMachine /*搜寻包头自动机状态 */
{
	Wait =0, /*初始状态 */
	Begin =1, /*读到‘0’*/
	Appear =2, /*读到‘01’ */
	Prepare =3, /*读到‘0111111’ */
	Found =4 /*读到‘01111110’ */
} TStateMachine;

typedef enum SWrState /*写状态 */
{
	NoMsgInQueue=0, /*消息队列中无消息 */
	MsgInQueue=1 /*消息队列中有消息 */
} TWrState;

enum TCommRole
{
	socket_invalid,
	socket_server,
	socket_client
};

typedef struct SQueueItem QueueItem;

struct SQueueItem /*消息队列中的消息项*/

{
	CHAR* pcMsg; /*消息项的内容指针 */
	CHAR* pcMsgPointer; /*移动指针*/
	UINT iLengthToWrite;/*要写的字节数*/
	QueueItem* pNext; /*消息项的下一项指针 */
};

_CLASSDEF(CSocket)
	;

	class CSocket
{
	public:
		CSocket();
		virtual ~CSocket();
		//返回值：
		//-1:失败
		// 0:监听成功，未连接
		// 1:打开连接成功
		INT openServer(CHAR* pcServerAddr, INT iServerPort, BOOL bNoBlock=TRUE); //打开服务端socket

		//打开客户端socket. 
		//Modified By Long Xiangming. 2007.03.29
		//增加iClientPort参数。如果等于0,则由操作系统选定客户端端口；如果不为0,则使用此客户端端口
		//Added by Long Xiangming. 2007.11.09 (PSA-SMPP模块用到。当本机存在多个IP时用到。上海短信中心上线时发现此问题) 
		//增加pcClientAddr参数。如果等于NULL,则由操作系统选定一个IP地址;如果不为NULL,则使用此客户端IP
		virtual BOOL openClient(CHAR* pcServerAddr, INT iServerPort,
				CHAR* pcClientAddr=NULL, INT iClientPort=0,
				struct timeval *timeout=NULL);

		//added by Long Xiangming. 2006.03.02
		virtual BOOL reAccept();//重新监听连接
		//added by Long Xiangming. 2006.05.31
		virtual BOOL reConnect();//重新连接到Server

		BOOL hasBufferedCode();
		/*
		 * return result:
		 * 1：成功读到所需数据
		 * 0：读到了0个字节. 对方socket已断开
		 * -1：socket错误
		 * -2：读到字节数少于要求数
		 * -3：不再使用（原意为搜索包头失败）
		 * -4：code错误，未分配地址空间
		 * -5：直接读取（无定界）时超长
		 * -9：socket没有建立
		 *
		 *   只有返回1表示读取正确。
		 *   如果返回0或-9，应该关闭或移除本socket，并进行重新连接或监听。
		 其他错误不用管。
		 */
		virtual INT recvCode(CCode& code); //接收消息主函数

		// Return result：
		//      INT
		//       1-----发送成功
		//       -1----写socket失败
		//       -2----向socket写入的字节数与期望值不相同(保存到sendBuff中)
		//       -3----(不用)
		//       -5----写socket sendbuff 满
		//        0 ---- 对端socket主动关闭或本端Socket已经主动关闭(m_socketState==CLOSE)。
		//       -9----对端Socket连接已经断开，或者物理链路故障
		virtual INT sendCode(CCode &code); //发送消息的主函数

		BOOL isSocketStateClose() {
			return (m_socketState==CLOSE) ? TRUE : FALSE;
		}

		void setSocketStateClose();

		//增加一个虚函数，便于使用多态。
		virtual BOOL setListenPort(CHAR* pcListenAddr, INT iListenPort);

		BOOL closeSocketConnection(); //关闭socket
		BOOL closeSocket(); //关闭socket

		BOOL isNoBlock(); //取得socket文件当前状态是否是阻塞模式

		void setNoBlock(BOOL bIsNoBlock); //设置socket是否以非阻塞模式进行通信

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
		CHAR* c_str_remote(); //得到对端的地址:端口

		void setSendBufSize(INT iSendBufSize);//设置发送缓冲区大小

		void setRecvBufSize(INT iRecvBufSize);//设置接收缓冲区大小


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

		BOOL isReconnected(); //仅仅当以前连接成功且调用了reconnect成功之后，才为TRUE

		void serverInfo(CStr*);
		void clientInfo(CStr*);
		void connectionInfo(CStr*);
	protected:
		/////////////////////////////////////////////// 
		INT getFlag(); //取得socket文件状态标志
		INT peekSocket(); //通过从socket中读取一个字节，来判断socket状态
		void clearBuffer();
		int _readFromBuff(void *dst, const void *src, unsigned int length);
		int _recvSocketIntoBuff(void *dst, const void *src, unsigned int length);
		INT recvCodeByFindHead(CCode& code); //需要处理消息定界符
		INT recvCodeDirect(CCode& code); //不需要处理消息定界符，直接接收


		virtual void pack(CCode &code, CCode& Packet); //打包
		BOOL hasUnsentCode();
		INT  preSendProcess(CCode &code);//added by Yubo Chow on Oct 10, 2008
		INT  sendUnsentCode(CCode &code);//added by ybzhou.  2008.10.10
		INT  sendMessage(CCode &code); //发送消息
		INT backupToSendBuff(CHAR* codeContent,UINT codeLength,INT codeFlag,CHAR* funcName);
		//added by lxm. 2008.10.13
		int  _sendBuffToSocket(void *dst, const void *src, unsigned int length);//added by lxm. 2008.10.14
		int _writeToBuff(void *dst, const void *src, unsigned int length);

		//执行真正的SET动作。
		void _setSendBufSize(INT iSendBufSize);//设置发送缓冲区大小
		void _setRecvBufSize(INT iRecvBufSize);//设置接收缓冲区大小

		void setClientLinger(BOOL isLinger); //设置客户端Linger

		void setServerLinger(BOOL isLinger); //设置服务端Linger

		void setClientAlive(BOOL isAlive); //设置客户端Alive

		void setServerAlive(BOOL isAlive); //设置服务端Alive//modified by lxm. 2008.12.01

		struct timeval m_conAttemptTimeout; //连接尝试超时时间。client 尝试连接 server 时的超时时间
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
		TCommRole m_socketRole; //角色:作为Server还是Client
		BOOL m_bIsNeedFindHead;

		//WrState       WriteState; //发送时判断消息队列中有无消息 

#ifndef WIN32
		struct sigaction act; //传送信号处理器信息的结构
		socklen_t m_iSocklen; //socket通信地址结构长度
#else
		INT m_iSocklen; //socket通信地址结构长度
#endif

		INT m_iSockWait; //accept() used.
		TBuffer<CSocket>* pSendMsgBuf;//modified by lxm. 2008.10.13
		TBuffer<CSocket> *pRecvMsgBuf;
		BOOL recvBufHasMessage;
		//QueueItem*    pSendQueueHead; //发送队列头指针

		//QueueItem*    pSendQueueTail; //发送队列尾指针
		BOOL m_addrReuseFlag; //地址重用标记。 //added by Long Xiangming.2006.06.30
		//缺省为1
		BOOL _reserved1; 

		BOOL m_bNoBlock; //调用Send或Recv时的阻塞模式，缺省为True,为非阻塞

		//CCode     PrePackage;    //发送之前的插入0预处理   

		CCode Package; //发送之前的打包预处理

		CCode RecvCode; //接收到的code处理

		CCode *codeToReceive;

		//added by Long Xiangming. 2007.9.19
		//socket buff 可以设置。
		UINT m_socketSendBufSize;
		UINT m_socketRecvBufSize;

		//
		CHAR m_remoteAddrStr[128];//c_str_remote返回

		UINT m_connectedCounter; //当客户端时，连接成功的次数

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

		//模糊比较。（只比较host和port)
		bool approxEqualsTo(const CUri& r) const; 

		virtual ~CUri();

		const char* host() const;
		const int   port() const;
		const char* sessionId() const;

		void  setHost(const char*);
		void  setPort(const int);
		void  setSessionId(const char*);

		const char* c_str() const; //获得其完整string

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

		//localPort为0时，由操作系统选定本地端口；否则，使用此端口作为客户端端口
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

		bool close(const CUri &remoteUri); //关闭一个connection，且delete之。
		bool closeByLinkId(UINT linkId);
		bool closeByConnId(UINT connId);
		void closeConnectionsWithProblem();
		void closeAll(); //关闭所有Connection,且移除它们

		void printState(CStr& str);
		void printConnectionsState(CStr& str);

		void buildFdSet(CFdSet&);
		int procFdSet(CFdSet&);

	private:
		CTcpConnectionList mConnections;
};
#endif

