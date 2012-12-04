/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: Proxy.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-02-28 17:11:47
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../Proxy.h"
#include "../DNS.h"
#include "../Base64.h"

static void Construct(TProxy *pProxy)
{
	pProxy->m_status = PROXY_STATUS_NULL;
	pProxy->m_sock = XOS_INVALID_SOCKET;
	pProxy->m_sock_bak = XOS_INVALID_SOCKET;
	pProxy->m_iProxyType = PROXY_TYPE_NULL;
	memset(pProxy->m_szDestAddr, 0, MAX_PROXY_DESTADDR_LEN);
	memset(pProxy->m_szProxyAddr, 0, MAX_PROXY_ADDR_LEN);
	memset(pProxy->m_szUserName, 0, MAX_PROXY_USERNAME_LEN);
	memset(pProxy->m_szPassword, 0, MAX_PROXY_PASSWORD_LEN);
	memset(pProxy->m_tmpbuf, 0, 256);
	memset(pProxy->m_sendbuf, 0, 1024);
	memset(pProxy->m_recvbuf, 0, 1024);
}

static void Destroy(TProxy *pProxy)
{
	if (pProxy->m_sock != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(pProxy->m_sock);
	}
}

static XOS_BOOL Create(TProxy *pProxy, XOS_PSZ pszProxyType, XOS_PSZ pszDestAddr,
					   XOS_PSZ pszProxyAddr, XOS_PSZ pszUserName, XOS_PSZ pszPassword)
{
	XOS_U32 uiProxyIP;
	XOS_U16 usProxyPort;

	if ((pszProxyType == NULL) ||
		(pszDestAddr == NULL) ||
		(pszProxyAddr == NULL) ||
		(pszUserName == NULL) ||
		(pszPassword == NULL) ||
		(strlen(pszDestAddr) >= MAX_PROXY_DESTADDR_LEN) ||
		(strlen(pszProxyAddr) >= MAX_PROXY_ADDR_LEN) ||
		(strlen(pszUserName) >= MAX_PROXY_USERNAME_LEN) ||
		(strlen(pszPassword) >= MAX_PROXY_PASSWORD_LEN))
	{
		return XOS_FALSE;
	}

	if (strcmp(pszProxyType, "HTTP") == 0)
	{
		pProxy->m_iProxyType = PROXY_TYPE_HTTP;
	}
	else if (strcmp(pszProxyType, "SOCKET4") == 0)
	{
		pProxy->m_iProxyType = PROXY_TYPE_SOCKET4;
	}
	else if (strcmp(pszProxyType, "SOCKET5") == 0)
	{
		pProxy->m_iProxyType = PROXY_TYPE_SOCKET5;
	}
	else
	{
		return XOS_FALSE;
	}

	strcpy(pProxy->m_szDestAddr, pszDestAddr);
	strcpy(pProxy->m_szProxyAddr, pszProxyAddr);
	strcpy(pProxy->m_szUserName, pszUserName);
	strcpy(pProxy->m_szPassword, pszPassword);

	if (!Addr2IPAndPort(pProxy->m_szProxyAddr, &uiProxyIP, &usProxyPort, XOS_FALSE))
	{
		return XOS_FALSE;
	}
	if (pProxy->m_iProxyType != PROXY_TYPE_HTTP)
	{
		if (!Addr2IPAndPort(pProxy->m_szDestAddr, &pProxy->m_uiDestIP, &pProxy->m_usDestPort, XOS_FALSE))
		{
			return XOS_FALSE;
		}
	}

	pProxy->m_sock = XOS_TCPConnectNB(uiProxyIP, usProxyPort);
	if (pProxy->m_sock == XOS_INVALID_SOCKET)
	{
		return XOS_FALSE;
	}
	pProxy->m_sock_bak = pProxy->m_sock;
	pProxy->m_status = PROXY_STATUS_CONNECTING;

	return XOS_TRUE;
}

static int Probe(TProxy *pProxy)
{
	if (pProxy->m_status == PROXY_STATUS_CONNECTING)
	{
		int rv;
		rv = XOS_TCPConnectProbe(pProxy->m_sock);
		if (rv == -1)
		{
			printf("proxy probe select failed.\r\n");
			return PROXY_PROBE_ERROR;
		}
		else if (rv == 0)
		{
			//连接成功,根据不同的代理类型进入不同的状态
			pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
			switch (pProxy->m_iProxyType)
			{
			case PROXY_TYPE_HTTP:
					sprintf(pProxy->m_sendbuf, "%s:%s", pProxy->m_szUserName, pProxy->m_szPassword);
					Base64_Encode(pProxy->m_tmpbuf, pProxy->m_sendbuf, strlen(pProxy->m_sendbuf));
					sprintf(pProxy->m_sendbuf, "CONNECT %s HTTP/1.0\r\nProxy-Connection: Keep-Alive\r\nProxy-Authorization: Basic %s\r\nPragma: no-cache\r\n\r\n",
						pProxy->m_szDestAddr, pProxy->m_tmpbuf);
					pProxy->m_ihaveSendLen = 0;
					pProxy->m_isendbufLen = strlen(pProxy->m_sendbuf);
					pProxy->m_status = PROXY_STATUS_CONNECTION_REQUEST;
					break;
				case PROXY_TYPE_SOCKET4:
					pProxy->m_sendbuf[0] = 0x04;
					pProxy->m_sendbuf[1] = 0x01;
					*(XOS_U16*)(&pProxy->m_sendbuf[2]) = htons(pProxy->m_usDestPort);
					*(XOS_U32*)(&pProxy->m_sendbuf[4]) = pProxy->m_uiDestIP;
					strcat(&pProxy->m_sendbuf[8], pProxy->m_szUserName);
					pProxy->m_isendbufLen = (int)(8+strlen(pProxy->m_szUserName)+1);
					//??????这个地方的发送假设可以一次发完
					pProxy->m_status = PROXY_STATUS_CONNECTION_REQUEST;
					break;
				case PROXY_TYPE_SOCKET5:
					if(strcmp(pProxy->m_szUserName, "") != 0)
					{
						memcpy(pProxy->m_sendbuf, "\005\002\002\000", 4);
					}
					else
					{
						memcpy(pProxy->m_sendbuf, "\005\001\000", 3);
					}
					pProxy->m_ihaveSendLen = 0;
					pProxy->m_isendbufLen = strlen(pProxy->m_sendbuf);
					pProxy->m_status = PROXY_STATUS_NEGOTIATION_REQUEST;
					break;
				default:
					return PROXY_PROBE_ERROR;
			}
		}
		else //rv == 1
		{
			return PROXY_PROBE_WOULDBLOCK;
		}
	}
	
	switch (pProxy->m_iProxyType)
	{
		case PROXY_TYPE_HTTP:
			return pProxy->ProbeHttp(pProxy);
		case PROXY_TYPE_SOCKET4:
			return pProxy->ProbeSocket4(pProxy);
		case PROXY_TYPE_SOCKET5:
			return pProxy->ProbeSocket5(pProxy);
		default:
			return PROXY_PROBE_ERROR;
	}
}

static XOS_SOCKET GetSocket(TProxy *pProxy)
{
	return pProxy->m_sock_bak;
}

static int ProbeHttp(TProxy *pProxy)
{
	int rv;

	if ((XOS_GetTickCount() - pProxy->m_uiOperationBeginTime) > MAX_PROXY_TIMEOUT)
	{
		return PROXY_PROBE_TIMEOUT;
	}
	
	switch (pProxy->m_status)
	{
		case PROXY_STATUS_CONNECTION_REQUEST:
			rv = XOS_TCPSendDataNB(pProxy->m_sock, pProxy->m_sendbuf, &(pProxy->m_ihaveSendLen), pProxy->m_isendbufLen);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if(rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else if (rv == 2) 
			{
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();				
				return PROXY_PROBE_WOULDBLOCK;
			}
			else
			{
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
				pProxy->m_irecvLen = 0;
				memset(pProxy->m_recvbuf, 0, 1024);
				pProxy->m_status = PROXY_STATUS_CONNECTION_RESPONSE;
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_CONNECTION_RESPONSE:
			//??????这个地方需要改成按照协议做,接收完响应报文
			rv = XOS_TCPRecvDataNB(pProxy->m_sock, pProxy->m_recvbuf, &(pProxy->m_irecvLen), 1024);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if (rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else //0 2
			{
				if ((strstr(pProxy->m_recvbuf, "200") != NULL) ||
					(strstr(pProxy->m_recvbuf, "Connection established") != NULL))
				{
					pProxy->m_status = PROXY_STATUS_CONNECTION_FINISHED;
					pProxy->m_sock = XOS_INVALID_SOCKET;
					return PROXY_PROBE_OK;
				}
				else if (pProxy->m_irecvLen >= 1023)
				{
					pProxy->m_status = PROXY_STATUS_FAILED;
					return PROXY_PROBE_ERROR;
				}
			}
		case PROXY_STATUS_CONNECTION_FINISHED:
			return PROXY_PROBE_OK;
		default:
			return PROXY_PROBE_ERROR;
	}
}

static int ProbeSocket4(TProxy *pProxy)
{
	int rv;

	if ((XOS_GetTickCount() - pProxy->m_uiOperationBeginTime) > MAX_PROXY_TIMEOUT)
	{
		return PROXY_PROBE_TIMEOUT;
	}
	
	switch (pProxy->m_status)
	{
		case PROXY_STATUS_CONNECTION_REQUEST:
			rv = XOS_TCPSendDataNB(pProxy->m_sock, pProxy->m_sendbuf, &(pProxy->m_ihaveSendLen), pProxy->m_isendbufLen);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if(rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else if (rv == 2) 
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else // rv == 0
			{
				pProxy->m_irecvLen = 0;
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
				pProxy->m_status = PROXY_STATUS_CONNECTION_RESPONSE;
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_CONNECTION_RESPONSE:
			rv = XOS_TCPRecvDataNB(pProxy->m_sock, pProxy->m_recvbuf, &(pProxy->m_irecvLen), 1024);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if (rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else //0 2
			{
				if (pProxy->m_irecvLen < 2)
				{
					return PROXY_PROBE_WOULDBLOCK;
				}
				if ((pProxy->m_recvbuf[1] == (char)0x90) || (pProxy->m_recvbuf[1] == 90))
				{
					pProxy->m_status = PROXY_STATUS_CONNECTION_FINISHED;
					pProxy->m_sock = XOS_INVALID_SOCKET;
					return PROXY_PROBE_OK;
				}
				else
				{
					pProxy->m_status = PROXY_STATUS_FAILED;
					return PROXY_PROBE_ERROR;
				}
			}
		case PROXY_STATUS_CONNECTION_FINISHED:
			return PROXY_PROBE_OK;
		default:
			return PROXY_PROBE_ERROR;
	}
}

static int ProbeSocket5(TProxy *pProxy)
{
	int rv;

	if ((XOS_GetTickCount() - pProxy->m_uiOperationBeginTime) > MAX_PROXY_TIMEOUT)
	{
		return PROXY_PROBE_TIMEOUT;
	}
	
	switch (pProxy->m_status)
	{
		case PROXY_STATUS_NEGOTIATION_REQUEST:
			rv = XOS_TCPSendDataNB(pProxy->m_sock, pProxy->m_sendbuf, &(pProxy->m_ihaveSendLen), pProxy->m_isendbufLen);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if(rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else if (rv == 2) 
			{
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();				
				return PROXY_PROBE_WOULDBLOCK;
			}
			else
			{
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
				pProxy->m_status = PROXY_STATUS_NEGOTIATION_RESPONSE;
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
				memset(pProxy->m_recvbuf, 0, 1024);
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_NEGOTIATION_RESPONSE:
			//??????这个地方需要改成按照协议做,接收完响应报文
			rv = XOS_TCPRecvDataNB(pProxy->m_sock, pProxy->m_recvbuf, &(pProxy->m_irecvLen), 1024);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if (rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else //0 2
			{
				if (pProxy->m_irecvLen < 2)
				{
					return PROXY_PROBE_WOULDBLOCK;
				}
				if((pProxy->m_recvbuf[0] == '\005') && (pProxy->m_recvbuf[1] == '\002'))
				{
					pProxy->m_sendbuf[0] = 1;
					pProxy->m_sendbuf[1] = strlen(pProxy->m_szUserName);
					memcpy(&pProxy->m_sendbuf[2], pProxy->m_szUserName, strlen(pProxy->m_szUserName));
					pProxy->m_sendbuf[2+strlen(pProxy->m_szUserName)] = strlen(pProxy->m_szPassword);
					memcpy(&pProxy->m_sendbuf[2+strlen(pProxy->m_szUserName)+1], pProxy->m_szPassword, strlen(pProxy->m_szPassword));
					pProxy->m_isendbufLen = 2+strlen(pProxy->m_szUserName)+1+strlen(pProxy->m_szPassword);					
					pProxy->m_ihaveSendLen = 0;
					pProxy->m_status = PROXY_STATUS_SUBNEGOTIATION_REQUEST;
				}
				else if((pProxy->m_recvbuf[0] == '\005') && (pProxy->m_recvbuf[1] == '\000'))
				{
					memcpy(pProxy->m_sendbuf, "\005\001\000\001\000\000\000\000\000\000", 10);
					*(XOS_U32*)(&pProxy->m_sendbuf[4]) = pProxy->m_uiDestIP;
					*(XOS_U16*)(&pProxy->m_sendbuf[8]) = htons(pProxy->m_usDestPort);
					pProxy->m_ihaveSendLen = 0;
					pProxy->m_isendbufLen = 10;
					pProxy->m_status = PROXY_STATUS_CONNECTION_REQUEST;
				}
				else
				{
					return PROXY_PROBE_ERROR;
				}
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_SUBNEGOTIATION_REQUEST:
			rv = XOS_TCPSendDataNB(pProxy->m_sock, pProxy->m_sendbuf, &(pProxy->m_ihaveSendLen), pProxy->m_isendbufLen);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if(rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else if (rv == 2) 
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else // rv == 0
			{
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
				pProxy->m_status = PROXY_STATUS_SUBNEGOTIATION_RESPONSE;
				memset(pProxy->m_recvbuf, 0 ,1024);
				pProxy->m_irecvLen = 0;
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_SUBNEGOTIATION_RESPONSE:
			//??????这个地方需要改成按照协议做,接收完响应报文
			rv = XOS_TCPRecvDataNB(pProxy->m_sock, pProxy->m_recvbuf, &(pProxy->m_irecvLen), 1024);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if (rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else //0 2
			{
				if (pProxy->m_irecvLen < 2)
				{
					return PROXY_PROBE_WOULDBLOCK;
				}
				if((pProxy->m_recvbuf[0] == '\001') && (pProxy->m_recvbuf[1] == '\000'))
				{
					pProxy->m_status = PROXY_STATUS_CONNECTION_REQUEST;
				}
				else
				{
					return PROXY_PROBE_ERROR;
				}
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_CONNECTION_REQUEST:
			rv = XOS_TCPSendDataNB(pProxy->m_sock, pProxy->m_sendbuf, &(pProxy->m_ihaveSendLen), pProxy->m_isendbufLen);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if(rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else if (rv == 2) 
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else // rv == 0
			{
				pProxy->m_uiOperationBeginTime = XOS_GetTickCount();
				pProxy->m_status = PROXY_STATUS_CONNECTION_RESPONSE;
				memset(pProxy->m_recvbuf, 0 ,1024);
				pProxy->m_irecvLen = 0;
				return PROXY_PROBE_WOULDBLOCK;
			}
		case PROXY_STATUS_CONNECTION_RESPONSE:
			//??????这个地方需要改成按照协议做,接收完响应报文
			rv = XOS_TCPRecvDataNB(pProxy->m_sock, pProxy->m_recvbuf, &(pProxy->m_irecvLen), 1024);
			if (rv == -1)
			{
				return PROXY_PROBE_ERROR;
			}
			else if (rv == 1)
			{
				return PROXY_PROBE_WOULDBLOCK;
			}
			else //0 2
			{
				if (pProxy->m_irecvLen < 2)
				{
					return PROXY_PROBE_WOULDBLOCK;
				}	
				if((pProxy->m_recvbuf[0] == '\005') && (pProxy->m_recvbuf[1] == '\000'))
				{
					pProxy->m_status = PROXY_STATUS_CONNECTION_FINISHED;
					pProxy->m_sock = XOS_INVALID_SOCKET;
					return PROXY_PROBE_OK;
				}
				else
				{
					pProxy->m_status = PROXY_STATUS_FAILED;
					return PROXY_PROBE_ERROR;
				}
			}
		case PROXY_STATUS_CONNECTION_FINISHED:
			return PROXY_PROBE_OK;
		default:
			return PROXY_PROBE_ERROR;
	}
}

const TProxy PROXY =
{
	&Construct,
	&Destroy,
	&Create,

	&Probe,
	&GetSocket,

	&ProbeHttp,
	&ProbeSocket4,
	&ProbeSocket5
};
