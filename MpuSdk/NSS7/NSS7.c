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

//////////////////////////////////////////////////////////////////////////
//检查创建参数
static XOS_BOOL CheckCreateParam(char *pszPUIAddr, char *pszPUID, char *pszProducerID,
								 char *pszDevID, char *pszDevType, char *pszDevModel,
								 char *pszHardwareVer, char *pszSoftwareVer, 
								 char *pszProxyType, char *pszProxyAddr,
								 char *pszProxyUsername, char *pszProxyPassword)
{
	//PUIAddr
	if (pszPUIAddr == NULL)
	{
		printf("CheckCreateParam PUIAddr is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszPUIAddr) >= NSS7_MAXLEN_PUIADDR)
	{
		printf("CheckCreateParam invalid PUIAddr length: %d.\r\n", strlen(pszPUIAddr));
		assert(0);
		return XOS_FALSE;
	}
	//PUID
	if (pszPUID == NULL)
	{
		printf("CheckCreateParam PUID is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//ProducerID
	if (pszProducerID == NULL)
	{
		printf("CheckCreateParam ProducerID is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//DevID
	if (pszDevID == NULL)
	{
		printf("CheckCreateParam DevID is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//DevType
	if (pszDevType == NULL)
	{
		printf("CheckCreateParam DevType is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//DevModel
	if (pszDevModel == NULL)
	{
		printf("CheckCreateParam DevModel is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//HardwareVer
	if (pszHardwareVer == NULL)
	{
		printf("CheckCreateParam HardwareVer is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//SoftwareVer
	if (pszSoftwareVer == NULL)
	{
		printf("CheckCreateParam SoftwareVer is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	//ProxyType
	if (pszProxyType == NULL)
	{
		printf("CheckCreateParam ProxyType is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszProxyType) >= NSS7_MAXLEN_PROXYTYPE)
	{
		printf("CheckCreateParam invalid ProxyType length: %d.\r\n", strlen(pszProxyType));
		assert(0);
		return XOS_FALSE;
	}
	//ProxyAddr
	if (pszProxyAddr == NULL)
	{
		printf("CheckCreateParam ProxyAddr is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszProxyAddr) >= NSS7_MAXLEN_PROXYADDR)
	{
		printf("CheckCreateParam invalid ProxyAddr length: %d.\r\n", strlen(pszProxyAddr));
		assert(0);
		return XOS_FALSE;
	}
	//ProxyUserName
	if (pszProxyUsername == NULL)
	{
		printf("CheckCreateParam ProxyUserName is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszProxyUsername) >= NSS7_MAXLEN_PROXYUNAME)
	{
		printf("CheckCreateParam invalid ProxyUserName length: %d.\r\n", strlen(pszProxyUsername));
		assert(0);
		return XOS_FALSE;
	}
	//ProxyPassword
	if (pszProxyPassword == NULL)
	{
		printf("CheckCreateParam ProxyPassword is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszProxyPassword) >= NSS7_MAXLEN_PROXYPSW)
	{
		printf("CheckCreateParam invalid ProxyPassword length: %d.\r\n", strlen(pszProxyPassword));
		assert(0);
		return XOS_FALSE;
	}

	return XOS_TRUE;
}

//构造Post报文Content
static char *BuildPostContent(char *pszContent, int iMaxBufLen, char *pszPUID, 
							  unsigned int uiIP, unsigned int uiSubnetMask, unsigned int uiDialIP,
							  char *pszProducerID, int iKeyResNum,
							  char *pszDevID, char *pszDevType, char *pszDevModel,
							  char *pszHardwareVer, char *pszSoftwareVer, char *pszExtendPost)
{
	char sz[128];
	char szIP[XOS_MAX_IPSTR_LEN];

	pszContent[0] = 0;
	//PUID
	sprintf(sz, "TerminalType=PU&PUID=%s", pszPUID);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//本地IP
	sprintf(sz, "&IP=%s", XOS_U32ToAddr(uiIP, szIP));
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//本地子网掩码
	sprintf(sz, "&SubnetMask=%s", XOS_U32ToAddr(uiSubnetMask, szIP));
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//拨号IP
	sprintf(sz, "&DialIP=%s", XOS_U32ToAddr(uiDialIP, szIP));
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//LocalPort,暂时不做
	//厂商ID
	sprintf(sz, "&ProducerID=%s", pszProducerID);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//关键资源数目
	sprintf(sz, "&KeyResNum=%d", iKeyResNum);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//设备ID
	sprintf(sz, "&DevID=%s", pszDevID);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//设备类型
	sprintf(sz, "&DevType=%s", pszDevType);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//硬件型号
	sprintf(sz, "&DevModel=%s", pszDevModel);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//硬件版本
	sprintf(sz, "&HardwareVer=%s", pszHardwareVer);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//软件版本
	sprintf(sz, "&SoftwareVer=%s", pszSoftwareVer);
	assert((iMaxBufLen - strlen(pszContent)) > strlen(sz));
	strcat(pszContent, sz);
	//附加参数
	if ((pszExtendPost != NULL) && (strlen(pszExtendPost) > 0))
	{
		assert((iMaxBufLen - strlen(pszContent)) > (strlen(sz)-1));
		strcat(pszContent, "&");
		strcat(pszContent, sz);
	}
	
	return pszContent;
}

//填充命令包
static int FillCommandPacket(TNSS7 *pNSS7, char *pMsgBody, int iMsgBodyLen,
							 XOS_U8 u8PacketType, XOS_U8 u8MsgType, XOS_U32 u32TransID)
{
	int iLen;
	int iPadLen;
	XOS_U8 u8CtlFlag;
	TNSS7Packet *pPacket;

	//判断链表长度
	iLen = pNSS7->m_plistSendPacket->Length(pNSS7->m_plistSendPacket);
	if (iLen > NSS7_MAXLEN_PACKETLIST)
	{
		printf("NSS7 FillCommandPacket overflow: %d > %d.\r\n", iLen, NSS7_MAXLEN_PACKETLIST);
		return NSS7_E_WOULDBLOCK;
	}

	//计算填补数据长度
	iPadLen = (8-iMsgBodyLen%8)%8;

	//创建包
	pPacket = (TNSS7Packet*)cnew(sizeof(TNSS7Packet), &NSS7PACKET);
	if (pPacket == NULL)
	{
		printf("NSS7 FillCommandPacket cnew failed.\r\n");
		return NSS7_E_MALLOC;
	}
	if (!pPacket->Create(pPacket, iMsgBodyLen + iPadLen))
	{
		cdelete(pPacket);
		printf("NSS7 FillCommandPacket Create failed.\r\n");
		return NSS7_E_MALLOC;
	}

	if (pNSS7->m_bDebug)
	{
		u8CtlFlag = 0;
	}
	else
	{
		u8CtlFlag = pNSS7->m_u8CtlFlag;
	}
	//填充包
	pPacket->FillHeader(pPacket, u8CtlFlag, u8PacketType, u8MsgType, u32TransID);
	pPacket->FillBody(pPacket, pMsgBody, iMsgBodyLen);

	//如果要加密就加密
	if (u8CtlFlag == 1)
	{
		int i;
		char *p;
		int iLen;
		p = pPacket->GetBody(pPacket);
		iLen = pPacket->GetBodyLen(pPacket);
		for (i=0; i<iLen; i+=8)
		{
			Des_Enc(&pNSS7->m_des, p + i);
		}
	}

	//放到发送链表中
	if (!pNSS7->m_plistSendPacket->AddTail(pNSS7->m_plistSendPacket, pPacket, NULL))
	{
		cdelete(pPacket);
		printf("FillCommandPacket AddTail failed.\r\n");
		return NSS7_E_MALLOC;
	}

	return NSS7_E_OK;
}

//////////////////////////////////////////////////////////////////////////
int NSS7_XOSStartup()
{
//	if (XOS_Startup())
//	{
//		return 0;
//	}
//	else
//	{
//		return -1;
//	}
}

int NSS7_DNS2IPInit()
{
	return DNS2IPInit();
}

NSS7_HANDLE NSS7_Create(char *pszPUIAddr, unsigned short usPUIPort, unsigned char binPswHash[16], 
						char *pszPUID, unsigned int uiIP, unsigned int uiSubnetMask, unsigned int uiDialIP,
						char *pszProducerID, unsigned char binProducerPsw[64], int iKeyResNum,
						char *pszDevID, char *pszDevType, char *pszDevModel,
						char *pszHardwareVer, char *pszSoftwareVer, char *pszExtendPost,
						int bProxy, char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
						char *pszProxyUsername, char *pszProxyPassword,
						int bFixPUIAddr, int bDebug)
{
	TNSS7 *pNSS7;
	char szPasswordHash[32+1];

	//打印版本
	printf("NSS7 Version: %s.\r\n", NCS7_VERSION);

	//检查参数
	if (!CheckCreateParam(pszPUIAddr,pszPUID, pszProducerID,
						  pszDevID, pszDevType, pszDevModel, pszHardwareVer, pszSoftwareVer,
						  pszProxyType, pszProxyAddr, pszProxyUsername, pszProxyPassword))
	{
		return NULL;
	}

	//打印参数
	Hex2Str(binPswHash, szPasswordHash, 16);
	printf("NSS7_Create:\r\n\
\taddr=%s port=%d psw=%s\r\n\
\tpuid=%s ip=0x%08X subnetmask=0x%08X dialip=0x%08X\r\n\
\tpid=%s keyresnum=%d devid=%s devmodel=%s\r\n\
\thardver=%s softver=%s ext=%s\r\n\
\tdg=%d fca=%d.\r\n", 
			pszPUIAddr, usPUIPort, szPasswordHash,
			pszPUID, uiIP, uiSubnetMask, uiDialIP,
			pszProducerID, iKeyResNum, pszDevID, pszDevModel,
			pszHardwareVer, pszSoftwareVer, pszExtendPost,
			bDebug, bFixPUIAddr);
	if (bProxy)
	{
		printf("              useproxy=%d type=%s addr=%s port=%d user=%s psw=%s.\r\n",
				bProxy, pszProxyType, pszProxyAddr, usProxyPort, pszProxyUsername, pszProxyPassword);
	}

	//分配对象内存
	pNSS7 = (TNSS7 *)malloc(sizeof(TNSS7));
	if (pNSS7 == NULL)
	{
		printf("NSS7_Create malloc failed.\r\n");
		return NULL;
	}
	memset(pNSS7, 0, sizeof(TNSS7));

	//初始化对象
	pNSS7->m_bFlatform = XOS_TRUE;
	pNSS7->m_bRecvHead = XOS_TRUE;
	pNSS7->m_sock = XOS_INVALID_SOCKET;
	pNSS7->m_status = NSS7_STATUS_CONNECT;
	pNSS7->m_u32KAInterval = NSS7_KEEPALIVEINTERVAL * 1000;
	pNSS7->uiSessionTimeOut = NSS7_CONNECTTIMEOUT * 1000;
	pNSS7->m_u32LastRecvTime = XOS_GetTickCount();
	pNSS7->m_u32LastSendTime = XOS_GetTickCount();
	pNSS7->m_bProxy = bProxy;
	pNSS7->m_bDebug = bDebug;
	pNSS7->m_bFixPUIAddr = bFixPUIAddr;
	pNSS7->m_u16PUIPort = usPUIPort;
	pNSS7->m_usProxyPort = usProxyPort; 
	pNSS7->m_uiBirthTime = time(NULL);
	memcpy(pNSS7->m_binPswHash, binPswHash, sizeof(pNSS7->m_binPswHash));
	strncpy(pNSS7->m_szPUIAddr, pszPUIAddr, NSS7_MAXLEN_PUIADDR);
	strncpy(pNSS7->m_szProxyType, pszProxyType, NSS7_MAXLEN_PROXYTYPE);
	strncpy(pNSS7->m_szProxyAddr, pszProxyAddr, NSS7_MAXLEN_PROXYADDR);
	strncpy(pNSS7->m_szProxyUserName, pszProxyUsername, NSS7_MAXLEN_PROXYUNAME);
	strncpy(pNSS7->m_szProxyPassword, pszProxyPassword, NSS7_MAXLEN_PROXYPSW);
	memcpy(pNSS7->binProducerPsw, binProducerPsw, sizeof(pNSS7->binProducerPsw));
	BuildPostContent(pNSS7->m_szContent, NSS7_MAXLEN_CONTENT-1, pszPUID,
					 uiIP, uiSubnetMask, uiDialIP,
					 pszProducerID, iKeyResNum, pszDevID, pszDevType, pszDevModel,
					 pszHardwareVer, pszSoftwareVer, pszExtendPost);

	//初始化发生包链表
	pNSS7->m_plistSendPacket = (TObjList *)cnew(sizeof(TObjList), &OBJLIST);
	if (pNSS7->m_plistSendPacket == NULL)
	{
		printf("NSS7_Create m_plistSendPacket cnew failed.\r\n");
		free(pNSS7);
		return NULL;
	}
	if (!pNSS7->m_plistSendPacket->Create(pNSS7->m_plistSendPacket, OBJLISTNODE_DATATYPE_BINARY, sizeof(XOS_U32)))
	{
		printf("NSS7_Create m_plistSendPacket Create failed.\r\n");
		cdelete(pNSS7->m_plistSendPacket);
		free(pNSS7);
		return NULL;
	}

	return pNSS7;
}

NSS7_HANDLE NSS7_CreateWithoutChat(int sock, unsigned char ucCtlFlag, char binDesKey[8],
								   unsigned int uiKeepAliveInterval, unsigned int uiSessionTimeOut,
								   char *pszUserID, char *pszClientType)
{
	TNSS7 *pNSS7;
	char sz[128];
	char szIP[16];

	//打印版本
	printf("NSS7 Version: 7.2010.12.10.\r\n");

	//打印参数
	printf("NSS7_CreateWithoutChat: sock=%d ctl=%d deskey=*** kaiv=%d tm=%d\r\n", 
			sock, ucCtlFlag, uiKeepAliveInterval, uiSessionTimeOut);

	//分配对象内存
	pNSS7 = (TNSS7 *)malloc(sizeof(TNSS7));
	if (pNSS7 == NULL)
	{
		printf("NSS7_Create malloc failed.\r\n");
		return NULL;
	}
	memset(pNSS7, 0, sizeof(TNSS7));

	//初始化对象
	pNSS7->m_u32RecvLen = sizeof(TNSS7Header);
	pNSS7->m_u32HaveRecvLen = 0;
	pNSS7->m_bRecvHead = XOS_TRUE;
	pNSS7->m_sock = sock;
	pNSS7->m_status = NSS7_STATUS_REGISTEROK;
	pNSS7->m_u32KAInterval = uiKeepAliveInterval;
	pNSS7->uiSessionTimeOut = uiSessionTimeOut;
	pNSS7->m_u32LastRecvTime = XOS_GetTickCount();
	pNSS7->m_u32LastSendTime = XOS_GetTickCount();
	pNSS7->m_uiBirthTime = time(NULL);
	memcpy(pNSS7->m_szUserID, pszUserID, NSS7_MAXLEN_USERID-1);
	memcpy(pNSS7->m_szClientType, pszClientType, NSS7_MAXLEN_CLIENTTYPE-1);
	Des_Init(&pNSS7->m_des, 0);
	Des_SetKey(&pNSS7->m_des, binDesKey);

	//初始化发生包链表
	pNSS7->m_plistSendPacket = (TObjList *)cnew(sizeof(TObjList), &OBJLIST);
	if (pNSS7->m_plistSendPacket == NULL)
	{
		printf("NSS7_Create m_plistSendPacket cnew failed.\r\n");
		free(pNSS7);
		return NULL;
	}
	if (!pNSS7->m_plistSendPacket->Create(pNSS7->m_plistSendPacket, OBJLISTNODE_DATATYPE_BINARY, sizeof(XOS_U32)))
	{
		printf("NSS7_Create m_plistSendPacket Create failed.\r\n");
		cdelete(pNSS7->m_plistSendPacket);
		free(pNSS7);
		return NULL;
	}
	XOS_TCPGetPeerAddr(pNSS7->m_sock, &pNSS7->m_uiPeerIP, &pNSS7->m_usPeerPort);
	sprintf(sz, "用户:admin:%s;地址:%s:%d\r\n", pNSS7->m_szClientType, XOS_U32ToAddr(pNSS7->m_uiPeerIP, szIP), pNSS7->m_usPeerPort);
	XOSLog_Record(sz, USER_LOG_TYPE_LOGIN);

	return pNSS7;
}

int NSS7_GetConnectStatus(NSS7_HANDLE hNSS7)
{
	int rv;
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;	

	//参数检查
	if (hNSS7 == NULL)
	{
		printf("NSS7_GetConnectStatus param invalid: %d.\r\n", hNSS7);
		return NSS7_E_PARAM;
	}

	//内部调度
	rv = NSS7_doRun(pNSS7);
	if (rv >= 0)
	{
		if (pNSS7->m_status == NSS7_STATUS_REGISTEROK)
		{
			return NSS7_E_OK;
		}
		else
		{
			return NSS7_E_WOULDBLOCK;
		}
	}
	else
	{
		printf("NSS7_GetConnectStatus NSS7_doRun faild: %d.\r\n", rv);
		return rv;
	}
}

int NSS7_IsFirstPacketSendOK(NSS7_HANDLE hNSS7)
{
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;	

	//参数检查
	if (hNSS7 == NULL)
	{
		printf("NSS7_IsFirstSendOK param invalid: %d.\r\n", hNSS7);
		return 0;
	}

	return pNSS7->m_bFirstPacketSendOK;
}

int NSS7_Run(NSS7_HANDLE hNSS7)
{
	int rv;
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;	

	//参数检查
	if (hNSS7 == NULL)
	{
		printf("NSS7_Run param invalid: %d.\r\n", hNSS7);
		return NSS7_E_PARAM;
	}
	
	//内部调度
	rv = NSS7_doRun(pNSS7);
	if (rv >= 0)
	{
		return NSS7_E_OK;
	}
	else
	{
		printf("NSS7_Run NSS7_doRun failed: %d.\r\n", rv);
		return rv;
	}
}

int NSS7_RecvRequest(NSS7_HANDLE hNSS7, char **ppMsgBody, int *piMsgBodyLen, unsigned int *puiTransID)
{
	int rv;
	TNSS7 *pNSS7;
	TNSS7Header header;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if ((hNSS7 == NULL) || 
		(ppMsgBody == NULL) || 
		(piMsgBodyLen == NULL) || 
		(puiTransID == NULL))
	{
		printf("NSS7_RecvRequest param invalid: h=%d, p=%d, l=%d, t=%d.\r\n",
				hNSS7, ppMsgBody, piMsgBodyLen, puiTransID);
		return NSS7_E_PARAM;
	}
	
	//内部调度
	rv = NSS7_doRun(pNSS7);
	if (rv != NSS7_E_OK)
	{
		if (rv != NSS7_E_WOULDBLOCK)
		{
			printf("NSS7_RecvRequest NSS7_doRun failed: %d.\r\n", rv);
		}
		return rv;
	}

	//判断是否接收成功
	if (pNSS7->m_bRecvReqOk)
	{
		//接收成功,则返回接收到的命令
		pNSS7->m_pRecvPacket->GetHeader(pNSS7->m_pRecvPacket, &header);
		*puiTransID = header.u32TransID;
		*piMsgBodyLen = pNSS7->m_pRecvPacket->GetBodyLen(pNSS7->m_pRecvPacket);
		*ppMsgBody = pNSS7->m_pRecvPacket->RemoveBody(pNSS7->m_pRecvPacket);
		cdelete(pNSS7->m_pRecvPacket);
		pNSS7->m_pRecvPacket = NULL;
		pNSS7->m_bRecvReqOk = XOS_FALSE;
		rv = NSS7_E_OK;
	}
	else
	{
		rv = NSS7_E_WOULDBLOCK;
	}

	return rv;
}

int NSS7_SendResponse(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen, unsigned int uiTransID)
{
	int rv;
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if ((hNSS7 == NULL) || 
		(pMsgBody == NULL))
	{
		printf("NSS7_SendResponse param invalid: %d, %d.\r\n", hNSS7, pMsgBody);
		return NSS7_E_PARAM;
	}

	//填充数据包
	rv = FillCommandPacket(pNSS7, pMsgBody, iMsgBodyLen,
						   NSS7HEADER_PACKETTYPE_DATA, NSS7_MSGTYPE_RESPONSE, uiTransID);

	return rv;
}

int NSS7_SendNotify(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen)
{
	int rv;
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if ((hNSS7 == NULL) || 
		(pMsgBody == NULL))
	{
		printf("NSS7_SendNotify param invalid: %d, %d.\r\n", hNSS7, pMsgBody);
		return NSS7_E_PARAM;
	}

	//填充数据包
	rv = FillCommandPacket(pNSS7, pMsgBody, iMsgBodyLen,
						   NSS7HEADER_PACKETTYPE_DATA, NSS7_MSGTYPE_NOTIFY, 0);

	return rv;
}

int NSS7_SendNotifyEx(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen, int iType)
{
	int rv;
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if ((hNSS7 == NULL) || 
		(pMsgBody == NULL))
	{
		printf("NSS7_SendNotify param invalid: %d, %d.\r\n", hNSS7, pMsgBody);
		return NSS7_E_PARAM;
	}
	switch (iType)
	{
		case NSS7_MSGTYPE_NOTIFY:
		case NSS7_MSGTYPE_REPORT:
		case NSS7_MSGTYPE_INTELL:
		case NSS7_MSGTYPE_EXTENDAPPNOTIFY:
			break;
		default:
		printf("NSS7_SendNotify param invalid, iType:%d.\r\n", iType);
		return NSS7_E_PARAM;
	}
	//填充数据包
	rv = FillCommandPacket(pNSS7, pMsgBody, iMsgBodyLen, NSS7HEADER_PACKETTYPE_DATA, (XOS_U8)iType, 0);

	return rv;
}

int NSS7_SendReport(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen)
{
	int rv;
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if ((hNSS7 == NULL) || 
		(pMsgBody == NULL))
	{
		printf("NSS7_SendReport param invalid: %d,%d.\r\n", hNSS7, pMsgBody);
		return NSS7_E_PARAM;
	}

	//填充数据包
	rv = FillCommandPacket(pNSS7, pMsgBody, iMsgBodyLen,
						   NSS7HEADER_PACKETTYPE_DATA, NSS7_MSGTYPE_REPORT, 0);

	return rv;
}

void NSS7_Free(NSS7_HANDLE hNSS7, void *pMsgBody)
{
	char *p;
	p = (char *)pMsgBody;
	p -= sizeof(TNSS7Header);
	if (p != NULL)
	{
		free(p);
	}
}

void NSS7_Close(NSS7_HANDLE hNSS7)
{
	char sz[128];
	char szIP[16];
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if (hNSS7 == NULL)
	{
		printf("NSS7_Run param invalid: %d.\r\n", hNSS7);
		return;
	}
	if (!pNSS7->m_bFlatform)
	{
		sprintf(sz, "用户:admin:%s;地址:%s:%d\r\n", pNSS7->m_szClientType, XOS_U32ToAddr(pNSS7->m_uiPeerIP, szIP), pNSS7->m_usPeerPort);
		XOSLog_Record(sz, USER_LOG_TYPE_LOGOUT);
	}

	//释放资源
	if (pNSS7->m_sock != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(pNSS7->m_sock);
		pNSS7->m_sock = XOS_INVALID_SOCKET;
	}
	if (pNSS7->m_pProxy != NULL)
	{
		cdelete(pNSS7->m_pProxy);
	}
	if (pNSS7->m_pRecvPacket != NULL)
	{
		cdelete(pNSS7->m_pRecvPacket);
		pNSS7->m_pRecvPacket = NULL;
	}
	if (pNSS7->m_pSendPacket != NULL)
	{
		cdelete(pNSS7->m_pSendPacket);
		pNSS7->m_pSendPacket = NULL;
	}
	if (pNSS7->m_plistSendPacket != NULL)
	{
		cdelete(pNSS7->m_plistSendPacket);
		pNSS7->m_plistSendPacket = NULL;
	}
	free(pNSS7);

	return;
}

int NSS7_GetSessionInfo(NSS7_HANDLE hNSS7, unsigned int *puiLoginTime, 
						unsigned int *puiPeerIP, unsigned short *pusPeerPort, 
						char szUerID[NSS7_MAXLEN_USERID], char szClientType[NSS7_MAXLEN_CLIENTTYPE])
{
	TNSS7 *pNSS7;
	pNSS7 = (TNSS7 *)hNSS7;

	//参数检查
	if (hNSS7 == NULL)
	{
		printf("NSS7_GetSessionInfo param invalid: %d.\r\n", hNSS7);
		return NSS7_E_PARAM;
	}
	
	*puiPeerIP = pNSS7->m_uiPeerIP;
	*pusPeerPort = pNSS7->m_usPeerPort;
	*puiLoginTime = pNSS7->m_uiBirthTime;
	memcpy(szUerID, pNSS7->m_szUserID, NSS7_MAXLEN_USERID);
	memcpy(szClientType, pNSS7->m_szClientType, NSS7_MAXLEN_CLIENTTYPE);

	return NSS7_E_OK;
}
