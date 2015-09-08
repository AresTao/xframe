#ifndef __UDPSOCKET_H
#define __UDPSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //include inet_addr()  
#include <netdb.h>
#include <unistd.h>
#else
#include <winsock2.h> //include timeval
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

		//ע�⣬server��client�õ�����ͬ��fd�������ͺͽ�����ʹ����ͬ��fd,������š�
		//�������Ҫ�գ�Ҳ��Ҫ������ôopenServer���ɡ����ֻ��Ҫ������openClient������ͬʱopenServer��openClient

		//�򿪷����socket
		INT    openServer(CHAR* localAddr, INT localPort); 
		//�򿪿ͻ���socket
		BOOL   openClient(CHAR* remoteAddr,INT remotePort); 
		

		//Added by Lxm. 2009-8-10
		//�򿪿ͻ���socket, ��ָ���Զ˵�ַ����ÿ�η���ʱ�� ��д��ַ���Է�����ͬ��ַ)
		BOOL   openClient();

		//������Ϣ
		//-1��дsocketʧ��
		//-3: socket�ر�
		// 1���ɹ�
		INT     sendCode(CCode &code);   

		INT     sendCode(CCode &code,CHAR* remoteAddr,INT remotePort); //ָ���Է���ַ�Ͷ˿ڷ���  

		//������Ϣ
		//-1: ��Socketʧ��
		//-3: socket�ر�
		//-4: code.content==NULL��
		//    ����Recv()֮ǰδ��code.content����ռ�
		// 1: �ɹ�
		INT     recvCode(CCode& code);   

		//�ر�socket
		BOOL    closeSocket();
		
		INT     getFd() { return m_socketFd;} //���һ�ε���open��õ�fd

		//�õ��ͻ��˵�ַ,�ڵ���recvCode֮���ٵ��ñ�������
		//Added by Lxm. 2009-08-10
		sockaddr_in& getClientSockAddr() {	return m_clientAddr;	}
		sockaddr_in& getRemoteSockAddr() { return m_remoteAddr;	}

		// format of addr: xxx.xxx.xxx.xxx:port
		void setRemoteSockAddr(const char* addr);
		void setRemoteSockAddr(const char* host, int port);
		void clientInfo(CStr*);
		void serverInfo(CStr*);
	
		void    setAddrReuse(BOOL isAddrReuse) { m_addrReuseFlag = isAddrReuse;}

		//added by Long Xiangming. 2010.1.27
		//socket buff �������á�Ŀǰ��ʹ��setRecvBufSize
		//ע�⣬������������Ҫ��ʽ�ĵ��ö�����������openServer��openClient�����У�(�ڵ���openServer����֮�����)
		//��Ϊ����������ʹ��udpSocket�ĵط���������Ҫ����buffSize
		void setRecvBufSize(INT iRecvBufSize);//���ý��ջ�������С
		void setRecvBufSize();//���ý��ջ�������С. ʹ��Ĭ��

		void setSendBufSize(INT iSendBufSize);//���÷��ͻ�������С
		void setSendBufSize();//���÷��ͻ�������С,ʹ��Ĭ��

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

		BOOL				m_addrReuseFlag; //��ַ���ñ�ǡ� //added by Long Xiangming.2006.06.30
										     //ȱʡΪ0

		//added by Long Xiangming. 2010.1.27
		//socket buff �������á�Ŀǰ��ʹ��recvBufSize
		UINT m_socketSendBufSize;
		UINT m_socketRecvBufSize;
};


#endif



