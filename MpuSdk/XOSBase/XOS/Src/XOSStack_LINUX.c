/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSStack_Funs_LINUX.c
 *  文件标识: 
 *  摘    要: linux平台下协议栈相关函数的实现	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-16 18:27:18
 *  修正备注: 
 *  
 */

#include "../XOS.h"

//windows(XP SP3)中,如果send函数不管发多大的数据,
//只要能够发,都一把放在缓冲中的慢慢发,并且返回成功
//这样如果传入的长度太大就会占用大量的内存,所以需要人为的进行拆包.
//为了避免这种平台的不一致性,TCP发送的时候都进行人为拆包发送.
//似乎NDK也存在这样的问题.超过一定长度的包就使用malloc函数分配内存存放.
#define TCP_SEND_PACKET_LEN	1400	//实际上以太网中传的是1426个
#define lastSockErrno	errno
#define SOCKET_ERROR	-1
#define INVALID_SOCKET	-1

static XOS_SOCKET XOS_TCPSocket()
{
	XOS_SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("tcp socket failed: %d.\r\n", lastSockErrno);
		return XOS_INVALID_SOCKET;
	}
	//设置Socket选项
	xos_setsockblock(sock);
	xos_setsockrecvbuf(sock, 8*1024);
	xos_setsocksendbuf(sock, 8*1024);
	xos_setsocknodelay(sock);
	xos_setsockkeepalive(sock);
	xos_setsocklinger(sock, 1, 0);
	return sock;
}

static XOS_SOCKET XOS_UDPSocket()
{
	XOS_SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		printf("udp socket failed: %d.\r\n", lastSockErrno);
		return XOS_INVALID_SOCKET;
	}
	//设置Socket选项
	xos_setsockblock(sock);
	xos_setsockrecvbuf(sock, 8*1024);
	xos_setsocksendbuf(sock, 8*1024);
	xos_setsocknodelay(sock);
	setsockrecverr(sock);
	return sock;
}

static XOS_BOOL XOS_TCPProbe(XOS_SOCKET sock, int iTimeOut, XOS_BOOL bWrite)
{
	int rv;
	fd_set writefds;
	fd_set readfds;
	fd_set exceptfds;
	struct timeval timeout;
	
	if (iTimeOut == 0)
	{
		iTimeOut = 0x7FFFFFFF;
	}
	timeout.tv_sec = iTimeOut;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	FD_ZERO(&writefds);
	FD_SET(sock, &writefds);
	FD_ZERO(&exceptfds);
	FD_SET(sock, &exceptfds);
	if (bWrite)
	{
		rv = select(sock+1, NULL, &writefds, &exceptfds, &timeout);
	}
	else
	{
		rv = select(sock+1, &readfds, NULL, &exceptfds, &timeout);
	}
	if (rv == SOCKET_ERROR)
	{
		printf("%d tcp probe select failed: %d.\r\n", sock, lastSockErrno);
		return XOS_FALSE;
	}
	else if (rv > 0)
	{
		if (FD_ISSET(sock, &exceptfds))
		{
			printf("%d tcp probe select except: %d.\r\n", sock, lastSockErrno);
			return XOS_FALSE;
		}
		
		if (bWrite)
		{
			if (FD_ISSET(sock, &writefds))
			{
				return XOS_TRUE;
			}
		}
		else
		{
			if (FD_ISSET(sock, &readfds))
			{
				return XOS_TRUE;
			}
		}
		
		printf("%d tcp probe impossible: %d.\r\n", sock, lastSockErrno);
		return XOS_FALSE;
	}
	else
	{
		//超时
		return XOS_FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////

XOS_PSZ XOS_U32ToAddr(XOS_U32 u32IP, char szIP[XOS_MAX_IPSTR_LEN])
{
	memset(szIP, 0, XOS_MAX_IPSTR_LEN);
	sprintf(szIP, "%d.%d.%d.%d", 
      			 (u32IP >>  0) & 0x000000FF,
				 (u32IP >>  8) & 0x000000FF,
				 (u32IP >> 16) & 0x000000FF,
				 (u32IP >> 24) & 0x000000FF); 
	return (XOS_PSZ)szIP;
}

XOS_U32 XOS_AddrToU32(XOS_CPSZ cpszAddr)
{
	return (XOS_U32)inet_addr(cpszAddr);
}

void XOS_CloseSocket(XOS_SOCKET sock)
{
	close(sock);
	printf("%d close socket.\r\n", sock);
}

XOS_SOCKET XOS_TCPConnect(XOS_U32 u32IP, XOS_U16 u16Port, int iTimeOut)
{
	XOS_SOCKET sock;

	sock = XOS_TCPConnectNB(u32IP, u16Port);
	if (sock == XOS_INVALID_SOCKET)
	{
		printf("%d tcp connect failed: %d.\r\n", sock, lastSockErrno);
		XOS_CloseSocket(sock);
		return XOS_INVALID_SOCKET;
	}
	
	if (!XOS_TCPProbe(sock, iTimeOut, XOS_TRUE))
	{
		//尝试超时,直接退出返回失败
		XOS_CloseSocket(sock);
		printf("%d tcp connect timeout.\r\n", sock);
		return XOS_INVALID_SOCKET;
	}
	xos_setsockblock(sock);

	printf("%d tcp connect ok.\r\n", sock);
	return sock;
}

XOS_SOCKET XOS_TCPConnectNB(XOS_U32 u32IP, XOS_U16 u16Port)
{
	int rv;
	XOS_SOCKET sock;
	struct sockaddr_in serverAddr;
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = u32IP;
	serverAddr.sin_port = htons(u16Port); // htons把内存中的16位(2字节)的端口转换为网络字节序
	printf("tcp connect nonblock: %s:%d.\r\n", inet_ntoa(serverAddr.sin_addr), u16Port);

	sock = XOS_TCPSocket();
	if (sock == XOS_INVALID_SOCKET)
	{
		return XOS_INVALID_SOCKET;
	}
	printf("%d tcp connect %s:%d nonblock sock ok.\r\n", sock, inet_ntoa(serverAddr.sin_addr), u16Port);
	
	xos_setsocknonblock(sock);
	rv = connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		if ((rv == EWOULDBLOCK) || (rv == EINPROGRESS))
		{
			printf("%d tcp connect %s:%d nonblock ok.\r\n", sock, inet_ntoa(serverAddr.sin_addr), u16Port);
			return sock;
		}
		else
		{
			printf("%d tcp connect nonblock failed: %d.\r\n", sock, rv);
			XOS_CloseSocket(sock);
			return XOS_INVALID_SOCKET;
		}
	}
	printf("%d tcp connect %s:%d nonblock ok.\r\n", sock, inet_ntoa(serverAddr.sin_addr), u16Port);
	return sock;
}

static int pasersoketerr(int iErr)
{
	//这些都是根据设备上测试的结果, 和linux下的/usr/include/asm-generic/errno.h中的值不一样
	if (iErr == ENETUNREACH)
	{
		return XOS_SOCKETERR_NOROUTE;
	}
	else if (iErr == ECONNREFUSED)
	{
		return XOS_SOCKETERR_NOSERVER;
	}
	else if (iErr == EHOSTUNREACH || iErr == ETIMEDOUT)
	{
		return XOS_SOCKETERR_UNREACHABLE;
	}
	else
	{
		return -1;
	}
}

int XOS_TCPConnectProbeWithErrNo(XOS_SOCKET sock)
{
	int rv;
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	struct timeval timeout;
    
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	FD_ZERO(&writefds);
	FD_SET(sock, &writefds);
	FD_ZERO(&exceptfds);
	FD_SET(sock, &exceptfds);
	rv = select(sock+1, &readfds, &writefds, &exceptfds, &timeout);
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		printf("%d tcp connect probe select failed: %d.\r\n", sock, rv);
		return pasersoketerr(rv);
	}
	else if (rv > 0)
	{
		if (FD_ISSET(sock, &exceptfds))
		{
			rv = lastSockErrno;
			printf("%d tcp connect probe select except: %d.\r\n", sock, rv);
			return pasersoketerr(rv);
		}
		else if (FD_ISSET(sock, &readfds) || FD_ISSET(sock, &writefds))
		{
			//连接成功
			int error;
			socklen_t len = sizeof(error);
			if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			{
				printf ("%d tcp connect probe getsockopt failed.\n", sock);
				return pasersoketerr(error);
			}
			if (error == 0)
			{
				printf("%d tcp connect probe ok.\r\n", sock);
				return 0;
			}
			else
			{
				printf("%d tcp connect probe getsockopt error: %d.\n", sock, error);
				return pasersoketerr(error);
			}
		}
		else
		{
			rv = lastSockErrno;
			printf("%d tcp connect probe impossible: %d.\r\n", sock, rv);
			return pasersoketerr(rv);
		}
	}
	else
	{
		return 1;
	}
}
int XOS_TCPConnectProbe(XOS_SOCKET sock)
{
	int rv;
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	FD_ZERO(&writefds);
	FD_SET(sock, &writefds);
	FD_ZERO(&exceptfds);
	FD_SET(sock, &exceptfds);
	rv = select(sock+1, &readfds, &writefds, &exceptfds, &timeout);
	if (rv == SOCKET_ERROR)
	{
		printf("%d tcp connect probe select failed: %d.\r\n", sock, lastSockErrno);
		return -1;
	}
	else if (rv > 0)
	{
		if (FD_ISSET(sock, &exceptfds))
		{
			printf("%d tcp connect probe select except: %d.\r\n", sock, lastSockErrno);
			return -1;
		}
		else if (FD_ISSET(sock, &readfds) || FD_ISSET(sock, &writefds))
		{
			//连接成功
			int error;
			socklen_t len = sizeof(error);
			if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			{
				printf ("%d tcp connect probe getsockopt failed.\n", sock);
				return -1;
			}
			if (error == 0)
			{
				printf("%d tcp connect probe ok.\r\n", sock);
				return 0;
			}
			else
			{
				printf("%d tcp connect probe getsockopt error: %d.\n", sock, error);
				return -1;
			}
		}
		else
		{
			printf("%d tcp connect probe impossible: %d.\r\n", sock, lastSockErrno);
			return -1;
		}
	}
	else
	{
		return 1;
	}
}

int XOS_TCPSendNB(XOS_SOCKET sock, char *pSendBuf, int iSendLen)
{
	int rv;
	rv = send(sock, pSendBuf, iSendLen, 0);
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		if ((rv == EWOULDBLOCK) || (rv == ENOBUFS))
		{
			return 0;
		}
		else
		{
			printf("%d tcp send nonblock failed: %d.\r\n", sock, rv);
			return -1;
		}
	}
	else
	{
		return rv;
	}
}

XOS_BOOL XOS_TCPSendData(XOS_SOCKET sock, char *pSendBuf, int iSendLen, int iTimeOut)
{
	int rv;
	//xos_setsockblock(sock);
	while (iSendLen > 0)
	{
		if (!XOS_TCPProbe(sock, iTimeOut, XOS_TRUE))
		{
			printf("%d tcp send data timeout.\r\n", sock);
			return XOS_FALSE;
		}
		if (iSendLen > TCP_SEND_PACKET_LEN)
		{
			rv = send(sock, pSendBuf, TCP_SEND_PACKET_LEN, 0);
		}
		else
		{
			rv = send(sock, pSendBuf, iSendLen, 0);
		}
		if (rv == SOCKET_ERROR)
		{
			printf("%d tcp send data failed: %d.\r\n", sock, lastSockErrno);
			return XOS_FALSE;
		}
		//printf("%d %d\r\n", iSendLen, rv);
		pSendBuf += rv;
		iSendLen -= rv;
	}
	return XOS_TRUE;
}

int XOS_TCPSendDataNB(XOS_SOCKET sock, char *pSendBuf, int *piSendLen, int iDataLen)
{
	int rv;
	XOS_BOOL bSendOK = XOS_FALSE;
	int iSendLen;

	//xos_setsocknonblock(sock);
	while (1)
	{
		if ((iDataLen-(*piSendLen)) > TCP_SEND_PACKET_LEN)
		{
			iSendLen = TCP_SEND_PACKET_LEN;
		}
		else
		{
			iSendLen = iDataLen-(*piSendLen);
		}
		rv = send(sock, pSendBuf+(*piSendLen), iSendLen, 0);
		if (rv == SOCKET_ERROR)
		{
			rv = lastSockErrno;
			if ((rv == EWOULDBLOCK) || (rv == ENOBUFS))
			{
				if (!bSendOK)
				{
					return 1;
				}
				else
				{
					//哪怕发送一个字节的情况
					return 2;
				}
			}
			else
			{
				printf("%d tcp send data nonblock failed: %d.\r\n", sock, rv);
				return -1;
			}
		}
		if (rv != 0)
		{
			bSendOK = XOS_TRUE;
		}
		(*piSendLen) += rv;
		
		if ((*piSendLen) == iDataLen)
		{
			//发送完了,把这个置成0,防止上层忘记赋0
			(*piSendLen) = 0;
			return 0;
		}
	}
}

int XOS_TCPSendCharNB(XOS_SOCKET sock, char c)
{
	int rv;
	int iLen = 0;
	rv = XOS_TCPSendDataNB(sock, &c, &iLen, 1);
	if (rv == 0)
	{
		return 0;
	}
	else if ((rv == 1) || (rv == 2))
	{
		return 1;
	}
	else //-1
	{
		return -1;
	}
}

XOS_BOOL XOS_TCPRecvData(XOS_SOCKET sock, char *pRecvBuf, int iRecvLen, int iTimeOut)
{
	int rv;
	while (1)
	{
		rv = XOS_TCPRecv(sock, pRecvBuf, iRecvLen, iTimeOut);
		if (rv == -1)
		{
			printf("%d tcp recv data failed.\r\n", sock);
			return XOS_FALSE;
		}
		
		pRecvBuf += rv;
		iRecvLen -= rv;
		
		if (iRecvLen == 0)
		{
			return XOS_TRUE;
		}
	}
}

int XOS_TCPRecvDataNB(XOS_SOCKET sock, char *pRecvBuf, int *piRecvLen, int iDataLen)
{
	int rv;

	//xos_setsocknonblock(sock);
	rv = XOS_TCPRecvNB(sock, pRecvBuf+(*piRecvLen), iDataLen-(*piRecvLen));
	if (rv == 0)
	{
		return 1;
	}
	else if (rv == -1)
	{
		printf("%d tcp recv data nonblock failed: %d.\r\n", sock, rv);
		return -1;
	}

	(*piRecvLen) += rv;
	//printf("%d %d\r\n", (*piRecvLen), rv);
	
	if ((*piRecvLen) == iDataLen)
	{
		//接收完了,把这个置成0,防止上层忘记赋0
		(*piRecvLen) = 0;
		return 0;
	}

	//哪怕接收一个字节的情况
	return 2;
}

int XOS_TCPRecv(XOS_SOCKET sock, char *pRecvBuf, int iMaxRecvLen, int iTimeOut)
{
	int rv;
	if (!XOS_TCPProbe(sock, iTimeOut, XOS_FALSE))
	{
		printf("%d tcp recv timeout.\r\n", sock);
		return -1;
	}
	//xos_setsockblock(sock);
	rv = recv(sock, pRecvBuf, iMaxRecvLen, 0);
	if (rv == SOCKET_ERROR)
	{
		printf("%d tcp recv failed: %d.\r\n", sock, lastSockErrno);
		return -1;
	}
	else if (rv == 0)
	{
		printf("%d tcp recv gracefully failed: %d.\r\n", sock, lastSockErrno);
		return -1;
	}
	return rv;
}

int XOS_TCPRecvNB(XOS_SOCKET sock, char *pRecvBuf, int iMaxRecvLen)
{
	int rv;
	//xos_setsocknonblock(sock);
	rv = recv(sock, pRecvBuf, iMaxRecvLen, 0);
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		if (rv == EWOULDBLOCK)
		{
			return 0;
		}
		else
		{
			printf("%d tcp recv nonblock failed: %d.\r\n", sock, rv);
			return -1;
		}
	}
	else if (rv == 0)
	{
		printf("%d tcp recv nonblock gracefully failed: %d.\r\n", sock, lastSockErrno);
		return -1;
	}
	return rv;
}

XOS_BOOL XOS_TCPRecvChar(XOS_SOCKET sock, char *pChar, int iTimeOut)
{
	return XOS_TCPRecvData(sock, pChar, 1, iTimeOut);
}

int XOS_TCPRecvCharNB(XOS_SOCKET sock, char *pChar)
{
	int iRecvLen = 0;
	return XOS_TCPRecvDataNB(sock, pChar, &iRecvLen, 1);
}

int XOS_TCPRecvHead(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_PSZ pszTail, int iTimeOut)
{
	int rv;
	char *psz;

	psz = pBuf;
	while (1)
	{
		if ((psz - pBuf) >= (iBufLen-1))
		{
			printf("%d tcp recv head overflow.\r\n", sock);
			return -2;
		}

		rv = XOS_TCPRecv(sock, psz, 1, iTimeOut);
		if (rv == -1)
		{
			printf("%d tcp recv single char failed.\r\n", sock);
			return -1;
		}

		psz ++;
		psz[0] = 0;
		
		if (strstr(pBuf, pszTail) != NULL)
		{
			return 0;
		}
	}
}

int XOS_TCPRecvHeadNB(XOS_SOCKET sock, char *pBuf, int iBufLen, int *piRecvLen, XOS_PSZ pszTail)
{
	int rv;
	int i;
	XOS_BOOL bRecved = XOS_FALSE;

	while (1)
	{
		if ((*piRecvLen) >= (iBufLen-1))
		{
			printf("%d tcp recv head nonblock overflow.\r\n", sock);
			return -2;
		}

		rv = XOS_TCPRecvNB(sock, pBuf+(*piRecvLen), 1);
		if (rv == -1)
		{
			printf("%d tcp recv single char nonblock failed.\r\n", sock);
			return -1;
		}
		else if (rv == 0)
		{
			if (bRecved)
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}

		bRecved = XOS_TRUE;
		(*piRecvLen) ++;
		pBuf[*piRecvLen] = 0;
		
		if ((*piRecvLen) < (int)strlen(pszTail))//接收到的数据还没有指定的字符串长度长
		{
			continue;
		}
		else
		{   
			i = strlen(pszTail);
			if (memcmp(&(pBuf[*piRecvLen - i]), pszTail, i) == 0)
			{ 
				(*piRecvLen) = 0;
				return 0;
			}
		}
		
//		if (strstr(pBuf, pszTail) != NULL)
//		{
//			//接收完了,把这个置成0,防止上层忘记赋0
//			(*piRecvLen) = 0;
//			return 0;
//		}
	}
}

//////////////////////////////////////////////////////////////////////////

XOS_SOCKET XOS_TCPListen(XOS_U16 u16Port, int iListenNum)
{
	int rv;
	XOS_SOCKET sock;	
	struct sockaddr_in local;

	printf("tcp listen: %d(%d).\r\n", u16Port, iListenNum);

	//创建socket
	sock = XOS_TCPSocket();
	if (sock == XOS_INVALID_SOCKET)
	{
		return XOS_INVALID_SOCKET;
	}
	printf("%d tcp listen %d(%d) sock ok.\r\n", sock, u16Port, iListenNum);
	
	//绑定到一个特定的地址端口上.
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(u16Port);
	rv = bind(sock, (struct sockaddr *)&local, sizeof(local));
	if (rv == SOCKET_ERROR)
	{
		printf("%d tcp bind failed: %d.\r\n", sock, lastSockErrno);
		XOS_CloseSocket(sock);
		return XOS_INVALID_SOCKET;
	}
	
	//侦听
	rv = listen(sock, iListenNum);
	if (rv == SOCKET_ERROR)
	{
		printf("%d tcp listen failed: %d.\r\n", sock, lastSockErrno);
		XOS_CloseSocket(sock);
		return XOS_INVALID_SOCKET;
	}
	printf("%d tcp listen ok %d(%d).\r\n", sock, u16Port, iListenNum);
	
	return sock;
}

XOS_SOCKET XOS_TCPAccept(XOS_SOCKET sock)
{
	XOS_SOCKET sockReq;
	struct sockaddr_in from;
	int fromlen = sizeof(from);

	//xos_setsockblock(sock);
	sockReq = accept(sock, (struct sockaddr*)&from, &fromlen);
	if (sockReq == INVALID_SOCKET)
	{
		printf("%d tcp accept failed: %d.\r\n", sock, lastSockErrno);
		return XOS_INVALID_SOCKET;
	}
	else
	{
		printf("%d tcp accept %d from %s:%d ok.\r\n", sock, sockReq, inet_ntoa(from.sin_addr), htons(from.sin_port));		
		//设置Socket选项
		xos_setsockblock(sockReq);
		xos_setsockrecvbuf(sockReq, 8*1024);
		xos_setsocksendbuf(sockReq, 8*1024);
		xos_setsocknodelay(sockReq);
		xos_setsockkeepalive(sockReq);
		xos_setsocklinger(sockReq, 1, 0);
		return sockReq;
	}
}

XOS_SOCKET XOS_TCPAcceptNB(XOS_SOCKET sock)
{
	int rv;
	XOS_SOCKET sockReq;
	struct sockaddr_in from;
	int fromlen = sizeof(from);

	//xos_setsocknonblock(sock);
	sockReq = accept(sock, (struct sockaddr*)&from, &fromlen);
	if (sockReq == INVALID_SOCKET)
	{
		rv = lastSockErrno;
		if ((rv == EWOULDBLOCK) || (rv == EINPROGRESS))
		{
			return XOS_INVALID_SOCKET;
		}
		else if (rv == ENOBUFS)
		{
			printf("%d tcp accept nonblock no buffer.\r\n", sock);
			return XOS_INVALID_SOCKET;
		}
		else
		{
			printf("%d tcp accept nonblock failed: %d.\r\n", sock, rv);
			return XOS_INVALID_SOCKET;
		}
	}
	else
	{
		printf("%d tcp accept %d nonblock from %s:%d ok.\r\n", sock, sockReq, inet_ntoa(from.sin_addr), htons(from.sin_port));		
		//设置Socket选项
		xos_setsocknonblock(sockReq);
		xos_setsockrecvbuf(sockReq, 8*1024);
		xos_setsocksendbuf(sockReq, 8*1024);
		xos_setsocknodelay(sockReq);
		xos_setsockkeepalive(sockReq);
		xos_setsocklinger(sockReq, 1, 0);
		return sockReq;
	}
}

XOS_BOOL XOS_TCPGetPeerAddr(XOS_SOCKET sock, XOS_U32 *pu32IP, XOS_U16 *pu16Port)
{
	struct sockaddr_in from;
	int iformlen;
    
	iformlen = sizeof(struct sockaddr);
	if (getpeername(sock, (struct sockaddr *)&from, &iformlen) == 0)
	{
		if (pu32IP != NULL)
		{
			*pu32IP = from.sin_addr.s_addr;
		}
		if (pu16Port != NULL)
		{
			*pu16Port = htons(from.sin_port);
		}
		return XOS_TRUE;
	}
	else
	{
		return XOS_FALSE;
	}
}

XOS_BOOL XOS_TCPGetLocalAddr(XOS_SOCKET sock, XOS_U32 *pu32IP, XOS_U16 *pu16Port)
{
	struct sockaddr_in addr;
	int iaddrlen;
    
	iaddrlen = sizeof(struct sockaddr);
	if (getsockname(sock, (struct sockaddr *)&addr, &iaddrlen) == 0)
	{
		if (pu32IP != NULL)
		{
			*pu32IP = addr.sin_addr.s_addr;
		}
		if (pu16Port != NULL)
		{
			*pu16Port = htons(addr.sin_port);
		}
		return XOS_TRUE;
	}
	else
	{
		return XOS_FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////

XOS_SOCKET XOS_UDPBind(XOS_U32 u32IP, XOS_U16 u16Port)
{
	int rv;
	XOS_SOCKET sock;
	struct sockaddr_in local;

	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	if (u32IP == 0)
	{
		local.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		local.sin_addr.s_addr = u32IP;
	}
    local.sin_port = htons(u16Port);

	printf("udp bind: %s:%d.\r\n", inet_ntoa(local.sin_addr), u16Port);
	
	sock = XOS_UDPSocket();
	if (sock == XOS_INVALID_SOCKET)
	{
		return XOS_INVALID_SOCKET;
	}
	printf("%d udp bind %s:%d sock ok.\r\n", sock, inet_ntoa(local.sin_addr), u16Port);
	
    rv = bind(sock, (struct sockaddr*)&local, sizeof(local));
	if (rv == SOCKET_ERROR)
    {
    	printf("%d udp bind %s:%d failed: %d.\r\n", sock, inet_ntoa(local.sin_addr), u16Port, lastSockErrno);
    	XOS_CloseSocket(sock);
    	return XOS_INVALID_SOCKET;
    }
	printf("%d udp bind %s:%d ok.\r\n", sock, inet_ntoa(local.sin_addr), u16Port);

	return sock;
}

XOS_BOOL XOS_UDPSendTo(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 u32IP, XOS_U16 u16Port)
{
	int rv;
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(u16Port);
	addr.sin_addr.s_addr = u32IP;
	
	//xos_setsockblock(sock);
	rv = sendto(sock, pBuf, iBufLen, 0, (struct sockaddr *)&addr, sizeof(addr));
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		if (rv == ENOBUFS)
		{
			//阻塞型的,不应该打印这个的
			//后来发现也会打印这个
			//printf("%d udp sendto %s:%d no buffer.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port));
			return XOS_FALSE;
		}
		else
		{
			//一般不应该出现其他错误,路由不可达的时候会打印这个
			if (rv != ENETUNREACH)
			{
				//ENETUNREACH出现比较多,不打印了,只打印其他错误
				printf("%d udp sendto %s:%d failed: %d.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port), rv);
			}
			return XOS_FALSE;
		}
	}
	else
	{
		return XOS_TRUE;
	}
}

int XOS_UDPSendToNB(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 u32IP, XOS_U16 u16Port)
{
	int rv;
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(u16Port);
	addr.sin_addr.s_addr = u32IP;
	
	//xos_setsocknonblock(sock);
	rv = sendto(sock, pBuf, iBufLen, 0, (struct sockaddr *)&addr, sizeof(addr));
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		if ((rv == EWOULDBLOCK))
		{
			return 1;
		}
		else if (rv == ENOBUFS)
		{
			//这个地方出现比较正常,不打印了,路由不可达的时候会打印这个
			//printf("%d udp sendto %s:%d nonblock no buffer,IP:%s port:%d.\r\n", sock,
			//		inet_ntoa(addr.sin_addr), htons(addr.sin_port));
			return 1;
		}
		else if (rv == ENETUNREACH)
		{
			//ENETUNREACH出现比较多,不打印了
			return 1;
		}
		else
		{
			//其他错误就比较异常了,打印下
			printf("%d udp sendto %s:%d nonblock failed: %d.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port), rv);
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

int XOS_UDPRecvFrom(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 *pu32IP, XOS_U16 *pu16Port)
{
	int rv;
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);

	//xos_setsockblock(sock);
	rv = recvfrom(sock, pBuf, iBufLen, 0, (struct sockaddr *)&addr, &addrlen);
	if (rv >= 0)
	{
		if (pu32IP != NULL)
		{
			*pu32IP = addr.sin_addr.s_addr;
		}
		if (pu16Port != NULL)
		{
			*pu16Port = ntohs(addr.sin_port);
		}
		return rv;
	}
	else
	{
		rv = lastSockErrno;
		if (rv == EMSGSIZE)
		{
			//数据太长
			printf("%d udp recvfrom %s:%d package is too large.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port));
			return -1;
		}
		else
		{
			//出错的原因很多:ICMP的端口不可达、参数不对
			printf("%d udp recvfrom %s:%d failed: %d.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port), rv);
			return -1; 
		}
	}
}

int XOS_UDPRecvFromNB(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 *pu32IP, XOS_U16 *pu16Port)
{
	int rv;
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);

	//xos_setsocknonblock(sock);
	rv = recvfrom(sock, pBuf, iBufLen, 0, (struct sockaddr *)&addr, &addrlen);
	if (rv >= 0)
	{
		if (pu32IP != NULL)
		{
			*pu32IP = addr.sin_addr.s_addr;
		}
		if (pu16Port != NULL)
		{
			*pu16Port = ntohs(addr.sin_port);
		}
		return rv;
	}
	else
	{
		rv = lastSockErrno;
		if (rv == EWOULDBLOCK)
		{
			//没数据
			return -1;
		}
		else if (rv == EMSGSIZE)
		{
			//数据太长
			printf("%d udp recvfrom %s:%d nonblock package is too large.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port));
			return -1;
		}
		else
		{
			//出错的原因很多:ICMP的端口不可达、参数不对
			printf("%d udp recvfrom %s:%d nonblock failed: %d.\r\n", sock, inet_ntoa(addr.sin_addr), htons(addr.sin_port), rv);
			return -1;
		}
	}
}
