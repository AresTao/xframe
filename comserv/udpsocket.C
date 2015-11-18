#include "udpsocket.h"
#include "func.h"
#include "info.h"

CUDPSocket::CUDPSocket()
{
	m_localFd    = -1;
	m_remoteFd   = -1;
	m_socketFd   = -1;
	m_localPort  = 0;
	m_remotePort = 0;

	m_socketState= CLOSE;
	m_addrReuseFlag = 0;

#ifdef WIN32
	WORD wVersionRequested = MAKEWORD(2,0);
	WSADATA wsaData;
	WSAStartup( wVersionRequested, &wsaData );
#endif
	m_addrLen = sizeof(struct sockaddr);
}

CUDPSocket::~CUDPSocket()
{
	if(m_socketState==OPEN)
		closeSocket();
}

void CUDPSocket::clientInfo(CStr* s)
{
	s->fCat("fd=%d, server=%s:%d", getFd(), inet_ntoa(m_remoteAddr.sin_addr), ntohs(m_remoteAddr.sin_port));
}
void CUDPSocket::serverInfo(CStr* s)
{
	s->fCat("fd=%d, port=%d", getFd(), m_localPort);
}

INT CUDPSocket::openServer(CHAR* localAddr, INT localPort)
{
	if(localPort <=0    ) return -1;

	m_localAddr.sin_family = AF_INET;
	m_localAddr.sin_port = htons(localPort);
	if(localAddr==NULL)
		m_localAddr.sin_addr.s_addr = INADDR_ANY;
	else
	{
		if(localAddr[0]==0 || strcmp(localAddr,"0")==0 || strcmp(localAddr,"0.0.0.0")==0)
			m_localAddr.sin_addr.s_addr = INADDR_ANY;
		else
			m_localAddr.sin_addr.s_addr=inet_addr(localAddr);
	}

	memset(&(m_localAddr.sin_zero), '\0', 8);
	m_localPort = localPort;

  	m_localFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_localFd<0) {
	    perror("open socket server");
	}

	if (setsockopt(m_localFd, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&m_addrReuseFlag, sizeof(m_addrLen)) == -1) {
		perror("setsockopt");
		return -1;
	}

	if (bind(m_localFd, (struct sockaddr *)&m_localAddr, sizeof(struct sockaddr)) != 0) {
	    perror("bind");
		return -1;
	}
	m_socketState = OPEN;
	m_socketFd = m_localFd;

	return 1;
}


void CUDPSocket::setRemoteSockAddr(const char* host, int port)
{
	m_remoteAddr.sin_port = htons(port);
	m_remoteAddr.sin_addr.s_addr = inet_addr(host);
}

void CUDPSocket::setRemoteSockAddr(const char* addr)
{
	const char* p = strstr(addr, ":");
	if(p==NULL)
		return;
	CStr host;
	host.set(addr, p-addr);
	setRemoteSockAddr(host.c_str(), atoi(p+1));
}

BOOL  CUDPSocket::openClient(CHAR* remoteAddr,INT remotePort)
{
	if(remotePort <=0    ) return FALSE;
	CHAR remoteAddress[256];
	if(remoteAddr == NULL)
	{
		strcpy(remoteAddress,"127.0.0.1");
	}
	else
	{
		if(remoteAddr[0] == 0 || strcmp(remoteAddr,"0")==0 || strcmp(remoteAddr,"0.0.0.0")==0)
			strcpy(remoteAddress,"127.0.0.1");
		else
			strcpy(remoteAddress,remoteAddr);
	}

	m_remoteAddr.sin_family = AF_INET;
	m_remoteAddr.sin_port = htons(remotePort);
	m_remoteAddr.sin_addr.s_addr=inet_addr(remoteAddress);

	memset(&(m_remoteAddr.sin_zero), '\0', 8);
	m_remotePort = remotePort;

	m_remoteFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_remoteFd<0)
	{
		perror("open socket client");
		return FALSE;
	}

	m_socketFd = m_remoteFd;
	m_socketState = OPEN;
	return TRUE;
}

BOOL  CUDPSocket::openClient()
{
	m_remoteFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_remoteFd<0)
	{
		perror("open socket client");
		return FALSE;
	}

	m_socketFd = m_remoteFd;
	m_socketState = OPEN;
	return TRUE;
}

INT CUDPSocket::sendCode(CCode &code)
{
	if(m_socketState == CLOSE) return -3;
	if(code.length <=0 || code.content == NULL) return -1;
	INT rt = sendto(m_socketFd, code.content, code.length, 0, (struct sockaddr *)&m_clientAddr, sizeof(struct sockaddr));
	if(rt>0)
		return 1;
	else return rt;

}

INT CUDPSocket::sendCode(CCode &code,CHAR* remoteAddr,INT remotePort)
{
	if(m_socketState == CLOSE) return -3;
	if(code.length <=0 || code.content == NULL) return -1;

	m_remoteAddr.sin_family = AF_INET;
	m_remoteAddr.sin_addr.s_addr=inet_addr(remoteAddr);
	m_remoteAddr.sin_port = htons(remotePort);
	memset(&(m_remoteAddr.sin_zero), '\0', 8);

	INT rt = sendto(m_socketFd, code.content, code.length, 0, (struct sockaddr *)&m_remoteAddr, sizeof(struct sockaddr));
	if(rt>0)
		return 1;
	else return rt;

}

INT CUDPSocket::recvCode(CCode& code)
{
	if(m_socketState == CLOSE) return -3;
	if(code.content == NULL) return -4;
	INT rt =recvfrom(m_socketFd, code.content, MaxRecvBufSize, MSG_DONTWAIT, (struct sockaddr *)&m_clientAddr, &m_addrLen);

	if(rt>0)
	{
		code.length = rt;
		return 1;
	}
	else return rt;

}

BOOL   CUDPSocket::closeSocket()
{
	if(m_socketState == CLOSE) return FALSE;
	BOOL rt = FALSE;
	if(m_remoteFd>0)
	{
		if(close_socket(m_remoteFd))
		{
			m_remoteFd = -1;
			m_socketState = CLOSE;  //socket״̬
			rt = TRUE;
		}
		else
			rt = FALSE;
    }

	if(m_localFd>0)
	{
		if(close_socket(m_localFd))
		{
			m_localFd = -1;
			m_socketState = CLOSE;  //socket״̬
			rt = TRUE;
		}
		else
			rt = FALSE;
	}
  	return rt;
}

void CUDPSocket::setRecvBufSize(INT iRecvBufSize)
{

	if (iRecvBufSize>0 && m_socketFd>0)
	{
		m_socketRecvBufSize = iRecvBufSize;

		if (setsockopt(m_socketFd, SOL_SOCKET, SO_RCVBUF, (CHAR*)&iRecvBufSize,
				sizeof(iRecvBufSize))<0)
		{
			UniERROR("set UDP receive buffer size error: %d.", iRecvBufSize);
			perror("setRecvBufSize:");
		}
		else
			UniINFO("Set UDP socket receive buffer size to %d(bytes) succeed.",
					iRecvBufSize);
	}
}

void CUDPSocket::setRecvBufSize()
{
	m_socketRecvBufSize = MaxRecvBufSize;
	setRecvBufSize(m_socketRecvBufSize);
}

void CUDPSocket::setSendBufSize(INT iSendBufSize)
{

	if (iSendBufSize>0 && m_socketFd>0)
	{
		m_socketSendBufSize = iSendBufSize;

		if (setsockopt(m_socketFd, SOL_SOCKET, SO_SNDBUF, (CHAR*)&iSendBufSize,
				sizeof(iSendBufSize))<0)
		{
			UniERROR("set UDP send buffer size error: %d.", iSendBufSize);
			perror("setSendBufSize:");
		}
		else
			UniINFO("Set UDP socket send buffer size to %d(bytes) succeed.",
					iSendBufSize);
	}
}

void CUDPSocket::setSendBufSize()
{
	m_socketSendBufSize = MaxSendBufSize;
	setSendBufSize(m_socketSendBufSize);
}

