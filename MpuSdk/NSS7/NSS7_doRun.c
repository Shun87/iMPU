/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: NSS7.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xiangmm
 *  完成日期: 2010-3-24 16:53:00
 *  修正备注: 
 *  
 */

#include "NSS7_Defs.h"

static int SendData(TNSS7 *pNSS7, char *pbinBuf);
static int RecvData(TNSS7 *pNSS7, char *pbinBuf);
static int Connect(TNSS7 *pNSS7);
static int ConnectProbe(TNSS7 *pNSS7);
static void BuildPost(TNSS7 *pNSS7, XOS_PSZ pszNTLMAuth);
static int SendPost(TNSS7 *pNSS7);
static int RecvPostHead(TNSS7 *pNSS7);
static int RecvPostBody(TNSS7 *pNSS7);
static int RecvPostContent(TNSS7 *pNSS7);
static int SendHash(TNSS7 *pNSS7);
static int RecvCertify(TNSS7 *pNSS7);
static int RecvParamLen(TNSS7 *pNSS7);
static int RecvParam(TNSS7 *pNSS7);
static int RecvCommand(TNSS7 *pNSS7);
static int SendCommand(TNSS7 *pNSS7);
static void BuildKeepAlive(TNSS7 *pNSS7);

//////////////////////////////////////////////////////////////////////////

int NSS7_doRun(TNSS7 *pNSS7)
{
	int rv;

	//判断连接释放超时
	if ((XOS_GetTickCount() - pNSS7->m_u32LastRecvTime) > pNSS7->uiSessionTimeOut)
	{
		printf("NSS7 timeout, Timeoutinterval=%dms.\r\n", pNSS7->uiSessionTimeOut);
		return NSS7_E_TIMEOUT;
	}

	//判断是否要发保活包
	if (pNSS7->m_status == NSS7_STATUS_REGISTEROK)
	{
		if ((XOS_GetTickCount() - pNSS7->m_u32LastSendTime) > pNSS7->m_u32KAInterval)
		{
			//长时间没有发数据了,则构造一个保活包
			BuildKeepAlive(pNSS7);
		}
	}

	switch(pNSS7->m_status) 
	{
		case NSS7_STATUS_CONNECT:
			rv = Connect(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_PROBE;
			//提高命令连接的通信优先级, 以防被数据通道占用太多带宽之后命令通道超时断掉.
#ifndef WIN32
			{
				//为了避免数据发送的时候,阻碍命令连接通信,提高了命令通道socket的优先级.
//				int iPriority = 6;
//				int iOptLen = sizeof(iPriority);
//				setsockopt(pNSS7->m_sock, SOL_SOCKET, SO_PRIORITY, &iPriority, iOptLen);
//
//				iPriority = -1;
//				getsockopt(pNSS7->m_sock, SOL_SOCKET, SO_PRIORITY, &iPriority, &iOptLen);
//				printf("getsockopt priority:%d.\r\n", iPriority);
			}
#endif
			
		case NSS7_STATUS_PROBE:
			rv = ConnectProbe(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_SENDPOST;

		case NSS7_STATUS_SENDPOST:
			rv = SendPost(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_RECVPOSTHEAD;

		case NSS7_STATUS_RECVPOSTHEAD:
			rv = RecvPostHead(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_RECVPOSTBODY;

		case NSS7_STATUS_RECVPOSTBODY:
			rv = RecvPostBody(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			if (pNSS7->m_bReConnect == XOS_FALSE)
			{
				rv = NSS7_E_WOULDBLOCK;
				pNSS7->m_status = NSS7_STATUS_SENDHASH;
				break;
			}
			pNSS7->m_bReConnect = XOS_FALSE;
			pNSS7->m_status = NSS7_STATUS_RECVCONTENT;
			
		case NSS7_STATUS_RECVCONTENT:
			rv = RecvPostContent(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			rv = NSS7_E_WOULDBLOCK;
			pNSS7->m_status = NSS7_STATUS_CONNECT;
			break;

		case NSS7_STATUS_SENDHASH:
			rv = SendHash(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_RECVCERTIFY;
			
		case NSS7_STATUS_RECVCERTIFY:
			rv = RecvCertify(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_RECVPARAMLEN;

		case NSS7_STATUS_RECVPARAMLEN:
			rv = RecvParamLen(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			pNSS7->m_status = NSS7_STATUS_RECVPARAM;
			
		case NSS7_STATUS_RECVPARAM:
			rv = RecvParam(pNSS7);
			if (rv != NSS7_E_OK)
			{
				break;
			}
			XOS_TCPGetPeerAddr(pNSS7->m_sock, &pNSS7->m_uiPeerIP, &pNSS7->m_usPeerPort);
			pNSS7->m_status = NSS7_STATUS_REGISTEROK;

		case NSS7_STATUS_REGISTEROK:
			rv = RecvCommand(pNSS7);
			if (rv < 0)
			{
				return rv;
			}
			rv = SendCommand(pNSS7);
			break;

		default:
			assert(0);
			rv = NSS7_E_UNKNOW;
	}

	return rv;
}

static int SendData(TNSS7 *pNSS7, char *pbinBuf)
{
	int rv;
	pNSS7->m_u32LastSendTime = XOS_GetTickCount();
	rv = XOS_TCPSendDataNB(pNSS7->m_sock, pbinBuf, &pNSS7->m_u32HaveSendLen, pNSS7->m_u32SendLen);
	if (rv == 0)
	{
		xlprintf("[%s] NSS7 SendData ok: %d.\r\n", "nss7_rx", pNSS7->m_u32SendLen);
		return NSS7_E_OK;
	}
	else if (rv == -1)
	{
		return NSS7_E_TCPSEND;
	}
	else if (rv == 1)
	{
		return NSS7_E_WOULDBLOCK;
	}
	else
	{
		return NSS7_E_WOULDBLOCK;
	}
}

static int RecvData(TNSS7 *pNSS7, char *pbinBuf)
{
	int rv;
	rv = XOS_TCPRecvDataNB(pNSS7->m_sock, pbinBuf, &pNSS7->m_u32HaveRecvLen, pNSS7->m_u32RecvLen);
	if (rv == 0)
	{
		xlprintf("[%s] NSS7 RecvData ok: %d.\r\n", "nss7_rx", pNSS7->m_u32RecvLen);
		pNSS7->m_u32LastRecvTime = XOS_GetTickCount();
		return NSS7_E_OK;
	}
	else if (rv == -1)
	{
		return NSS7_E_TCPRECV;
	}
	else if (rv == 1)
	{
		return NSS7_E_WOULDBLOCK;
	}
	else
	{
		pNSS7->m_u32LastRecvTime = XOS_GetTickCount();
		return NSS7_E_WOULDBLOCK;
	}
}

static int Connect(TNSS7 *pNSS7)
{
	if (pNSS7->m_bProxy)
	{
		char szDestAddr[MAX_PROXY_DESTADDR_LEN];
		char szProxyAddr[MAX_PROXY_DESTADDR_LEN];

		assert(pNSS7->m_pProxy == NULL);
		pNSS7->m_pProxy = cnew(sizeof(TProxy), &PROXY);
		if (pNSS7->m_pProxy == NULL)
		{
			printf("NSS7 Connect cnew failed.\r\n");
			return NSS7_E_MALLOC;
		}

		//对传入的长度进行了限制,不会越界的
		sprintf(szDestAddr, "%s:%d", pNSS7->m_szPUIAddr, pNSS7->m_u16PUIPort);
		sprintf(szProxyAddr, "%s:%d", pNSS7->m_szProxyAddr, pNSS7->m_usProxyPort);
		if (!pNSS7->m_pProxy->Create(pNSS7->m_pProxy, pNSS7->m_szProxyType, szDestAddr, szProxyAddr, 
									 pNSS7->m_szProxyUserName, pNSS7->m_szProxyPassword))
		{
			cdelete(pNSS7->m_pProxy);
			pNSS7->m_pProxy = NULL;
			printf("NSS7 Connect m_pProxy Create failed.\r\n");
			return NSS7_E_MALLOC;
		}

		printf("NSS7 Connect by proxy ok.\r\n");
		return NSS7_E_OK;
	}
	else
	{
		XOS_U32 u32CUIIP;
		u32CUIIP = XOS_AddrToU32(pNSS7->m_szPUIAddr);
		if ((u32CUIIP == 0) || (u32CUIIP == 0xFFFFFFFF))
		{
			u32CUIIP = DNS2IP(pNSS7->m_szPUIAddr);
		}
		if ((u32CUIIP == 0) || (u32CUIIP == 0xFFFFFFFF))
		{
			//还没有解释成功
			return NSS7_E_WOULDBLOCK;
		}

		pNSS7->m_sock = XOS_TCPConnectNB(u32CUIIP, pNSS7->m_u16PUIPort);
		if (pNSS7->m_sock == XOS_INVALID_SOCKET)
		{
			printf("NSS7 Connect XOS_TCPConnectNB failed.\r\n");
			return NSS7_E_UNREACHABLE;
		}

		printf("NSS7 Connect ok.\r\n");
		return NSS7_E_OK;
	}
}

static int ConnectProbe(TNSS7 *pNSS7)
{
	int rv;
	XOS_PSZ pszNTLMAuth = NULL;
	if (pNSS7->m_pProxy)
	{
		rv = pNSS7->m_pProxy->Probe(pNSS7->m_pProxy);
		if (rv == PROXY_PROBE_WOULDBLOCK)
		{
			return NSS7_E_WOULDBLOCK;
		}
		else if (rv != PROXY_PROBE_OK)
		{
			//代理超时或出错
			printf("NSS7 ConnectProbe by proxy fialed: %d.\r\n", rv);
			cdelete(pNSS7->m_pProxy);
			pNSS7->m_pProxy = NULL;
			return NSS7_E_PROXYCONNECT;
		}
		else
		{
			printf("NSS7 ConnectProbe by proxy ok.\r\n");
		}
	}
	else
	{
		rv = XOS_TCPConnectProbeWithErrNo(pNSS7->m_sock);
		if (rv == 1)
		{
			return NSS7_E_WOULDBLOCK;
		}
		else if (rv < 0)
		{	
			if (rv == XOS_SOCKETERR_NOROUTE)
			{
				rv = NSS7_E_NOROUTE;
			}
			else if (rv == XOS_SOCKETERR_NOSERVER)
			{
				rv = NSS7_E_NOSERVER;
			}
			else
			{
				rv = NSS7_E_UNREACHABLE;
			}
			printf("NSS7 ConnectProbe fialed: %d.\r\n", rv);
			XOS_CloseSocket(pNSS7->m_sock);
			pNSS7->m_sock = XOS_INVALID_SOCKET;
			return rv;
		}
		else
		{
			printf("NSS7 ConnectProbe ok.\r\n");
		}
	}
	
	if ((pNSS7->m_bProxy) &&
		(strcmp(pNSS7->m_szProxyType, "NTLM") == 0))
	{
		//pszNTLMAuth = pNSS7->m_pProxy->GetNTLMAuth(pNSS7->m_pProxy);
	}
	BuildPost(pNSS7, pszNTLMAuth);
	if (pNSS7->m_pProxy)
	{
		pNSS7->m_sock = pNSS7->m_pProxy->GetSocket(pNSS7->m_pProxy);
		cdelete(pNSS7->m_pProxy);
		pNSS7->m_pProxy = NULL;
	}
	return NSS7_E_OK;
}

static int SendPost(TNSS7 *pNSS7)
{
	int rv;
	rv = SendData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 SendPost SendData failed: %d.\r\n", rv);
		}
		return rv;
	}
	memset(pNSS7->m_binBuf, 0, sizeof(pNSS7->m_binBuf));
	pNSS7->m_u32HaveRecvLen = 0;
	pNSS7->m_u32RecvLen = 4;
	printf("NSS7 SendPost ok.\r\n");
	return NSS7_E_OK;
}

//接收4个字节看看是不是重定向报文
static int RecvPostHead(TNSS7 *pNSS7)
{
	int rv;
	rv = RecvData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 RecvPostHead RecvData failed: %d.\r\n", rv);
		}
		return rv;
	}
	if (memcmp(pNSS7->m_binBuf, "HTTP", 4) == 0)
	{
		pNSS7->m_u32HaveRecvLen = 4;
		pNSS7->m_bReConnect = XOS_TRUE;
	}
	else
	{
		pNSS7->m_u32HaveRecvLen = 4;
		pNSS7->m_u32RecvLen = 56;
		pNSS7->m_bReConnect = XOS_FALSE;
	}
	printf("NSS7 RecvPostHead ok: m_bReConnect=%d.\r\n", pNSS7->m_bReConnect);
	return NSS7_E_OK;
}

static int RecvPostBody(TNSS7 *pNSS7)
{
	int rv;
	XOS_U32 u32Temp = 0;
	char *p = NULL;
	if (pNSS7->m_bReConnect == XOS_TRUE)
	{
		rv = XOS_TCPRecvHeadNB(pNSS7->m_sock, pNSS7->m_binBuf, sizeof(pNSS7->m_binBuf), &pNSS7->m_u32HaveRecvLen, "\r\n\r\n");
		if (rv == 0)
		{
			pNSS7->m_u32LastRecvTime = XOS_GetTickCount();
		}
		else if (rv == -1)
		{
			printf("NSS7 RecvPostBody XOS_TCPRecvHeadNB failed.\r\n");
			return NSS7_E_TCPRECV;
		}
		else if (rv == -2)
		{
			printf("NSS7 RecvPostBody XOS_TCPRecvHeadNB overflow.\r\n");
			return NSS7_E_POSTLEN;
		}
		else if (rv == 1)
		{
			return NSS7_E_WOULDBLOCK;
		}
		else
		{
			pNSS7->m_u32LastRecvTime = XOS_GetTickCount();
			return NSS7_E_WOULDBLOCK;
		}
		printf("NSS7 RecvPostBody XOS_TCPRecvHeadNB ok:\r\n%s\r\nEND.\r\n", pNSS7->m_binBuf);

		//接收好了，处理重定向数据
		p = strstr(pNSS7->m_binBuf, "Content-Length:");
		if (p == NULL)
		{
			printf("NSS7 RecvPostBody no Content-Length.\r\n");
			return NSS7_E_POST;
		}
		p += strlen("Content-Length:");
		strtok(p, " \r\n\r\n");
		if ((atoi(p) < 0) || (atoi(p) >= sizeof(pNSS7->m_binBuf)))
		{
			printf("NSS7 RecvPostBody invalid Content-Length: %s.\r\n", p);
			return NSS7_E_POSTLEN;
		}
		pNSS7->m_u32RecvLen = atoi(p);
		pNSS7->m_u32HaveRecvLen = 0;
		printf("NSS7 RecvPostBody ok: Content-Length=%d.\r\n", pNSS7->m_u32RecvLen);
		return NSS7_E_OK;
	}
	else
	{
		rv = RecvData(pNSS7, pNSS7->m_binBuf);
		if (rv != NSS7_E_OK)
		{
			if (rv != NSS7_E_WOULDBLOCK)
			{
				printf("NSS7 RecvPostBody RecvData failed: %d.\r\n", rv);
			}
			return rv;
		}
		printf("NSS7 RecvPostBody RecvData ok.\r\n");

		//接收到挑战码
		//首先判断协议版本
		if (pNSS7->m_binBuf[0] != 0x07)
		{
			printf("NSS7 RecvPostBody invalid ver: 0x%02X.\r\n", pNSS7->m_binBuf[0]);
			return NSS7_E_REGISTERVER;
		}
		
		memcpy(&u32Temp, pNSS7->m_binBuf+4, 4);
		if (xos_htonu32(u32Temp) == 0)
		{
			MD5_CTX md5_ctx;
			char binHash[16];
			char binDEVPswHash[16];
			char binPRDPswHash[16];
			char binTempBuf[128];

			//计算接入密码哈希运算结果
			memcpy(binTempBuf, pNSS7->m_binBuf + 8, 32);
			memcpy(binTempBuf + 32, pNSS7->m_binPswHash, sizeof(pNSS7->m_binPswHash));
			MD5_Init(&md5_ctx);
			MD5_Update(&md5_ctx, binTempBuf, 32 + 16);
			MD5_Final(binDEVPswHash, &md5_ctx);

			//计算厂商密码哈希运算结果
			memcpy(binTempBuf, pNSS7->m_binBuf + 8, 32);
			memcpy(binTempBuf + 32, pNSS7->binProducerPsw, sizeof(pNSS7->binProducerPsw));
			MD5_Init(&md5_ctx);
			MD5_Update(&md5_ctx, binTempBuf, 32 + 64);
			MD5_Final(binPRDPswHash, &md5_ctx);

			//计算加密密码
			memcpy(binTempBuf, binDEVPswHash, 16);
			memcpy(binTempBuf + 16, pNSS7->m_binPswHash, sizeof(pNSS7->m_binPswHash));
			MD5_Init(&md5_ctx);
			MD5_Update(&md5_ctx, binTempBuf, 16 + 16);
			MD5_Final(binHash, &md5_ctx);
			Des_Init(&pNSS7->m_des, 0);
			Des_SetKey(&pNSS7->m_des, binHash);

			//填充响应
			memcpy(pNSS7->m_binBuf, binDEVPswHash, 16);
			memcpy(pNSS7->m_binBuf + 16, binPRDPswHash, 16);

			pNSS7->m_u32SendLen = 32;
			pNSS7->m_u32HaveSendLen = 0;

			printf("NSS7 RecvPostBody ok.\r\n");

			return NSS7_E_OK;
		}
		else
		{
			printf("NSS7 RecvPostBody faild: %d.\r\n", xos_htonu32(u32Temp));
			return (NSS7_E_PLATFORMBASE - xos_htonu32(u32Temp)); 
		}
	}
}

static int RecvPostContent(TNSS7 *pNSS7)
{
	int rv;
	char *psz;
	const char *cpszValue;
	mymxml_t *p_mymxml;

	rv = RecvData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 RecvPostContent RecvData failed: %d.\r\n", rv);
		}
		return rv;
	}
	printf("NSS7 RecvPostContent RecvData ok:\r\n%s\r\nEND.\r\n", pNSS7->m_binBuf);

	//<Redirect>
	//        <Result ErrorCode="0" >OK</Result>
	//        <Server Addr="192.168.1.12:8958" ></Server>
	//</Redirect>

	//接收完了,解析出IP和Port
	p_mymxml = mymxmlLoadString(pNSS7->m_binBuf);
	if (p_mymxml != NULL)
	{
		cpszValue = mymxmlGetAttributeValue(p_mymxml, "/Redirect/Result", "ErrorCode", NULL);
		if (cpszValue != NULL)
		{
			if (strcmp(cpszValue, "0") != 0)
			{
				mymxmlDelete(p_mymxml);
				return NSS7_E_REDIRECT;
			}

			cpszValue = mymxmlGetAttributeValue(p_mymxml, "/Redirect/Server", "Addr", NULL);
			if (cpszValue != NULL)
			{
				sprintf(pNSS7->m_binBuf, "%s", cpszValue);
				psz = strstr(pNSS7->m_binBuf, ":");
				if (psz != NULL)
				{
					psz[0] = 0;
					psz ++;
					if (!pNSS7->m_bFixPUIAddr)
					{
						strncpy(pNSS7->m_szPUIAddr, pNSS7->m_binBuf, NSS7_MAXLEN_PUIADDR);
					}
					pNSS7->m_u16PUIPort = atoi(psz);
					XOS_CloseSocket(pNSS7->m_sock);
					pNSS7->m_sock = XOS_INVALID_SOCKET;
					mymxmlDelete(p_mymxml);

					printf("NSS7 RecvPostContent ok: %s:%d.\r\n", pNSS7->m_szPUIAddr, pNSS7->m_u16PUIPort);
					return NSS7_E_OK;
				}
			}
		}
		mymxmlDelete(p_mymxml);
	}

	printf("NSS7 RecvPostContent invalid content.\r\n");
	return NSS7_E_POST;
}

static int SendHash(TNSS7 *pNSS7)
{
	int rv;
	rv = SendData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 SendHash SendData failed: %d.\r\n", rv);
		}
		return rv;
	}
	pNSS7->m_u32HaveRecvLen = 0;
	pNSS7->m_u32RecvLen = 4;
	printf("NSS7 SendHash ok.\r\n");
	return NSS7_E_OK;
}

static int RecvCertify(TNSS7 *pNSS7)
{
	int rv;
	XOS_U32 u32Temp;
	rv = RecvData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 RecvCertify RecvData failed: %d.\r\n", rv);
		}
		return rv;
	}

	memcpy(&u32Temp, pNSS7->m_binBuf, 4);
	if (xos_htonu32(u32Temp) == 0)
	{
		pNSS7->m_u32HaveRecvLen = 0;
		pNSS7->m_u32RecvLen = 4;
		printf("NSS7 RecvCertify ok.\r\n");
		return NSS7_E_OK;
	}
	else
	{
		printf("NSS7 RecvCertify failed: %d.\r\n", xos_htonu32(u32Temp));
		return (NSS7_E_PLATFORMBASE - xos_htonu32(u32Temp));
	}
}

static int RecvParamLen(TNSS7 *pNSS7)
{
	int rv;
	XOS_U32 u32Temp;
	rv = RecvData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 RecvParamLen RecvData failed: %d.\r\n", rv);
		}
		return rv;
	}
	
	memcpy(&u32Temp, pNSS7->m_binBuf, 4);
	if (xos_htonu32(u32Temp) < 0 && xos_htonu32(u32Temp) >= sizeof(pNSS7->m_binBuf))
	{
		printf("NSS7 RecvParamLen invalid len: %d.\r\n", xos_htonu32(u32Temp));
		return NSS7_E_PARAMLEN;
	}
	pNSS7->m_u32HaveRecvLen = 0;
	pNSS7->m_u32RecvLen = xos_htonu32(u32Temp);
	printf("NSS7 RecvParamLen ok: len=%d.\r\n", xos_htonu32(u32Temp));
	return NSS7_E_OK;
}

static int RecvParam(TNSS7 *pNSS7)
{
	int rv;
	XOS_U16 u16Temp;
	rv = RecvData(pNSS7, pNSS7->m_binBuf);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7 RecvParam RecvData failed: %d.\r\n", rv);
		}
		return rv;
	}
	if (pNSS7->m_u32RecvLen >= 2)
	{
		memcpy(&u16Temp, pNSS7->m_binBuf, 2);
		u16Temp = xos_htonu16(u16Temp);
		pNSS7->m_u32KAInterval = u16Temp*1000;
		printf("\tkeepalive interval: %dms.\r\n", pNSS7->m_u32KAInterval);
	}
	if (pNSS7->m_u32RecvLen >= 4)
	{
		memcpy(&u16Temp, pNSS7->m_binBuf + 2, 2);
		u16Temp = xos_htonu16(u16Temp);
		pNSS7->uiSessionTimeOut = u16Temp*1000;
		printf("\tsession timeout: %dms.\r\n", pNSS7->uiSessionTimeOut);
	}
	if (pNSS7->m_u32RecvLen >= 6)
	{
		pNSS7->m_u8CtlFlag = *(XOS_U8 *)(pNSS7->m_binBuf + 5);
		printf("\tCtl flag: %d.\r\n", pNSS7->m_u8CtlFlag);
	}

	pNSS7->m_u32RecvLen = sizeof(TNSS7Header);
	pNSS7->m_u32HaveRecvLen = 0;
	printf("NSS7 Register ok.\r\n");
	return NSS7_E_OK;
}

static int RecvCommand(TNSS7 *pNSS7)
{
	int rv;
	TNSS7Header header;

	//因为NSS7不缓冲接收的命令,所以如果接收有请求,则不再接收了
	if (pNSS7->m_bRecvReqOk)
	{
		return NSS7_E_OK;
	}

	if (pNSS7->m_bRecvHead)
	{
		rv = RecvData(pNSS7, pNSS7->m_binBuf);
		if (rv != NSS7_E_OK)
		{
			if (rv != NSS7_E_WOULDBLOCK)
			{
				printf("NSS7 RecvCommand RecvData failed: %d.\r\n", rv);
			}
			return rv;
		}
		memcpy(&header, pNSS7->m_binBuf, sizeof(header));
		header.u32TransID = xos_htonu32(header.u32TransID);
		header.u32MsgLen = xos_htonu32(header.u32MsgLen);

		if (header.u8SyncFlag != '#')
		{
			printf("NSS7 RecvCommand unkown sync flag: %c %02X.\r\n", header.u8SyncFlag, header.u8SyncFlag);
			return NSS7_E_SYNCFLAG;
		}

		if (header.u8Version != 7)
		{
			printf("NSS7 RecvCommand unkown version: %d.\r\n", header.u8Version);
			return NSS7_E_VERSION;
		}

		if (header.u8CtrlFlag != 0 && header.u8CtrlFlag != 1)
		{
			printf("NSS7 RecvCommand unkown ctrl flag: %d.\r\n", header.u8CtrlFlag);
			return NSS7_E_CTRLFLAG;
		}

		//分析包类型
		if (header.u8PacketType == NSS7HEADER_PACKETTYPE_KEEPALIVE)
		{
			printf("NSS7 RecvCommand keepalive ok.\r\n");
			xlprintf("[%s] NSS7 RecvCommand keepalive ok.\r\n", "nss7_ka");
			return NSS7_E_OK;
		}
		else if (header.u8PacketType == NSS7_MSGTYPE_REQUEST)
		{
			//数据包
			if (header.u8MsgType != NSS7_MSGTYPE_REQUEST)
			{
				printf("NSS7 RecvCommand unkown MsgType :%d\r\n", header.u8MsgType);
				return NSS7_E_MSGTYPE;
			}

			if ((int)header.u32MsgLen < 0)
			{
				printf("NSS7 RecvCommand packetlen overflow: %d.\r\n", (int)header.u32MsgLen);
				return NSS7_E_PACKETLEN;
			}

			assert(pNSS7->m_pRecvPacket == NULL);
			pNSS7->m_pRecvPacket = (TNSS7Packet *)cnew(sizeof(TNSS7Packet), &NSS7PACKET);
			if (pNSS7->m_pRecvPacket == NULL)
			{
				printf("NSS7 RecvCommand cnew packet failed.\r\n");
				return NSS7_E_MALLOC;
			}

			if (!pNSS7->m_pRecvPacket->Create(pNSS7->m_pRecvPacket, header.u32MsgLen))
			{
				printf("NSS7 RecvCommand Create packet failed.\r\n");
				cdelete(pNSS7->m_pRecvPacket);
				pNSS7->m_pRecvPacket = NULL;
				return NSS7_E_MALLOC;
			}

			pNSS7->m_pRecvPacket->SetHeader(pNSS7->m_pRecvPacket, &header);
			pNSS7->m_u32RecvLen = header.u32MsgLen;
			pNSS7->m_u32HaveRecvLen = 0;
			pNSS7->m_bRecvHead = XOS_FALSE;
			//printf("header.u32MsgLen=%d\r\n", header.u32MsgLen);
			rv = RecvCommand(pNSS7);
			return rv;
		}
		else
		{
			printf("NSS7 RecvCommand unkown PacketType :%d\r\n", header.u8PacketType);
			return NSS7_E_PACKETTYPE;
		}
	}
	else
	{
		int i;
		int iLen;
		char *p = NULL;
		assert(pNSS7->m_pRecvPacket != NULL);
		rv = RecvData(pNSS7, pNSS7->m_pRecvPacket->GetBody(pNSS7->m_pRecvPacket));
		if (rv == NSS7_E_OK)
		{
			pNSS7->m_pRecvPacket->GetHeader(pNSS7->m_pRecvPacket, &header);
			p = pNSS7->m_pRecvPacket->GetBody(pNSS7->m_pRecvPacket);
			//DES解密算法
			if (header.u8CtrlFlag == 1)
			{
				iLen = (int)header.u32MsgLen;
				if ((iLen % 8) != 0)
				{
					printf("RecvCommand invalid length: %d.\r\n", iLen);
					cdelete(pNSS7->m_pRecvPacket);
					pNSS7->m_pRecvPacket = NULL;
					return NSS7_E_PACKETLEN;
				}

				for (i=0; i<iLen; i+=8)
				{
					Des_Dec(&pNSS7->m_des, p + i);
				}
			}
			pNSS7->m_bRecvReqOk = XOS_TRUE;
			pNSS7->m_bRecvHead = XOS_TRUE;
			pNSS7->m_u32RecvLen = sizeof(TNSS7Header);
			pNSS7->m_u32HaveRecvLen = 0;
		}
	}
	return rv;
}

static int SendCommand(TNSS7 *pNSS7)
{
	int rv = NSS7_E_OK;
	if (pNSS7->m_pSendPacket == NULL)
	{
		//上次发送完了
		pNSS7->m_pSendPacket = (TNSS7Packet *)pNSS7->m_plistSendPacket->RemoveHead(pNSS7->m_plistSendPacket);
		if (pNSS7->m_pSendPacket != NULL)
		{
			pNSS7->m_u32HaveSendLen = 0;
			pNSS7->m_u32SendLen = pNSS7->m_pSendPacket->GetBufferLen(pNSS7->m_pSendPacket);
		}
	}
	if (pNSS7->m_pSendPacket != NULL)
	{
		rv = SendData(pNSS7, pNSS7->m_pSendPacket->GetBuffer(pNSS7->m_pSendPacket));
		if (rv == NSS7_E_OK)
		{
			//printf("pNSS7->m_u32SendLen=%d.\r\n", pNSS7->m_u32SendLen);
			if (!pNSS7->m_bFirstPacketSendOK)
			{
				pNSS7->m_bFirstPacketSendOK = XOS_TRUE;
			}
			cdelete(pNSS7->m_pSendPacket);
			pNSS7->m_pSendPacket = NULL;
			rv = SendCommand(pNSS7);
		}
	}
	return rv;
}

static void BuildKeepAlive(TNSS7 *pNSS7)
{
	TNSS7Packet *pPacket;

	if ((pNSS7->m_plistSendPacket->Length(pNSS7->m_plistSendPacket) > 0) ||
		(pNSS7->m_pSendPacket != NULL))
	{
		//还有数据,就不发包保活包了
		return ;
	}
	pPacket = (TNSS7Packet*)cnew(sizeof(TNSS7Packet), &NSS7PACKET);
	if (pPacket == NULL)
	{
		printf("BuildKeepAlive cnew failed.\r\n");
		return ;
	}
	if (!pPacket->Create(pPacket, 0))
	{
		cdelete(pPacket);
		printf("BuildKeepAlive Create failed.\r\n");
		return ;
	}
	pPacket->FillHeader(pPacket, 0, NSS7HEADER_PACKETTYPE_KEEPALIVE, 0, 0);

	if (!pNSS7->m_plistSendPacket->AddTail(pNSS7->m_plistSendPacket, pPacket, NULL))
	{
		cdelete(pPacket);
		printf("BuildKeepAlive AddTail failed.\r\n");
	}
	else
	{
		printf("NSS7 BuildKeepAlive ok.\r\n");
		xlprintf("[%s] NSS7 BuildKeepAlive ok.\r\n", "nss7_ka");
	}
	return ;
}

static void BuildPost(TNSS7 *pNSS7, XOS_PSZ pszNTLMAuth)
{
	char *psz = "POST http://%s:%d/PUIServer HTTP/1.0\r\n\
Accept: */*\r\n\
          Content-Type: application/x-www-form-urlencoded\r\n\
          Connection: Keep-Alive\r\n\
          Host: %s:%d\r\n\
          Content-Length: %d\r\n";
          
          sprintf(pNSS7->m_binBuf, psz, pNSS7->m_szPUIAddr, pNSS7->m_u16PUIPort, 
          pNSS7->m_szPUIAddr, pNSS7->m_u16PUIPort, strlen(pNSS7->m_szContent)+1);
          if (pszNTLMAuth != NULL)
          {
          strcat(pNSS7->m_binBuf, "Proxy-Connection: Keep-Alive\r\n");
          strcat(pNSS7->m_binBuf, "Proxy-Authorization: NTLM ");
          strcat(pNSS7->m_binBuf, pszNTLMAuth);
          strcat(pNSS7->m_binBuf, "\r\n");
          }
          strcat(pNSS7->m_binBuf, "\r\n");
          strcat(pNSS7->m_binBuf, pNSS7->m_szContent);
          assert(strlen(pNSS7->m_binBuf) < sizeof(pNSS7->m_binBuf));
          pNSS7->m_u32SendLen = strlen(pNSS7->m_binBuf)+1;
          pNSS7->m_u32HaveSendLen = 0;
          printf("NSS7 BuildPost:\r\n%s\r\nEND.\r\n", pNSS7->m_binBuf);
          return;
}
