#ifndef __UDPSOCKET_H
#define __UDPSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <netdb.h>
#include <unistd.h>
#else
#include <winsock2.h> 
#endif

#include <fcntl.h>
#include <time.h>

#include "comtypedef.h"
#include "comtypedef_vchar.h"

#ifndef _WIN32
#define SOCKLEN_T socklen_t
#else
#define SOCKLEN_T int
#endif

const INT MaxRecvBufSize = 8096;
const INT MaxSendBufSize = 8096;

_CLASSDEF(CUDPSocket)
class CUDPSocket 
{
    public:
        CUDPSocket();
        virtual ~CUDPSocket();
        INT    openServer(CHAR* localAddr, INT localPort); 
        BOOL   openClient(CHAR* remoteAddr,INT remotePort); 

        BOOL   openClient();

        INT     sendCode(CCode &code);   

        INT     sendCode(CCode &code,CHAR* remoteAddr,INT remotePort);   

	INT     recvCode(CCode& code);   

        BOOL    closeSocket();

        INT     getFd() { return m_socketFd;} 

        sockaddr_in& getClientSockAddr() {	return m_clientAddr;	}
        sockaddr_in& getRemoteSockAddr() { return m_remoteAddr;	}

        void setRemoteSockAddr(const char* addr);
        void setRemoteSockAddr(const char* host, int port);
        void clientInfo(CStr*);
        void serverInfo(CStr*);

        void    setAddrReuse(BOOL isAddrReuse) { m_addrReuseFlag = isAddrReuse;}

	void setRecvBufSize(INT iRecvBufSize);
        void setRecvBufSize();

        void setSendBufSize(INT iSendBufSize);
        void setSendBufSize();

    protected:
        struct  sockaddr_in m_localAddr;
        struct  sockaddr_in m_remoteAddr;
        struct  sockaddr_in m_clientAddr;
        INT					m_localFd;
        INT					m_remoteFd;
        INT                 m_socketFd;
        INT					m_localPort;
        INT					m_remotePort;
        SOCKLEN_T			m_addrLen;

        TCommState          m_socketState;

        BOOL				m_addrReuseFlag; 

        UINT m_socketSendBufSize;
        UINT m_socketRecvBufSize;
};


#endif

