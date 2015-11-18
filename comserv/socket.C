#include <errno.h>
#ifdef WIN32
#include <windef.h>
#endif

#include "socket.h"
#include "func.h"
#include "info.h"

#define SO_SNDLOWAT_VALUE 40000
#define MAX_SOCKET_CACHE_SEND_SIZE  200000000 //200M 

#ifdef WIN32
int errno = 0;
#endif

UINT blockCounter         =0;
UINT fullCounter            =0;
UINT notenoughCounter      =0;
UINT senderrorCounter      =0;
UINT backupCounter         =0;
UINT sendAvailableCounter   =0;
UINT resendSucceedCounter   =0;
UINT sendUnavailableCounter   =0;

#define OUTPUT_BLOCK_COUNTER              5000
#define OUTPUT_BACK_COUNTER                100000
#define OUTPUT_FULL_COUNTER              100000
#define OUTPUT_RESENDSUCCEED_COUNTER    10000
#define OUTPUT_SENDAVAILABLE_COUNTER     10000
#define OUTPUT_SENDUNAVAILABLE_COUNTER    100000
#define OUTPUT_NOTENOUGH_COUNTER           5000
#define OUTPUT_SENDERROR_COUNTER            100

//for recv
UINT uncompleteNoDropCounter = 0;
UINT uncompleteDropCounter     =0;
UINT readDirectFullCounter            =0;

#define OUTPUT_UNCOMPLETE_NODROP_COUNTER      50000
#define OUTPUT_UNCOMPLETE_DROP_COUNTER          100
#define OUTPUT_READDIRECT_FULL_COUNTER            10000


extern "C"
{
}
void socketDown(INT i)
{
   UniINFO("Catch SIGPIPE signal.");
   return;
}

CSocket::CSocket()
{
#ifndef WIN32
   act.sa_handler = socketDown; //设置新的信号处理器
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0; //没有特殊选项
   if (sigaction(SIGPIPE,&act,NULL) == -1)
      UniERROR("Could not install  SIGPIPE signal handler.");
#endif

#ifdef WIN32
   WORD wVersionRequested= MAKEWORD(2,0);
   WSADATA wsaData;
   WSAStartup(wVersionRequested, &wsaData);
#endif

   m_iSockWait = -99; //服务端监听句柄
   //--对于SET_FD(),可能会出“段错误，核心陷阱”
   m_iSockfd = -99; //服务端接收句柄
   m_iSocklen=sizeof(server); //socket通信地址结构长度
   m_socketState = CLOSE; //socket状态

   pSendMsgBuf = NULL;
   UINT size = MaxMsgLength;
   pRecvMsgBuf = new TBuffer<CSocket>(size, this, &CSocket::_readFromBuff, &CSocket::_recvSocketIntoBuff);

   recvBufHasMessage = FALSE;

   m_bNoBlock=TRUE; //socket文件阻塞标记   

   m_addrReuseFlag = 1; 

   //--Modified 2005-2008.07.10, by He Jiangling
   //PrePackage.length=0;
   //PrePackage.content=new CHAR[MAX_MSG_BUF_SIZE];
   Package.content=new CHAR[MAX_MSG_BUF_SIZE+8+COM_MSG_LENGTH+1];
   memset(Package.content, 0, MAX_MSG_BUF_SIZE+8+COM_MSG_LENGTH+1);
   Package.content[0]=0x00; //定界的第1字节为‘0’
   for (UINT i=1; i<7; i++)
      Package.content[i]=0xff; //定界的第2-7字节为‘1’
   Package.content[7] = 0x00; //定界的第8字节为‘0’
   m_bIsNeedFindHead = TRUE; // default needing to find the msg head.
   m_socketRole = socket_invalid;

   m_localServerAddr[0]=0;
   m_localClientAddr[0]=0;
   m_remoteServerAddr[0]=0;
   m_localServerPort = 0;
   m_remoteServerPort = 0;
   m_localClientPort =0;
   m_conAttemptTimeout.tv_sec = 2;//默认连接尝试的超时时间为2s 
   m_conAttemptTimeout.tv_usec = 0; 
   m_socketSendBufSize = SendBufSize;
   m_socketRecvBufSize = RecvBufSize;

   m_remoteAddrStr[0] = 0;
   m_connectedCounter = 0;

   FD_ZERO(&fd_writeset);
   b_writeNeedSelect=FALSE;

   m_resendSelectWaitTime.tv_sec = 0;
   m_resendSelectWaitTime.tv_usec = 0;

   m_shortConnectionMode = 0;
}
CSocket::~CSocket()
{
   closeSocket();
   //--Modified 2006.08.04, Long Xiangming.
   delete []Package.content; // added by Longxiangming. 2007.9.12
   if (pSendMsgBuf)
      delete pSendMsgBuf; //added by lxm. 2008.10.15
   if (pRecvMsgBuf)
   {
      delete pRecvMsgBuf;
   }
}

void CSocket::clientInfo(CStr* s)
{
   if(m_socketRole == socket_server)
      *s << "ERROR: socket is server";
   else
      s->fCat("Client: fd=%d, server=%s"
         , getFd()
         , c_str_remote()
      );
}
void CSocket::serverInfo(CStr* s)
{
   if(m_socketRole == socket_client)
      *s << "ERROR: socket is client";
   else
      s->fCat("Server: fd=%d, port=%d"
         , getWaitFd()
         , m_localServerPort
      );
}
void CSocket::connectionInfo(CStr* s)
{
   if(m_socketRole == socket_client)
      *s << "ERROR: socket is client";
   else
      s->fCat("Connection: fd=%d, client=%s"
         , getFd()
         , c_str_remote()
      );
}
INT CSocket::openServer(CHAR* pcServerAddr, INT iServerPort, BOOL bNoBlock)
{
   m_bNoBlock=bNoBlock;
   m_socketRole = socket_server;
   m_remoteAddrStr[0] = 0; //added by lxm. 2008.10.30.  for c_str().

   //保留本地地址和端口
   if (pcServerAddr)
      strcpy(m_localServerAddr, pcServerAddr);

   if (iServerPort <= 0)
   {
      UniERROR("Invalid localServerPort: %d", iServerPort);
      return -1;
   }
   m_localServerPort=iServerPort;

   if (m_iSockWait < 0) //add by chl 2003-3-31 to avoid relistening when reopen
   {

      m_iSockWait=socket(AF_INET,SOCK_STREAM,0); //建立socket句柄作为监听句柄
      if (m_iSockWait < 0)
      {
         //建立socket句柄失败
         UniINFO("creating server socket error. addr: %s, port: %d",
               pcServerAddr, iServerPort);
         m_iSockWait = -99;
         return -1;
      }

      if (bNoBlock)
      {
#ifndef WIN32
         if (fcntl(m_iSockWait, F_SETFL, FNDELAY)<0)
         {
            UniERROR("Set server socket non_block flag error.");
            m_iSockWait = -99;
            return -1;
         }
#else
         u_long nbFlag = 1;
         if (ioctlsocket(m_iSockWait, FIONBIO, &nbFlag)!=0)
         {
            UniERROR("Set server socket non_block flag error.");
            m_iSockWait = -99;
            return -1;
         }
#endif
      }
      server.sin_family=AF_INET; //socket使用的协议
      if (pcServerAddr==NULL)
         server.sin_addr.s_addr=htonl(INADDR_ANY);
      else
      {
         if (pcServerAddr[0]==0 || strcmp(pcServerAddr, "0")==0 || strcmp(
               pcServerAddr, "0.0.0.0")==0)
            server.sin_addr.s_addr=htonl(INADDR_ANY);
         else
            server.sin_addr.s_addr=inet_addr(pcServerAddr);
      }
      server.sin_port=htons(iServerPort);

      {
         setServerLinger(1); //设置服务端Linger
         setServerAlive(1);//moved here. by lxm. 2008.12.02
      }
      setsockopt(m_iSockWait, SOL_SOCKET, SO_REUSEADDR,
		      (CHAR*)(&m_addrReuseFlag), sizeof(m_addrReuseFlag));

      if (bind(m_iSockWait, reinterpret_cast<struct sockaddr*>(&server),
			      sizeof server)<0)
      {
         UniERROR("binding server socket error. addr: %s, port: %d",
               pcServerAddr, iServerPort);
         close_socket(m_iSockWait);
         m_iSockWait = -99;
         return -1;
      }
      if (listen(m_iSockWait, QueueLength)<0)
         perror("Listen socket error:"); //监听端口

   } //  end of (if m_iSockwait<0)


   UniINFO("Openning server(%s:%d) success. wait fd is %d",
         pcServerAddr, iServerPort,m_iSockWait);
         
#ifndef HPOS
   m_iSockfd = accept(m_iSockWait,
         reinterpret_cast<struct sockaddr*>(&client), &m_iSocklen);
#else
   m_iSockfd = accept(m_iSockWait,reinterpret_cast<struct sockaddr*>(&client),(int*)&m_iSocklen);
#endif
   INT ret = 0;
   if (m_iSockfd > 0)
   {
      m_socketState = OPEN; //socket状态置为OPEN
      setNoBlock(m_bNoBlock);
      _setSendBufSize(m_socketSendBufSize); //设置服务端发送缓冲区大小
      _setRecvBufSize(m_socketRecvBufSize); //设置服务端接收缓冲区大小
      INT val= SO_SNDLOWAT_VALUE;
      setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDLOWAT, (CHAR*)&val, sizeof(val));
      FD_SET(m_iSockfd, &fd_writeset);

      b_writeNeedSelect = FALSE;
      ret = 1;
   }
   else
   {
      ret = 0;
   }

   return ret;
}

BOOL CSocket::reAccept()
{
   if (m_iSockWait<=0)
      return FALSE;

   m_remoteAddrStr[0] = 0;
   m_iSockfd = accept(m_iSockWait,
         reinterpret_cast<struct sockaddr*>(&client), &m_iSocklen);
   if (m_iSockfd > 0)
   {
      m_socketState = OPEN; //socket状态置为OPEN
      setNoBlock(m_bNoBlock);
      _setSendBufSize(m_socketSendBufSize); //设置服务端发送缓冲区大小
      _setRecvBufSize(m_socketRecvBufSize); //设置服务端接收缓冲区大小
      INT val= SO_SNDLOWAT_VALUE;
      setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDLOWAT, (CHAR*)&val, sizeof(val));
      FD_SET(m_iSockfd, &fd_writeset);

      b_writeNeedSelect = FALSE;
      if(!m_shortConnectionMode)
         UniINFO("socket server accept new connection. remote_addr: %s, sock_fd: %d.", c_str_remote(), m_iSockfd);
      return TRUE;
   }
   else
   {
      m_socketState = CLOSE;
      return FALSE;
   }

}

void CSocket::setSocketStateClose()
{
   m_socketState = CLOSE;
   m_iSockfd = -99;

}
BOOL CSocket::closeSocketConnection()
{
   clearBuffer();
   BOOL rt = TRUE;
   if (m_iSockfd>0)
   {
      if (close_socket(m_iSockfd))
      {
         m_iSockfd = -99; //服务端接收句柄
         m_socketState = CLOSE; //socket状态
         rt= TRUE;
      }
      else //关闭socket连接失败
      {
         UniERROR("socket client close failure. sock_fd: %d.", m_iSockfd);
         m_iSockfd = -99; //服务端接收句柄
         m_socketState = CLOSE; //socket状态
         rt = FALSE;
      }
   }
   return rt;
}

BOOL CSocket::closeSocket()
{
   BOOL rt = closeSocketConnection();
   if (m_iSockWait>0)
   {

      if (close_socket(m_iSockWait))
      {
         UniINFO("CSocket::closeSocket() for server listen succeed. wait_sock_fd: %d",m_iSockWait);
         m_iSockWait = -99; //服务端接收句柄
         m_socketState = CLOSE; //socket状态
         rt = TRUE;
      }
      else //关闭socket连接失败
      {
         UniERROR("socket server close failure. sock_fd: %d.", m_iSockWait);
         rt = FALSE;
      }
   }
   b_writeNeedSelect = TRUE;

   return rt;

}
void CSocket::setNoBlock(BOOL bIsNoBlock)
{
   INT iFlag; //文件状态标记

   if (m_socketState!=OPEN)
   {
      //socket当前状态未打开
      m_bNoBlock=bIsNoBlock;
      return;
   }
#ifndef WIN32
   iFlag=getFlag(); //取得当前socket文件的状态标记
   if (bIsNoBlock==TRUE)
      iFlag |= O_NONBLOCK; //将标记置为非阻塞
   else
      iFlag &=~O_NONBLOCK; //将标记置为阻塞

   if (fcntl(m_iSockfd, F_SETFL, iFlag)==-1)
   {
      //设置文件状态失败
      UniERROR("Set socket block flag error!");
      return;
   }
#else
   u_long nbFlag;
   if (bIsNoBlock)
   nbFlag = 1;
   else
   nbFlag = 0;

   if (ioctlsocket(m_iSockfd, FIONBIO, &nbFlag)!=0)
   {
      UniERROR("Set server socket block flag error.");
      return;
   }

#endif

   m_bNoBlock=bIsNoBlock; //当前socket文件状态为bIsNoBlock

   return;
}
INT CSocket::getFlag()
{
   INT iFlag = -1; //文件状态标记
   //取得当前socket文件的状态标记
#ifndef WIN32
   iFlag=fcntl(m_iSockfd, F_GETFL, 0);

   if (iFlag==-1)
   {
      //取得当前socket文件的状态标记失败
      UniINFO("get flag error");
      return -1;
   }
#endif

   return iFlag;
}
BOOL CSocket::isNoBlock()
{
#ifndef WIN32  
   INT iFlag; //文件状态标记
   iFlag=getFlag(); //取得当前socket文件的状态标记
   if ((iFlag & O_NONBLOCK)!=0)
   {
      return TRUE; //当前socket文件的状态为非阻塞
   }
   else
   {
      return FALSE; //当前socket文件的状态为阻塞
   }
#else

   return m_bNoBlock;

#endif
}
INT CSocket::peekSocket()
{
   INT iNRead;
   CHAR buf[2];

   if (m_iSockfd < 0)
   {
      UniINFO("CSocket::PeekSocket() -> invalid socketid！");
   }
#ifdef WIN32
   iNRead=recv(m_iSockfd, buf, 1, MSG_PARTIAL);
#else
   iNRead=read(m_iSockfd, buf, 1);
#endif

   if (iNRead ==1) //实际读出的字节数等于要读的字节数
   {
      return (1);
   }

   if (iNRead == 0) //实际读出的字节个数为0
   {
      UniINFO("CSocket::PeekSocket() -> No Message Come From The Other Side.");
      return (-9);
   }
   if (iNRead < 0) //读socket失败
   {
      // 如果EBADF == errno，表示描述符无效
      if (EBADF == errno)
      {
         return (-9);
      }
#ifndef WIN32
      if (errno == EWOULDBLOCK) //add by Yubo Chow for non-block mode
#else
      if (errno == WSAEWOULDBLOCK)
#endif      
      {
         return 1;
      }
      return (-1);
   }
}
BOOL CSocket::setListenPort(CHAR* pcListenAddr, INT iListenPort)
{
   UniINFO("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
   UniINFO("This Method should not be used, It is added only for multicast!");
   UniINFO("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
   return FALSE;
}
void CSocket::setSendBufSize(INT iSendBufSize)
{
   if (iSendBufSize>0)
   {
      m_socketSendBufSize = iSendBufSize;
      if (m_iSockfd>0)
         _setSendBufSize(iSendBufSize);

   }

}

void CSocket::_setSendBufSize(INT iSendBufSize)
{
   INT iBufSize, iBufLength;
   //初始化Socket发送缓冲大小，见comconst.h
   iBufSize=iSendBufSize;
   iBufLength=sizeof iSendBufSize;
   if (setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDBUF, (CHAR*)&iBufSize,
         iBufLength)<0)
   {
      UniERROR("set send buffer size error: %d.", iSendBufSize);
      perror("setSendBufSize:");
   }
}

void CSocket::setRecvBufSize(INT iRecvBufSize)
{

   if (iRecvBufSize>0)
   {
      m_socketRecvBufSize = iRecvBufSize;
      if (m_iSockfd>0)
         _setRecvBufSize(iRecvBufSize);

   }
}

void CSocket::_setRecvBufSize(INT iRecvBufSize)
{
   INT iBufSize, iBufLength;
   iBufSize=iRecvBufSize;
   iBufLength=sizeof iRecvBufSize;
   if (setsockopt(m_iSockfd, SOL_SOCKET, SO_RCVBUF, (CHAR*)&iBufSize,
         iBufLength)<0)
   {
      UniERROR("set receive buffer size error: %d.", iRecvBufSize);
      perror("setRecvBufSize:");
   }
}
void CSocket::setServerLinger(BOOL isLinger)
{
   struct linger SendOver;
   SendOver.l_onoff=isLinger;//1; //是否延时等待
   SendOver.l_linger = 0; // (容许逗留的时间为0秒). added by lxm. 2008.12.02


   INT iSendOverLength=sizeof SendOver;
   memset((CHAR*)&SendOver, 0, iSendOverLength);
   if (setsockopt(m_iSockWait, SOL_SOCKET, SO_LINGER, (CHAR*)&SendOver,
         iSendOverLength)<0)
   {
      UniERROR("set linger error");
   }
}
void CSocket::setClientLinger(BOOL isLinger)
{
   struct linger SendOver;
   SendOver.l_onoff=isLinger; //是否延时等待
   SendOver.l_linger = 0; // (容许逗留的时间为0秒). added by lxm. 2008.12.02


   INT iSendOverLength=sizeof SendOver;
   memset((CHAR*)&SendOver, 0, iSendOverLength);
   if (setsockopt(m_iSockfd, SOL_SOCKET, SO_LINGER, (CHAR*)&SendOver,
         iSendOverLength)<0)
   {
      UniERROR("set linger error");
   }
}
void CSocket::setServerAlive(BOOL isAlive)
{
   INT ikeepAliveFlag=isAlive;
   INT ikeepAliveSize=sizeof ikeepAliveFlag;

   if (setsockopt(m_iSockWait, SOL_SOCKET, SO_KEEPALIVE,
         (CHAR*)&ikeepAliveFlag, ikeepAliveSize)<0)
   {
      UniERROR("setServerAlive error: %d",isAlive);
   }
}
void CSocket::setClientAlive(BOOL isAlive)
{
   INT ikeepAliveFlag=isAlive;
   INT ikeepAliveSize=sizeof ikeepAliveFlag;

   if (setsockopt(m_iSockfd, SOL_SOCKET, SO_KEEPALIVE, (CHAR*)&ikeepAliveFlag,
         ikeepAliveSize)<0)
   {
      UniERROR("setClientAlive error: %d",isAlive);
   }
}

CHAR* CSocket::c_str_remote() //得到对端的地址:端口
{
   if(m_remoteAddrStr[0]==0) //added by lxm.2008.10.29
   {
      if (m_socketRole==socket_client)
      {
            sprintf(m_remoteAddrStr, "%s:%d", m_remoteServerAddr,
               m_remoteServerPort);
      }
      else if (m_socketRole == socket_server)
      {
         if (m_socketState==OPEN)
            sprintf(m_remoteAddrStr, "%s:%d", inet_ntoa(client.sin_addr),
                  ntohs(client.sin_port));
      }
   }
   return m_remoteAddrStr;

}

BOOL CSocket::isReconnected()
{
   if (m_connectedCounter>=2)
      return TRUE;
   else
      return FALSE;

}

BOOL CSocket::reConnect()
{
   if (m_remoteServerPort <=0)
      return FALSE;
   return openClient(m_remoteServerAddr, m_remoteServerPort,
         m_localClientAddr, m_localClientPort, &m_conAttemptTimeout) ;
}

BOOL CSocket::hasUnsentCode()
{
   if (pSendMsgBuf && pSendMsgBuf->filledLength()>0)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

void CSocket::clearBuffer()
{
   if (pRecvMsgBuf)
   {
      pRecvMsgBuf->reset();
   }
   if (pSendMsgBuf)
      pSendMsgBuf->reset();

}
INT CSocket::sendCode(CCode &code)
{
   INT SendResult=-1;

   if (SOCKET_CODE_MAXLENGTH < code.length)
   {
      UniINFO("CSocket::Send() -> Msg Too Long! Must LESS Than: %d ", 
      SOCKET_CODE_MAXLENGTH);
      return -6;
   }

   if (m_bIsNeedFindHead)
   {
      pack(code, Package); //加上包头
      SendResult=sendMessage(Package); //发送包
   }
   else
      SendResult=sendMessage(code);

   if (SendResult==1)
   {
   }
   else
   {
      if (SendResult == -2)
      {
         b_writeNeedSelect = TRUE;
      }
      else if (SendResult!=-5)
      {
         senderrorCounter++;
         if (senderrorCounter==1 || senderrorCounter
               %OUTPUT_SENDERROR_COUNTER==0)
         {
            UniERROR(
                  "CSocket::sendCode() : Write Socket Error: result: %d, errno = %s. socket_fd: %d.  counter: %d\n",
                  SendResult, strerror(errno), m_iSockfd,
                  senderrorCounter);
         }
      }
   }
   return (SendResult);
}
void CSocket::pack(CCode & code, CCode& Packet)
{
   Packet.length = code.length+8+COM_MSG_LENGTH;
   CHAR* pcPacket=Packet.content+8;
   UINT MsgLength = htonl((UINT)(code.length));

   memcpy(pcPacket, &MsgLength, COM_MSG_LENGTH); //填写消息长度
   pcPacket+=COM_MSG_LENGTH; //指针后移COM_MSG_LENGTH位

   memcpy(pcPacket, code.content, code.length); //填写消息内容
   Packet.content[Packet.length] = 0;
}
INT CSocket::sendMessage(CCode &code)
{

   INT iNwrite=0;
   INT ret = 1; //must set 1 when initial

   if (m_socketState==CLOSE || m_iSockfd<0)
   {
      UniERROR("sendMessage() ERROR: socket state is CLOSE. m_socketState: %d. m_sockfd: %d",m_socketState,m_iSockfd);
      return 0;
   }
   while(hasUnsentCode() && ret==1)
   {
      if(b_writeNeedSelect)
         ret = preSendProcess(code);
      if(ret==1)
      {
         ret = sendUnsentCode(code);
      }
   }

   if(ret == 1) //when resend last unsent code succeed or no unsent code
   {
   
      if (code.length <= 0)
      {
         UniINFO("CSocket::sendMessage(): WARNING: code.length <= 0 !");
         return 1;
      }

#ifndef MYTEST
      iNwrite=send(m_iSockfd, code.content, code.length, MSG_DONTROUTE);
#else

      iNwrite=send(m_iSockfd, code.content, MYTEST, MSG_DONTROUTE);

      iNwrite += send(m_iSockfd, code.content + MYTEST, code.length - MYTEST, MSG_DONTROUTE);
      
#endif
      if (iNwrite > 0)
      {
         if (iNwrite==code.length)
         {
            return (1);
         }
         notenoughCounter++;
         if (notenoughCounter==1 || notenoughCounter%OUTPUT_NOTENOUGH_COUNTER==0)
            UniINFO(
                  "CSocket::sendMessage(): Write Socket byte is not enough. STORED to send-buffer. counter: %u",
                  notenoughCounter);
         ret = -2;
      }
      else if (iNwrite == 0)
      {
         UniERROR("CSocket::sendMessage(): Write Socket Error!! Maybe Pipe Down. result = 0");
         return 0;
      }
      else
      {
#ifndef WIN32   
         if ( errno == EWOULDBLOCK)
#else
         if ( errno == WSAEWOULDBLOCK)
#endif         
         {
            blockCounter++;
            if (blockCounter==1 || blockCounter%OUTPUT_BLOCK_COUNTER==0)
               UniINFO(
                     "CSocket::sendMessage(): Write Socket byte is none, blocked. errno = EWOULDBLOCK. STORED to send-buffer. counter: %u",
                     blockCounter);
            iNwrite = 0;
            ret = -2;
         }
         else
         {
            ret = -1;
            if (-1 == iNwrite)
            {
#ifndef WIN32
               if (errno == EPIPE || errno == ECONNRESET)
#else
               if (errno == EPIPE || errno == WSAECONNRESET)
#endif
               {
                  ret = -9;
                  UniERROR(
                        "CSocket::sendMessage(): Write Socket Error!! Pipe Down. errno = %s",
                        strerror(errno));
               }
               else
               {
                  UniERROR(
                        "CSocket::sendMessage(): Write Socket Error!!errno = %s",
                        strerror(errno));
               }
            }
            return ret;
         }
      }
   }

   
   if (ret == -2)
   {
      return backupToSendBuff(code.content+iNwrite, code.length-iNwrite, code.flag,
            "sendMessage()");
   }

   return ret;
}

INT CSocket::preSendProcess(CCode &code)
{
   INT ret;
   FD_ZERO(&fd_writeset);
   FD_SET(m_iSockfd, &fd_writeset);//注意，必须每次都重新设置。
   ret = select(m_iSockfd+1, NULL, &fd_writeset, NULL, &m_resendSelectWaitTime);
   if (ret > 0 && FD_ISSET(m_iSockfd, &fd_writeset))
   {
      b_writeNeedSelect = FALSE;
      sendAvailableCounter++;
      if (sendAvailableCounter==1 || sendAvailableCounter
            %OUTPUT_SENDAVAILABLE_COUNTER==0)
         UniINFO(
               "CSocket::preSendProcess(): Socket is available again. counter: %u",
               sendAvailableCounter);
      return 1;
   }
   else if (ret == 0)
   {
      sendUnavailableCounter++;
      if(sendUnavailableCounter==1 || sendUnavailableCounter%OUTPUT_SENDUNAVAILABLE_COUNTER==0)
      {
         
         UniINFO("CSocket::preSendProcess(): Write Socket byte is none. Socket is unavailable when writing. It will be STORED in send-buffer. counter: %u. remote_addr: %s, sock_fd: %d",
            sendUnavailableCounter,c_str_remote(),getFd());
      }
      ret = -2;
   }
   else
   {//error

#ifndef WIN32   
      if ( errno == EWOULDBLOCK)
#else
      if ( errno == WSAEWOULDBLOCK)
#endif         
      {
         blockCounter++;
         if (blockCounter==1 || blockCounter%OUTPUT_BLOCK_COUNTER==0)
            UniINFO(
                  "CSocket::preSendProcess(): Write Socket byte is none, blocked. errno = EWOULDBLOCK. It will be STORED in send-buffer and try to send next time. counter: %u.",
                  blockCounter);
         ret = -2;
      }
      else
      {

         ret = -1;
#ifndef WIN32
         if (errno == EPIPE || errno == ECONNRESET)
#else
         if (errno == EPIPE || errno == WSAECONNRESET)
#endif
         {
            ret = -9;
            UniERROR(
                  "CSocket::preSendProcess(): Write Socket Error!! Pipe Down. errno = %s",
                  strerror(errno));
         }
         else
         {
            UniERROR(
                  "CSocket::preSendProcess(): Write Socket Error!!errno = %s",
                  strerror(errno));
         }

      }
   }
      return ret;
}

INT CSocket::sendUnsentCode(CCode &code)
{
   INT ret = 1;
   INT iNwrite=0;
   if (pSendMsgBuf==NULL)
      return 1;
   UINT lengthToSend=
         m_socketSendBufSize<=pSendMsgBuf->filledLength() ? m_socketSendBufSize
               : pSendMsgBuf->filledLength();
   if (lengthToSend==0)
      return 1;
   iNwrite = pSendMsgBuf->readFromBuffer(NULL,lengthToSend);
   if (iNwrite> 0)
   {
      if (iNwrite == lengthToSend)
      {
         resendSucceedCounter++;
         if (resendSucceedCounter==1 || resendSucceedCounter
               %OUTPUT_RESENDSUCCEED_COUNTER==0)
            UniINFO(
                  "CSocket::sendUnsentCode(): resend last code succeed( %d bytes). counter: %u",
                  iNwrite, resendSucceedCounter);
         if (pSendMsgBuf->filledLength()>0) //there are packets in the sendbuff.
         {
            return backupToSendBuff(code.content, code.length, code.flag,
                  "sendUnsentCode()");
         }
         else
            return 1;
      }
      else if (iNwrite < lengthToSend)
      {
         notenoughCounter++;
         if (notenoughCounter==1 || notenoughCounter
               %OUTPUT_NOTENOUGH_COUNTER==0)
            UniINFO(
                  "CSocket::sendUnsentCode(): Write Socket byte is not enough. STORED to send-buffer. counter: %u.",
                  notenoughCounter);

         ret = -2;
      }
   }
   else if (iNwrite == 0)
   {
      UniERROR("CSocket::sendUnsentCode(): Write Socket Error!! Pipe Down. result = 0");
      ret = 0;
   }
   else
   {
#ifndef WIN32   
      if ( errno == EWOULDBLOCK)
#else
      if ( errno == WSAEWOULDBLOCK)
#endif
      {
         blockCounter++;
         if (blockCounter==1 || blockCounter%OUTPUT_BLOCK_COUNTER==0)
            UniINFO(
                  "CSocket::sendUnsentCode(): Write Socket byte is none, blocked. errno = EWOULDBLOCK. STORED to send-buff. counter: %u",
                  blockCounter);
         iNwrite = 0;
         ret = -2;
      }
      else
      {
         ret = -1;
         if (-1== iNwrite)
         {
#ifndef WIN32   
            if (errno == EPIPE || errno == ECONNRESET)
#else
            if (errno == EPIPE || errno == WSAECONNRESET)
#endif
            {
               ret = -9;
               UniERROR(
                     "CSocket::sendUnsentCode(): Write Socket Error!! Pipe Down. errno = %s",
                     strerror(errno));
            }
            else
            {
               UniERROR(
                     "CSocket::sendUnsentCode(): Write Socket Error!! errno = %s",
                     strerror(errno));
            }
         }
      }
   }

   return ret;
}

int CSocket::_writeToBuff(void *dst, const void *src, unsigned int length)
{
   int ret = length;
   memcpy(dst, src, length);
   return ret;
}

int CSocket::_sendBuffToSocket(void *dst, const void *src, unsigned int length)
{
   return send(m_iSockfd, (const char*)src, length, MSG_DONTROUTE) ;
}

int CSocket::_readFromBuff(void *dst, const void *src, unsigned int length)
{
   TStateMachine state = Wait;
   int bufferLength = pRecvMsgBuf->filledLength();
   int ret = 0;
   int lengthToRead;
   const int headLength = 8 + COM_MSG_LENGTH;
   char *message = (char *)src;

   lengthToRead = headLength;
   while (bufferLength > lengthToRead)
   {
      switch (state)
      {
      case Wait:
         if ((UCHAR)*message == ZERO)
         {
            state = Begin;
            lengthToRead--;
         }
         break;
      case Begin:
         if ((UCHAR)*message == ONE)
         {
            state=Appear;
            lengthToRead--;
         }
         else
         {
            if ((UCHAR)*message != ZERO)
            {
               state=Wait;
               lengthToRead = headLength;
            }
         }
         break;
      case Appear:
         if ((UCHAR)*message == ZERO)
         {
            state=Begin;
            lengthToRead = headLength - 1;
         }
         else if ((UCHAR)*message == ONE)
         {
            lengthToRead--;
            if (lengthToRead == 1 + COM_MSG_LENGTH)
            {
               state = Prepare;
            }
         }
         else
         {
            state=Wait;
            lengthToRead = headLength;
         }
         break;
      case Prepare:
         if ((UCHAR)*message == ZERO)
         {
            state = Found;
         }
         else
         {
            state = Wait;
            lengthToRead = headLength;
         }
         break;
      default:
         break;
      }
      ret++;
      message++;
      bufferLength--;
      if (state == Found)
      {
         ret += COM_MSG_LENGTH;
         bufferLength -= COM_MSG_LENGTH;
         memcpy(&lengthToRead, message, COM_MSG_LENGTH);
         if ((lengthToRead = ntohl(lengthToRead))> 0)
         {
            if (lengthToRead > SOCKET_CODE_MAXLENGTH)//error
            {
               lengthToRead = headLength;
               message += COM_MSG_LENGTH;
            }
            else if (lengthToRead > bufferLength)//not enough

            {
               return ret - headLength;
            }
            else//success

            {
               codeToReceive->length = lengthToRead;
               memcpy(codeToReceive->content, message + COM_MSG_LENGTH,
                     lengthToRead);
               return ret + lengthToRead;
            }
         }
      }
   }

   return ret - (headLength - lengthToRead);
}

int CSocket::_recvSocketIntoBuff(void *dst, const void *src, unsigned int length)
{
   int ret = read_socket_raw(m_iSockfd, (char *)dst, length);
   if (ret == 0)
   {
      if(!m_shortConnectionMode)
         UniINFO("Read Socket Error! read result: 0.  EoF in the buffer, Maybe Pipe Down!!");
   }
   else if (ret < 0)
   {
#ifndef WIN32
      if (errno == EWOULDBLOCK)
#else
      if (errno == WSAEWOULDBLOCK)
#endif
      {
         UniERROR(
               "Read Socket Error: read result: %d(return -2), errno = EWOULDBLOCK",
               ret);
         ret = -2;
      }
      else
      {
         UniERROR(
               "Read Socket Error: read result: %d(return -1), errno: %d(%s)",
               ret, errno, strerror(errno));
         ret = -1;
      }
   }
   return ret;
}
INT CSocket::backupToSendBuff(CHAR* codeContent, UINT codeLength, INT codeFlag,
      CHAR* funcName)
{
   INT ret = -2;
   if (pSendMsgBuf==NULL)
   {
      UINT size = MaxMsgLength;
      pSendMsgBuf = new TBuffer<CSocket>(size,this,&CSocket::_sendBuffToSocket,&CSocket::_writeToBuff);
      UniINFO(
            "Prepare for backup sending msg:  new socket backup SendMsgBuf  %u bytes succeed.",
            size);
   }

   if (codeLength<=pSendMsgBuf->freeLength()-MaxEmergencyLength)
   {
      pSendMsgBuf->writeIntoBuffer(codeContent, codeLength);

      backupCounter++;
      if (backupCounter==1 || backupCounter%OUTPUT_BACK_COUNTER==0)
         UniINFO(
               "CSocket::%s: Write Socket Blocked! backup this msg to SendMsgBuf succeed(%u bytes). counter: %u",
               funcName, codeLength, backupCounter);
   }
   else if(codeFlag==code_emergency && codeLength<=pSendMsgBuf->freeLength())
   {
      pSendMsgBuf->writeIntoBuffer(codeContent, codeLength);
      backupCounter++;
         UniINFO(
               "CSocket::%s: Write Socket Blocked! backup this emergency msg to SendMsgBuf succeed(%u bytes). counter: %u",
               funcName, codeLength, backupCounter);

   }
   else
   {
      fullCounter++;
      if(codeFlag==code_emergency)
      {   UniERROR(
               "CSocket::%s ERROR: Write Socket Error! Buffered msg is full(%u bytes), Sending is much too fast. discard this emergency msg. counter: %u",
               funcName, pSendMsgBuf->filledLength(), fullCounter);
      }   
      else if (fullCounter==1 || fullCounter%OUTPUT_FULL_COUNTER==0 )
      {   UniERROR(
               "CSocket::%s ERROR: Write Socket Error! Buffered msg is full(%u bytes), Sending is much too fast. discard it. counter: %u",
               funcName, pSendMsgBuf->filledLength(), fullCounter);
      }
	ret = -5;

   }

   return ret;

}

BOOL CSocket::openClient(CHAR* pcServerAddr, INT iServerPort,
      CHAR* pcClientAddr, INT iClientPort, struct timeval *timeout)
{
#ifdef WIN32
   return FALSE;

#else
   if (!socketInit(pcServerAddr, iServerPort, pcClientAddr, iClientPort))
   {
      return openClientResult(-1);//modified by lxm. 2008.10.22
   }

#ifndef WIN32
   int flags = fcntl(m_iSockfd, F_GETFL, 0);

   if (fcntl(m_iSockfd, F_SETFL, flags | O_NONBLOCK) < 0)
   {
      UniERROR("Set server socket non_block flag error.");
      m_iSockWait = -99;
      return openClientResult(-2);
   }
#else
   u_long nbFlag = 1;
   if (ioctlsocket(m_iSockfd, FIONBIO, &nbFlag)!=0)
   {
      UniERROR("Set server socket non_block flag error.");
      m_iSockWait = -99;
      return openClientResult(-2);
   }
#endif

   errno = 0;
   INT rt=connect(m_iSockfd, reinterpret_cast<struct sockaddr*>(&server),
         sizeof(server));
   if (rt < 0)
   {
      if (errno != EINPROGRESS)
      {
         return openClientResult(-3);
      }
   }
   if (rt == 0)
   {
#ifndef WIN32
      fcntl(m_iSockfd, F_SETFL, flags);
#endif
      return openClientResult(0);
   }

   //need to wait a timeout
   fd_set rset, wset;
   FD_ZERO(&rset);

   FD_SET(m_iSockfd, &rset);
   FD_ZERO(&wset);
   FD_SET(m_iSockfd, &wset);
   //wset = rset;

   if (timeout)
   {
      m_conAttemptTimeout = *timeout;
   }

   timeval tempTimeout=m_conAttemptTimeout;
   rt=select(m_iSockfd + 1, &rset, &wset, NULL, &tempTimeout);

   if (rt == 0)
   { //timeout
      errno = ETIMEDOUT;
      UniINFO("CSocket::openClient()  ERROR: connect timeout: sec: %d. orgin_sec: %d",tempTimeout.tv_sec,m_conAttemptTimeout.tv_sec);
      return openClientResult(-4);
   }
   if (FD_ISSET(m_iSockfd, &rset) || FD_ISSET(m_iSockfd, &wset))
   {
      INT error;
      socklen_t len = sizeof(error);

      if (getsockopt(m_iSockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
      {
         return openClientResult(-5);
      }
      if (error)
      {//如果error的值不是零，则没有连接成功
         errno = error;
         return openClientResult(-6);
      }
#ifndef WIN32
      fcntl(m_iSockfd, F_SETFL, flags);
#endif
      return openClientResult(0);
   }
   else
   {
      return openClientResult(-7);
   }
#endif
}

BOOL CSocket::socketInit(CHAR* pcServerAddr, INT iServerPort,
      CHAR* pcClientAddr, INT iClientPort)
{
   m_socketRole = socket_client;
   m_remoteAddrStr[0] = 0; //added by lxm. 2008.10.30.  for c_str().


   if (pcServerAddr == NULL)
   {
      strcpy(m_remoteServerAddr, "127.0.0.1");
      UniERROR("openClient(): remoteServerAddr is NULL. we will use 127.0.0.1 as default.");
      //return FALSE;
   }
   else
   {
      if (pcServerAddr[0] == 0 || strcmp(pcServerAddr, "0")==0 || strcmp(
            pcServerAddr, "0.0.0.0") ==0)
         strcpy(m_remoteServerAddr, "127.0.0.1");
      else
         strcpy(m_remoteServerAddr, pcServerAddr);

   }
   if (pcClientAddr == NULL)
   {
      m_localClientAddr[0]=0;
   }
   else
   {
      strcpy(m_localClientAddr, pcClientAddr);
   }

   if (iServerPort<=0)
   {
      UniERROR("Invalid serverPort: %d", iServerPort);
      return FALSE;
   }
   if (iClientPort<0)
   {
      UniERROR("Invalid clientPort: %d", iClientPort);
      return FALSE;
   }

   m_remoteServerPort=iServerPort;
   m_localClientPort = iClientPort;

   m_iSockfd=socket(AF_INET,SOCK_STREAM,0);
   if (m_iSockfd < 0)
   {
      m_iSockfd=-99;
      m_socketState = CLOSE;
      return FALSE;
   }

   _setSendBufSize(m_socketSendBufSize); //设置服务端发送缓冲区大小
   _setRecvBufSize(m_socketRecvBufSize); //设置服务端接收缓冲区大小
   setClientLinger(1); //设置客户端Linger
   setClientAlive(1); //设置客户端Alive

   server.sin_family=AF_INET; //socket使用的协议
   server.sin_addr.s_addr=inet_addr(m_remoteServerAddr);
   server.sin_port=htons(iServerPort);

   if (iClientPort>0 || m_localClientAddr[0]!=0) //需要绑定客户端IP或端口

   {
      if (!(iClientPort==0 && (strcmp(m_localClientAddr, "0")==0 || strcmp(
            m_localClientAddr, "0.0.0.0")==0)))
      {
         struct sockaddr_in sClient;
         sClient.sin_family=AF_INET;
         sClient.sin_port=htons(iClientPort);
         if (m_localClientAddr[0]==0 || strcmp(m_localClientAddr, "0")==0
               || strcmp(m_localClientAddr, "0.0.0.0")==0)
            sClient.sin_addr.s_addr=htonl(INADDR_ANY);
         else
            sClient.sin_addr.s_addr=inet_addr(m_localClientAddr);
         int rtBind;
         rtBind =bind(m_iSockfd, (struct sockaddr *)&sClient,
               sizeof(sClient));
         if (rtBind!=0)
         {
            UniERROR(
                  "open client socket error: bind() failed: %d. clientAddr:%s, clientPort: %d.",
                  rtBind, m_localClientAddr, iClientPort);
            return FALSE;
         }
         else
         {
            UniINFO("Bind Client Socket Ok! clientAddr:%s, clientPort: %d. sock_fd: %d",
                  m_localClientAddr, iClientPort,getFd());

         }
      }
   }
   return TRUE;
}

BOOL CSocket::openClientResult(int rt)
{
   if (rt < 0)
   {
      m_socketState = CLOSE; //socket状态置为CLOSE
      if(m_iSockfd>0) //added by lxm. 2008.10.22
         close_socket(m_iSockfd);
      m_iSockfd=-99; //modified by chl 2004-3-31.如果不赋值则GetFd仍然返回一个有效值，导致外部使用fd时出错。
     UniINFO("open client socket error: connect() failed: %d. errorno: %s. timeout: %d sec. remoteAddr: %s, remotePort: %d",
            rt, strerror(errno),m_conAttemptTimeout.tv_sec,m_remoteServerAddr, m_remoteServerPort);
      return FALSE;
   }
   else
   {
      m_socketState = OPEN; //socket状态置为OPEN
      setNoBlock(m_bNoBlock);
      _setSendBufSize(m_socketSendBufSize); //设置服务端发送缓冲区大小
      _setRecvBufSize(m_socketRecvBufSize); //设置服务端接收缓冲区大小

      FD_SET(m_iSockfd, &fd_writeset);
      INT val= SO_SNDLOWAT_VALUE;
      setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDLOWAT, (CHAR*)&val, sizeof(val));
      b_writeNeedSelect = FALSE;
      UniINFO("Open client socket succeed. remoteAddr: %s. clientPort: %d.  sock_fd: %d",
               c_str_remote(), m_localClientPort,getFd());

   
      m_connectedCounter++;
      return TRUE;
   }
}

BOOL CSocket::hasBufferedCode()
{
   return recvBufHasMessage;
}

INT CSocket::recvCode(CCode& code)
{

   code.length =0;

   if (m_iSockfd < 1)
   {
      UniERROR("CSocket::recvCode -> Socket ID less than ZERO! Socket NOT EXIST!!!");
      return (-9);
   }

   if (code.content==NULL)
   {
      UniERROR("CSocket::recvCode -> Receive code.content is NULL, can not receive new Msg!");
      return (-4);
   }

   if (m_bIsNeedFindHead)
   {
      return recvCodeByFindHead(code);
   }
   else
   {
      return recvCodeDirect(code);
   }

}

INT CSocket::recvCodeDirect(CCode& code)
{

   UINT iRecvLength=MaxRecvLength;//<=MaxMsgLength;

   INT ReadResult= read_socket_raw(m_iSockfd, code.content, iRecvLength);

   if (ReadResult > 0) //读取socket成功
   {
      if (ReadResult > MaxMsgLength)
      {
         UniERROR(
               "CSocket::recvCodeDirect() -> Read Socket Error ! The read result is out max length(%d): %d.",
               MaxMsgLength, ReadResult);
         return (-5);
      }
      if (ReadResult==MaxRecvLength)
      {
         readDirectFullCounter++;
         if(readDirectFullCounter==1 || readDirectFullCounter%OUTPUT_READDIRECT_FULL_COUNTER==0)
         UniINFO(
               "WARNING in CSocket::recvCodeDirect(): The socket buff is full(%d). Maybe the sender is too more fast than the receiver. counter: %u",
               ReadResult,readDirectFullCounter);

      }

      code.length=ReadResult; //填写消息长度
      return (1);
   }
   else if (ReadResult==0)
   {
      if(!m_shortConnectionMode)
         UniINFO("CSocket::recvCodeDirect() -> Read Socket Error ! ReadResult: 0.  Pipe Down!!");
      return 0;
   }
   else
   {
#ifndef WIN32
      if (errno == EWOULDBLOCK)
#else
      if (errno == WSAEWOULDBLOCK)
#endif
      {
         UniINFO(
               "WARNING: CSocket::recvCodeDirect() -> Read Socket Error ! ReadResult: %d ( return 1), errno = EWOULDBLOCK. set code.length=0.",
               ReadResult);
         code.length = 0;
         return 1;
      }
      UniERROR(
            "CSocket::recvCodeDirect() -> Read Socket Error !  ReadResult: %d(return -1), errno: %d(%s)",
            ReadResult, errno, strerror(errno));
      return (-1); //读取socket失败. modified by lxm. 2008.10.10

   }

}

INT CSocket::recvCodeByFindHead(CCode& code)
{
   INT ret;
   if (!recvBufHasMessage)
   {
      ret = pRecvMsgBuf->writeIntoBuffer(NULL, pRecvMsgBuf->size());
      if (ret <= 0)
      {
         return ret;
      }
      recvBufHasMessage = TRUE;
   }
   codeToReceive = &code;
   ret = pRecvMsgBuf->readFromBuffer(NULL, MaxRecvLength);
   if (ret > 0)
   {
      if (pRecvMsgBuf->filledLength() == 0)
      {
         recvBufHasMessage = FALSE;
      }
      if (codeToReceive->length > 0)
      {
         return 1;
      }
      else
      {
         uncompleteDropCounter++;
         if(uncompleteDropCounter==1 || uncompleteDropCounter%OUTPUT_UNCOMPLETE_DROP_COUNTER==0)
           UniERROR("Warning in CSocket::recvCodeByFindHead(): some codes are uncomplete and droped, length = %d. counter: %u", ret,uncompleteDropCounter);

         recvBufHasMessage = FALSE;
         ret = -2;
      }
   }
   else
   {
      uncompleteNoDropCounter++;
      if(uncompleteNoDropCounter==1 || uncompleteNoDropCounter%OUTPUT_UNCOMPLETE_NODROP_COUNTER==0)
         UniERROR("Warning in CSocket::recvCodeByFindHead(): some codes are uncomplete. counter: %u", uncompleteNoDropCounter);

      recvBufHasMessage = FALSE;
      ret = -2;
   }
   
   if(ret==-2)
      pRecvMsgBuf->rearrange();

   return ret;
}

CUri::CUri()
{
   mHost[0]=0;
   mPort = 0;
   mSessionId[0]=0;

}

CUri::CUri(const char* host,const int port,const char* sessionId)
{
   mHost[0]=0;
   mSessionId[0]=0;
   if(host!=0)
      strcpy(mHost,host);
   if(sessionId!=0)
      strcpy(mSessionId,sessionId);
   mPort = port;
}

CUri::CUri(sockaddr_in addr)
{
   strcpy(mHost, inet_ntoa(addr.sin_addr));
   mPort = ntohs(addr.sin_port);

}


CUri::CUri(const CUri& uri)
{
   strcpy(mHost,uri.host());
   mPort = uri.port() ;

}


CUri& CUri::operator=(const CUri& uri)
{
   strcpy(mHost,uri.host());
   mPort = uri.port() ;
   return *this;
}

bool CUri::operator==(const CUri& r) const
{
   if(strcmp(mHost,r.host())==0 && mPort == r.port()  )
      return true;
   else
      return false;
}

bool CUri::operator<(const CUri& r) const
{
   if(mPort < r.port()) return true;
   else return false;
}

bool CUri::approxEqualsTo(const CUri& r) const
{
   if(strcmp(mHost,r.host())==0 && mPort == r.port())
      return true;
   else
      return false;
}


CUri::~CUri()
{
}

const char* CUri::host() const
{
   return (char*)mHost;
}

const int  CUri::port() const
{
   return mPort;
}

const char* CUri::sessionId() const
{
   return (char*)mSessionId;
}

const char* CUri::c_str() const
{
   static char temp[256];
   temp[0]=0;
   sprintf(temp,"%s:%d",mHost,mPort);
   return temp;
}

void  CUri::setHost(const char* host)
{
   if(NULL!=host)
      strcpy(mHost,host);

}

void  CUri::setPort(const int port)
{
   mPort = port;

}

void  CUri::setSessionId(const char* sessionId)
{
   if(NULL!=sessionId)
      strcpy(mSessionId,sessionId);
}

CTcpConnection::CTcpConnection()
{
   mLinkId = 0;
   recvCounter = 0;
   sendCounter = 0;
}

CTcpConnection::~CTcpConnection()
{
}

bool CTcpConnection::connect(const char* remoteHost,  int remotePort,  char* localHost,int localPort, BOOL needFindHead)
{
   if(openClient(const_cast<char*>(remoteHost),remotePort,localHost, localPort)==1)
   {
      setNeedFindHead(needFindHead);
      return true;
   }
   else
      return false;
}


void CTcpConnection::setFd(int fd)
{
   m_iSockfd = fd;
   if(fd<0)
      m_socketState = CLOSE;
   else
      m_socketState = OPEN;

}

int  CTcpConnection::sockfd()
{
   return getFd();
}

void CTcpConnection::buildFdSet(CFdSet &fdSet)
{
   if(getFd()>0)
   {
      fdSet.setRead(getFd());
      if(needSend())
         fdSet.setWrite(getFd());
   }
}

int CTcpConnection::processRecv()
{
   return 0;
}

void CTcpConnection::processSend()
{
}

int CTcpConnection::process(CFdSet &fdSet)
{
   if(sockfd()<=0) 
      return 0;
   if(fdSet.readyToWrite(sockfd())) 
      processSend();
   if(fdSet.readyToRead(sockfd())) 
      return processRecv();
   return 0;
}

bool CTcpConnection::closeAnyway()
{
   return closeSocket();
}

int CTcpConnection::receive(CCode& code)
{
   int re = recvCode(code);
   if(re > 0)
      recvCounter ++;
   return re;
}

int CTcpConnection::send(CCode& code)
{
   int re = sendCode(code);
   if(re > 0)
      sendCounter ++;
   return re;
}

void CTcpConnection::printState(CStr& str)
{
   str.fCat("%-6s %-5d %-8d %-8d %-6d %s\n"
      , m_socketRole == socket_client ? "Client" : "Server"
      , getFd()
      , sendCounter
      , recvCounter
      , linkId()
      , mRemoteUri.c_str()
   );
}

CUri& CTcpConnection::remoteUri()
{
   return mRemoteUri;
}

CTcpListener::CTcpListener(BOOL needFindHead)
{
   setNeedFindHead(needFindHead);
}

CTcpListener::~CTcpListener()
{
}

bool CTcpListener::open(char* localAddr, int localPort)
{
   if(openServer(localAddr,localPort)>=0)
      return true;
   else
      return false;
}

void CTcpListener::buildFdSet(CFdSet& fdSet)
{
   if(getWaitFd()>0)
      fdSet.setRead(getWaitFd());
}

void CTcpListener::dupConnection(CTcpConnection* tcpConnection)
{
   tcpConnection->setFd(getFd());
   tcpConnection->setNeedFindHead(m_bIsNeedFindHead);
   tcpConnection->setCommRole(socket_server);
   tcpConnection->getClientSockAddr() = client;

   CUri remote_Uri(client);
   tcpConnection->remoteUri() = remote_Uri;

   if(m_shortConnectionMode)
      tcpConnection->setShortConnectionMode();
}

int CTcpListener::accept(CFdSet& fdSet)
{
   if(getWaitFd()<=0)
      return 0;

   if(!fdSet.readyToRead(getWaitFd()))
      return 0;

   if(reAccept())
      return 1;
   return 0;
}

bool CTcpListener::close()
{
   return closeSocket();
}

void CTcpListener::printState(CStr& str)
{
   str.fCat("%-5d %-5d"
      , m_localServerPort
      , getWaitFd()
   );
}

////////////////////////// For CTcpConnectionManager //////////////
CTcpConnectionManager::CTcpConnectionManager()
{
}

CTcpConnectionManager::~CTcpConnectionManager()
{
   closeAll();
}

void CTcpConnectionManager::addConnection(CTcpConnection* connection)
{
   if(connection==NULL) return ;
   mConnections.push_back(connection);
}

CTcpConnection* CTcpConnectionManager::find(const CUri& remoteUri)
{
   for(CTcpConnectionList::iterator i=mConnections.begin(); i!=mConnections.end(); i++)
   {
      if((*i)->remoteUri() == remoteUri)
         return (*i);
   }
   for(CTcpConnectionList::iterator it=mConnections.begin();it!=mConnections.end();it++)
   {
      if((*it)->remoteUri().approxEqualsTo(remoteUri))
         return (*it);
   }
   return NULL;
}

CTcpConnection* CTcpConnectionManager::findByLinkId(UINT linkId)
{
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();i++)
   {
      if((*i)->linkId() == linkId)
         return (*i);
   }
   return NULL;
}

CTcpConnection* CTcpConnectionManager::findByConnId(UINT connid)
{
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();i++)
   {
      if((*i)->connId() == connid)
         return (*i);
   }
   return NULL;
}

bool CTcpConnectionManager::close(const CUri &remoteUri)
{
   bool rt = false;
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();)
   {
      if(((*i)->remoteUri() == remoteUri)||( (*i)->remoteUri().approxEqualsTo(remoteUri) ) )
      {
         rt = (*i)->close();
         delete (*i);
         i=mConnections.erase(i);
         UniERROR("connection is closed succeed. %s",remoteUri.c_str());
         return rt;

      }
      else
         ++i;
   }
   UniERROR("CTcpConnectionManager::close() ERROR --> no such connection: %s.",remoteUri.c_str());
   return rt;
}

bool CTcpConnectionManager::closeByConnId(UINT connId)
{
   bool rt = false;
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();)
   {
      if (connId==(*i)->connId())
      {
         rt = (*i)->close();
         delete (*i);
         i=mConnections.erase(i);
         UniERROR("connid: %d is closed succeed.",connId);
         return rt;
      }
      else
         ++i;
   }
   UniERROR("CTcpConnectionManager::close() ERROR --> no such connectionId: %d.",connId);
   return rt;
}

bool CTcpConnectionManager::closeByLinkId(UINT linkId)
{
   bool rt = false;
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();)
   {
      if((*i)->linkId() == linkId) 
      {
         rt = (*i)->close();
         delete (*i);
         i=mConnections.erase(i);
         UniERROR("link %d is closed succeed",linkId);
         return rt;
      }
      else
         ++i;
   }
   UniERROR("CTcpConnectionManager::close() ERROR --> no such connection:linkID=%d",linkId);
   return rt;
}

void CTcpConnectionManager::closeAll()
{
   if(mConnections.empty()) return;
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();)
   {
      (*i)->close();
      delete (*i);
      i=mConnections.erase(i);
   }
}

void CTcpConnectionManager::buildFdSet(CFdSet &fdSet)
{
   if(mConnections.empty()) return;
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();i++)
   {
      (*i)->buildFdSet(fdSet);
   }
}

int CTcpConnectionManager::procFdSet(CFdSet& fdSet)
{
   if(mConnections.empty())
      return 0;

   int n = 0;
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();)
   {
      int re = (*i)->process(fdSet);
      if(re < 0)
      {
         (*i)->close(); 
         delete (*i);
         i=mConnections.erase(i);
         continue;
      }
      else if(re > 0)
         n++;
      ++i;
   }
   return n;
}

void CTcpConnectionManager::printState(CStr& str)
{
   str << "---- Connections ----\n";
   str << "TYPE   FD    SEND     RECEIVE  LINKID REMOTE-ADDR\n";
   printConnectionsState(str);
}

void CTcpConnectionManager::printConnectionsState(CStr& str)
{
   for(CTcpConnectionList::iterator i=mConnections.begin();i!=mConnections.end();i++)
   {
      (*i)->printState(str);
   }
}

