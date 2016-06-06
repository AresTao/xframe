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
#include <arpa/inet.h>   
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#else
#include <winsock2.h> 
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

#define SendBufSize         4096000 
#define RecvBufSize          SendBufSize //
#define LOCAL_PORT_NUM  18
#define SOCKET_CODE_MAXLENGTH   20476
typedef enum SStateMachine 
{
    Wait =0, 
    Begin =1, 
    Appear =2, 
    Prepare =3, 
    Found =4 
} TStateMachine;

typedef enum SWrState 
{
    NoMsgInQueue=0, 
    MsgInQueue=1 
} TWrState;

enum TCommRole
{
    socket_invalid,
    socket_server,
    socket_client
};

typedef struct SQueueItem QueueItem;

struct SQueueItem 

{
    CHAR* pcMsg; 
    CHAR* pcMsgPointer; 
    UINT iLengthToWrite;
    QueueItem* pNext; 
};

_CLASSDEF(CSocket)
    ;

    class CSocket
{
    public:
        CSocket();
        virtual ~CSocket();
	INT openServer(CHAR* pcServerAddr, INT iServerPort, BOOL bNoBlock=TRUE); 

	virtual BOOL openClient(CHAR* pcServerAddr, INT iServerPort,
                CHAR* pcClientAddr=NULL, INT iClientPort=0,
                struct timeval *timeout=NULL);

        virtual BOOL reAccept();
        virtual BOOL reConnect();

        BOOL hasBufferedCode();
	virtual INT recvCode(CCode& code); 

	virtual INT sendCode(CCode &code); 

        BOOL isSocketStateClose() {
            return (m_socketState==CLOSE) ? TRUE : FALSE;
        }

        void setSocketStateClose();

        virtual BOOL setListenPort(CHAR* pcListenAddr, INT iListenPort);

        BOOL closeSocketConnection(); 
        BOOL closeSocket(); 

        BOOL isNoBlock(); 

        void setNoBlock(BOOL bIsNoBlock); 

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
        CHAR* c_str_remote(); 

        void setSendBufSize(INT iSendBufSize);

        void setRecvBufSize(INT iRecvBufSize);


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

        BOOL isReconnected(); 

        void serverInfo(CStr*);
        void clientInfo(CStr*);
        void connectionInfo(CStr*);
    protected:
        INT getFlag(); 
        INT peekSocket(); 
        void clearBuffer();
        int _readFromBuff(void *dst, const void *src, unsigned int length);
        int _recvSocketIntoBuff(void *dst, const void *src, unsigned int length);
        INT recvCodeByFindHead(CCode& code);
        INT recvCodeDirect(CCode& code); 


        virtual void pack(CCode &code, CCode& Packet); 
        BOOL hasUnsentCode();
        INT  preSendProcess(CCode &code);
        INT  sendUnsentCode(CCode &code);
        INT  sendMessage(CCode &code); 
        INT backupToSendBuff(CHAR* codeContent,UINT codeLength,INT codeFlag,CHAR* funcName);
        
        int  _sendBuffToSocket(void *dst, const void *src, unsigned int length);
        int _writeToBuff(void *dst, const void *src, unsigned int length);

        void _setSendBufSize(INT iSendBufSize);
        void _setRecvBufSize(INT iRecvBufSize);

        void setClientLinger(BOOL isLinger); 

        void setServerLinger(BOOL isLinger); 

        void setClientAlive(BOOL isAlive); 

        void setServerAlive(BOOL isAlive); 

        struct timeval m_conAttemptTimeout;
        virtual BOOL openClientResult(int rt);
        virtual BOOL socketInit(CHAR* pcServerAddr, INT iServerPort,
                CHAR* pcClientAddr, INT iClientPort);
        INT m_iSockfd; //socket fd.
        struct sockaddr_in server; //server internet addr.
        struct sockaddr_in client; //client internet addr.
        CHAR m_localServerAddr[30];
        CHAR m_localClientAddr[30]; 
        CHAR m_remoteServerAddr[30];
        INT m_localServerPort;
        INT m_remoteServerPort;
        INT m_localClientPort;
        TCommState m_socketState; //socket state : OPEN / CLOSE
        TCommRole m_socketRole; 
        BOOL m_bIsNeedFindHead;


#ifndef WIN32
        struct sigaction act; 
        socklen_t m_iSocklen; 
#else
        INT m_iSocklen;
#endif

        INT m_iSockWait; 
        TBuffer<CSocket>* pSendMsgBuf;
        TBuffer<CSocket> *pRecvMsgBuf;
        BOOL recvBufHasMessage;
        BOOL m_addrReuseFlag; 
        BOOL _reserved1; 

        BOOL m_bNoBlock; 

        CCode Package; 

        CCode RecvCode; 

        CCode *codeToReceive;

        UINT m_socketSendBufSize;
        UINT m_socketRecvBufSize;

        CHAR m_remoteAddrStr[128];

        UINT m_connectedCounter; 

        fd_set fd_writeset;
        BOOL b_writeNeedSelect;
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

        bool approxEqualsTo(const CUri& r) const; 

        virtual ~CUri();

        const char* host() const;
        const int   port() const;
        const char* sessionId() const;

        void  setHost(const char*);
        void  setPort(const int);
        void  setSessionId(const char*);

        const char* c_str() const; 

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

        bool connect(const char* remoteHost, int remotePort, char* localHost, int localPort, BOOL needFindHead=FALSE);
        void setFd(int fd); 
        void buildFdSet(CFdSet&);
        int process(CFdSet&);

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

        bool close(const CUri &remoteUri); 
        bool closeByLinkId(UINT linkId);
        bool closeByConnId(UINT connId);
        void closeConnectionsWithProblem();
        void closeAll(); 

        void printState(CStr& str);
        void printConnectionsState(CStr& str);

        void buildFdSet(CFdSet&);
        int procFdSet(CFdSet&);

    private:
        CTcpConnectionList mConnections;
};
#endif

