/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: DC7.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xiangmm
 *  完成日期: 2010-3-23 20:07:26
 *  修正备注: 
 *  
 */
#include "DC7_Defs.h"

#define FRAME_HEAD_LEN	12	//这个长度加上帧信息长度,必须小于sizeof(TDC7FrameInfo)

//向流分发服务器建立数据通道的POST报文的具体内容
static char s_postCmd[] = "\
POST /DispatchServer.cgi HTTP/1.0\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Connection: Keep-Alive\r\n\
Host: %s\r\n\
Content-Length: %d\r\n\
\r\n\
%s";

static XOS_BOOL CheckProxyParam(char *pszProxyType, char *pszProxyAddr, char *pszUserName, char *pszPassword);
static DC7_HANDLE doCreate(const char *cpszAddr, XOS_U16 u16Port,
						   const char *cpszToken, const char *cpszChID, const char *cpszChData,
						   const char *cpszHost,
						   XOS_BOOL bByProxy,
						   char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
						   char *pszProxyUsername, char *pszProxyPassword);
static int doRecvNotifyPacket(TDC7 *pDC7);
static int doRecvDataPacket(TDC7 *pDC7);
static int doRecvFrame(TDC7 *pDC7);
static int doSendFrame(TDC7 *pDC7);
static int doRun(TDC7 *pDC7);
static void CalculateBytes(TDC7 *pDC7);

int DC7DLL_API DC7_DNS2IPInit()
{
	return DNS2IPInit();
}

//建立一个数据通道
DC7_HANDLE DC7DLL_API DC7_Create(const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost)
{
	return doCreate(cpszAddr, u16Port, cpszToken, cpszChID, cpszChData, cpszHost,
					XOS_FALSE, NULL, NULL, 0, NULL, NULL);
}
DC7_HANDLE DC7DLL_API DC7_CreateByProxy(const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost,
				 	char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
					char *pszProxyUsername, char *pszProxyPassword)
{
	return doCreate(cpszAddr, u16Port, cpszToken, cpszChID, cpszChData, cpszHost,
					XOS_TRUE, pszProxyType, pszProxyAddr, usProxyPort, pszProxyUsername, pszProxyPassword);
}
int DC7DLL_API DC7_CreateBlock(DC7_HANDLE *phDC7, const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost)
{
	int rv;
	DC7_HANDLE hDC7;

	*phDC7 = NULL;
	hDC7 = DC7_Create(cpszAddr, u16Port, cpszToken, cpszChID, cpszChData, cpszHost);
	if (hDC7 == NULL)
	{
		return DC7_E_IVALHANDLE;
	}
	while (1)
	{
		rv = DC7_GetConnectStatus(hDC7);
		if (rv == DC7_E_OK)
		{
			*phDC7 = hDC7;
			return DC7_E_OK;
		}
		else if (rv < 0)
		{
			return rv;
		}
		else
		{
			//等待片刻,再来查询
			XOS_Sleep(2);
		}
	}
}
int DC7DLL_API DC7_CreateByProxyBlock(DC7_HANDLE *phDC7, const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost,
					char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
					char *pszProxyUsername, char *pszProxyPassword)
{
	int rv;
	DC7_HANDLE hDC7;

	*phDC7 = NULL;
	hDC7 = DC7_CreateByProxy(cpszAddr, u16Port, cpszToken, cpszChID, cpszChData, cpszHost,
							 pszProxyType, pszProxyAddr, usProxyPort, pszProxyUsername, pszProxyPassword);
	if (hDC7 == NULL)
	{
		return DC7_E_IVALHANDLE;
	}
	while (1)
	{
		rv = DC7_GetConnectStatus(hDC7);
		if (rv == DC7_E_OK)
		{
			*phDC7 = hDC7;
			return DC7_E_OK;
		}
		else if (rv < 0)
		{
			return rv;
		}
		else
		{
			//等待片刻,再来查询
			XOS_Sleep(2);
		}
	}
}

int DC7DLL_API DC7_GetConnectStatus(DC7_HANDLE hDC7)
{
	int iRet;
	TDC7 *pDC7 = NULL;
	if (hDC7 == NULL)
	{
		return DC7_E_IVALHANDLE;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	iRet = doRun(pDC7);
	XOSMutex_Unlock(pDC7->hMutex);
	return iRet;
}

DC7_HANDLE DC7DLL_API DC7_CreateWithoutChat(int sock)
{
	TDC7 *pDC7 = NULL;
	//打印版本
	printf("DC7 Version: %s.\r\n", DC7_VERSION);
	pDC7 = (TDC7 *)malloc(sizeof(TDC7));
	if (pDC7 == NULL)
	{
		return NULL;
	} 
	memset(pDC7, 0, sizeof(TDC7));
	pDC7->hMutex = XOSMutex_Create();
	if (pDC7->hMutex == NULL)
	{
		free(pDC7);
		return NULL;
	}

	//初始化变量,缺省为0参数不初始化了,因为前面有个memset为0
	pDC7->sock = sock;
	xos_setsocknonblock(pDC7->sock);
	//xos_setsocksendbuf(pDC7->sock, 1024*1024);
	pDC7->u32LastSendOKTime = XOS_GetTickCount();
	pDC7->u32LastRecvOKTime = XOS_GetTickCount();
	pDC7->u32LastTryRecvTime = XOS_GetTickCount();
	pDC7->u32LastSendKATime = XOS_GetTickCount();
	pDC7->bRecvHead = XOS_TRUE;
	pDC7->u32RecvLen = sizeof(TDC7PacketHead);
	pDC7->iProbeStatus = DC7_PS_OK; //协商OK
	pDC7->uiBirthTime = time(NULL);
	MiniList_Init(&pDC7->lstNotify);
	return pDC7;
}

void DC7DLL_API DC7_SetSendBufferSize(DC7_HANDLE hDC7, int iSize)
{
	TDC7 *pDC7 = NULL;
	if (hDC7 == NULL)
	{
		return ;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	if (pDC7->sock != XOS_INVALID_SOCKET)
	{
		xos_setsocksendbuf(pDC7->sock, iSize);
	}
	XOSMutex_Unlock(pDC7->hMutex);
	return ;
}

void DC7DLL_API DC7_SetRecvBufferSize(DC7_HANDLE hDC7, int iSize)
{
	TDC7 *pDC7 = NULL;
	if (hDC7 == NULL)
	{
		return ;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	if (pDC7->sock != XOS_INVALID_SOCKET)
	{
		xos_setsockrecvbuf(pDC7->sock, iSize);
	}
	XOSMutex_Unlock(pDC7->hMutex);
	return ;
}

int DC7DLL_API DC7_Run(DC7_HANDLE hDC7)
{
	int iRet;
	TDC7 *pDC7 = NULL;
	if (hDC7 == NULL)
	{
		return DC7_E_IVALHANDLE;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	iRet = doRun(pDC7);
	if (iRet < 0)
	{
		XOSMutex_Unlock(pDC7->hMutex);
		return iRet;
	}
	else
	{
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_OK;
	}
}

//从协议栈中读取一帧数据
static int doDC7RecvFrame(DC7_HANDLE hDC7, char *binFrameBuf, XOS_U32 *pu32BufLen, TDC7FrameInfo *pInfo)
{
	int iRet;
	TDC7 *pDC7;
	if ((hDC7 == NULL) || (binFrameBuf == NULL))
	{
		printf("[%08X] doDC7RecvFrame invalid param: %d.\r\n", hDC7, binFrameBuf);
		return DC7_E_PARAM;
	}
	pDC7 = (TDC7 *)hDC7;
	
	xlprintf("[%s]: [%08X] doDC7RecvFrame: *pu32BufLen=%d.\r\n", "dc7", hDC7, *pu32BufLen);

	//内部调度一次
	iRet = doRun(pDC7);
	if (iRet != DC7_E_OK)
	{
		return iRet;
	}

	if (pDC7->bRecvFrameOk)
	{
		if (*pu32BufLen < pDC7->u32RecvFrameOffset)
		{
			*pu32BufLen = pDC7->u32RecvFrameOffset;
			return DC7_E_FRAMEBUF_LEN;
		}
		memcpy(binFrameBuf, pDC7->pRecvFrameBuf, pDC7->u32RecvFrameOffset);
		memcpy(pInfo, &pDC7->RecvFrameInfo, sizeof(TDC7FrameInfo));
		*pu32BufLen = pDC7->u32RecvFrameOffset;
		pDC7->u32RecvFrameOffset = 0;
		pDC7->bRecvFrameOk = XOS_FALSE;
		if (pDC7->bRecvEmptyPacketOk)
		{
			pDC7->bRecvEmptyPacketOk = XOS_FALSE;
			return DC7_E_END;
		}
		else
		{
			return DC7_E_OK;
		}
	}
	else
	{
		return DC7_E_WOULDBLOCK;
	}
}

int DC7DLL_API DC7_RecvFrame(DC7_HANDLE hDC7, char *binFrameBuf, XOS_U32 *pu32BufLen, TDC7FrameInfo *pInfo)
{
	int iRet;
	TDC7 *pDC7;
	if (hDC7 == NULL)
	{
		printf("DC7_RecvFrame invalid param: %d.\r\n", hDC7);
		return DC7_E_PARAM;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	iRet = doDC7RecvFrame(hDC7, binFrameBuf, pu32BufLen, pInfo);
	XOSMutex_Unlock(pDC7->hMutex);
	return iRet;
}

int DC7DLL_API DC7_RecvRawFrame(DC7_HANDLE hDC7, char *binFrameBuf, unsigned int *pu32BufLen, int *piFrameType, int *pbKeyFrame)
{
	int rv;
	int iBufLen;
	int iHeadLen;
	TDC7FrameInfo info;
	TDC7 *pDC7;

	if (hDC7 == NULL)
	{
		printf("[%08X] DC7_RecvRawFrame invalid param.\r\n", hDC7);
		return DC7_E_PARAM;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	//读取数据帧
	(*pu32BufLen) -= sizeof(TDC7FrameInfo);	//这样处理之后,后面的拷贝就是安全的了
	rv = doDC7RecvFrame(hDC7, binFrameBuf+sizeof(TDC7FrameInfo), pu32BufLen, &info);
	if (rv != DC7_E_OK)
	{
		XOSMutex_Unlock(pDC7->hMutex);
		return rv;//下层的空包也在这里返回了.
	}

	//填充裸数据
	iHeadLen = FRAME_HEAD_LEN;
	memset(binFrameBuf, 0, iHeadLen);
	memcpy(binFrameBuf, &info.u32UTCTime, sizeof(unsigned int));
	memcpy(binFrameBuf+4, &info.u32Timestamp, sizeof(unsigned int));
	memcpy(binFrameBuf+8, &info.bIFrame, sizeof(unsigned char));
	if (info.u8FrameType == DC7_FRAME_VIDEO)
	{
		iHeadLen += sizeof(TDC7VideoFrameInfo);
		memcpy(binFrameBuf+FRAME_HEAD_LEN, &info.Res.video, sizeof(info.Res.video));
	}
	else if (info.u8FrameType == DC7_FRAME_AUDIO)
	{
		iHeadLen += sizeof(TDC7AudioFrameInfo);
		memcpy(binFrameBuf+FRAME_HEAD_LEN, &info.Res.audio, sizeof(info.Res.audio));
	}
	else if (info.u8FrameType == DC7_FRAME_DATA)
	{
	}
	else if (info.u8FrameType == DC7_FRAME_GPS)
	{
		iHeadLen += sizeof(TDC7GPSFrameInfo);
		memcpy(binFrameBuf+FRAME_HEAD_LEN, &info.Res.GPS, sizeof(info.Res.GPS));
	}
	else if (info.u8FrameType == DC7_FRAME_SP)
	{
		iHeadLen += sizeof(TDC7SPFrameInfo);
		memcpy(binFrameBuf+FRAME_HEAD_LEN, &info.Res.SP, sizeof(info.Res.SP));
	}
	else if (info.u8FrameType == DC7_FRAME_PIC)
	{
		iHeadLen += sizeof(TDC7PICFrameInfo);
		memcpy(binFrameBuf+FRAME_HEAD_LEN, &info.Res.PIC, sizeof(info.Res.PIC));
	}
	else
	{
		printf("DC7_RecvRawFrame invalid type: %d.\r\n", info.u8FrameType);
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_FRAME_TYPE;
	}
	iBufLen = *pu32BufLen;
	memcpy(binFrameBuf+iHeadLen, binFrameBuf+sizeof(TDC7FrameInfo), iBufLen);
	iBufLen += iHeadLen;

	*pu32BufLen = iBufLen;
	*piFrameType = info.u8FrameType;
	*pbKeyFrame = info.bIFrame;
	XOSMutex_Unlock(pDC7->hMutex);

	return rv;
}

int DC7DLL_API DC7_SendEmptyPacket(DC7_HANDLE hDC7)
{
	TDC7 *pDC7 = NULL;

	pDC7 = (TDC7 *)hDC7;
	//参数检测
	if (hDC7 == NULL)
	{
		printf("DC7_SendEmptyPacket invalid param: %d.\r\n", hDC7);
		return DC7_E_PARAM;
	}
	XOSMutex_Lock(pDC7->hMutex);
	if (pDC7->bSendEmptyPacket)
	{
		//上次的空包没有发送完
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_WOULDBLOCK;
	}
	pDC7->bSendEmptyPacket = XOS_TRUE;
	XOSMutex_Unlock(pDC7->hMutex);
	return DC7_E_OK;
}

static int doDC7SendFrame(DC7_HANDLE hDC7, char *binSendFrame, XOS_U32 u32FrameLen, TDC7FrameInfo *pFrameinfo)
{
	TDC7 *pDC7 = NULL;
	int iRet;

	pDC7 = (TDC7 *)hDC7;
	
	xlprintf("[%s]: [%08X] doDC7SendFrame: len=%d.\r\n", "dc7", hDC7, u32FrameLen);
	
	//参数检测
	if ((hDC7 == NULL) || (binSendFrame == NULL))
	{
		printf("[%08X] doDC7SendFrame invalid param: %d.\r\n", hDC7, binSendFrame);
		return DC7_E_PARAM;
	}
	
	//内部调度一次
	iRet = doRun(pDC7);
	if (iRet != DC7_E_OK)
	{
		return iRet;
	}

	//判断是否有数据没有发完
	if (!pDC7->bSendFrameValid)
	{
		//都发完了,则拷贝数据帧
		if (pDC7->u32MaxSendFrameLen < u32FrameLen)
		{
			//pDC7->pSendFrameBuf初始化时要赋NULL,不然会内存出错
			pDC7->pSendFrameBuf = (char *)realloc(pDC7->pSendFrameBuf, u32FrameLen);
			if (pDC7->pSendFrameBuf == NULL)
			{
				return DC7_E_MALLOC;
			}
			pDC7->u32MaxSendFrameLen = u32FrameLen;
		}
		memcpy(pDC7->pSendFrameBuf, binSendFrame, u32FrameLen);
		memcpy(&pDC7->SendFrameInfo, pFrameinfo, sizeof(TDC7FrameInfo));
		pDC7->u32SendFrameLen = u32FrameLen;
		pDC7->bSendFrameValid = XOS_TRUE;
		iRet = doRun(pDC7);
		assert(iRet != DC7_E_WOULDBLOCK);
	}
	else
	{
		//否则,等待数据发完
		iRet = DC7_E_WOULDBLOCK;
	}

	return iRet;
}

//将一帧数据分包发送
int DC7DLL_API DC7_SendFrame(DC7_HANDLE hDC7, char *binSendFrame, XOS_U32 u32FrameLen, TDC7FrameInfo *pFrameinfo)
{
	TDC7 *pDC7 = NULL;
	int iRet;

	if (hDC7 == NULL)
	{
		printf("[%08X] DC7_SendFrame invalid param.\r\n", hDC7);
		return DC7_E_PARAM;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	iRet = doDC7SendFrame(hDC7, binSendFrame, u32FrameLen, pFrameinfo);
	XOSMutex_Unlock(pDC7->hMutex);

	return iRet;
}

int DC7DLL_API DC7_SendRawFrame(DC7_HANDLE hDC7, char *binSendFrame, unsigned int u32FrameLen, int iFrameType, unsigned short u16FrameNO)
{
	int iRet;
	int iHeadLen;
	TDC7FrameInfo info;
	TDC7 *pDC7;

	if (!hDC7)
	{
		printf("[%08X] DC7_SendRawFrame invalid param.\r\n", hDC7);
		return DC7_E_PARAM;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);

	xlprintf("[%s]: [%08X] DC7_SendRawFrame len=%d t=%d fn=%d.\r\n", "dc7", hDC7, u32FrameLen, iFrameType, u16FrameNO);

	memset(&info, 0, sizeof(info));
	info.u16FrameNO = u16FrameNO;
	info.u8FrameType = iFrameType;
	iHeadLen = FRAME_HEAD_LEN;
	memcpy(&info.u32UTCTime, binSendFrame, sizeof(unsigned int));
	memcpy(&info.u32Timestamp, binSendFrame+4, sizeof(unsigned int));
	memcpy(&info.bIFrame, binSendFrame+8, sizeof(unsigned char));
	if (info.u8FrameType == DC7_FRAME_VIDEO)
	{
		iHeadLen += sizeof(TDC7VideoFrameInfo);
		memcpy(&info.Res.video, binSendFrame+FRAME_HEAD_LEN, sizeof(info.Res.video));
	}
	else if (info.u8FrameType == DC7_FRAME_AUDIO)
	{
		iHeadLen += sizeof(TDC7AudioFrameInfo);
		memcpy(&info.Res.audio, binSendFrame+FRAME_HEAD_LEN, sizeof(info.Res.audio));
	}
	else if (info.u8FrameType == DC7_FRAME_DATA)
	{
	}
	else if (info.u8FrameType == DC7_FRAME_GPS)
	{
		iHeadLen += sizeof(TDC7GPSFrameInfo);
		memcpy(&info.Res.GPS, binSendFrame+FRAME_HEAD_LEN, sizeof(info.Res.GPS));
	}
	else if (info.u8FrameType == DC7_FRAME_SP)
	{
		iHeadLen += sizeof(TDC7SPFrameInfo);
		memcpy(&info.Res.SP, binSendFrame+FRAME_HEAD_LEN, sizeof(info.Res.SP));
	}
	else if (info.u8FrameType == DC7_FRAME_PIC)
	{
		iHeadLen += sizeof(TDC7PICFrameInfo);
		memcpy(&info.Res.PIC, binSendFrame+FRAME_HEAD_LEN, sizeof(info.Res.PIC));
	}
	else
	{
		printf("DC7_SendRawFrame invalid type: %d.\r\n", info.u8FrameType);
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_FRAME_TYPE;
	}
	if ((int)u32FrameLen < iHeadLen)
	{
		printf("DC7_SendRawFrame invalid framelen: %d < %d.\r\n", u32FrameLen, iHeadLen);
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_PARAM;
	}
	iRet = doDC7SendFrame(hDC7, binSendFrame+iHeadLen, u32FrameLen-iHeadLen, &info);

	XOSMutex_Unlock(pDC7->hMutex);

	return iRet;
}

//从通知链表中获得一个通知
int DC7DLL_API DC7_RecvNotify(DC7_HANDLE hDC7, char *pbinNotifyBuf, XOS_U32 *pu32BufLen)
{
	TDC7NotifyNode *pNode = NULL;
	TDC7 *pDC7 = NULL;
	if (hDC7 == NULL)
	{
		printf("DC7_RecvNotify invalid param: %d %d.\r\n", hDC7);
		return DC7_E_IVALHANDLE;
	}
	pDC7 = (TDC7 *)hDC7;
	XOSMutex_Lock(pDC7->hMutex);
	pNode = (TDC7NotifyNode *)MiniList_RemoveHead(&pDC7->lstNotify);
	if (pNode != NULL)
	{
		if (*pu32BufLen < pNode->u32Len)
		{
			MiniList_AddTail(&pDC7->lstNotify, (TMiniListNode *)pNode);
			*pu32BufLen = pNode->u32Len;
			XOSMutex_Unlock(pDC7->hMutex);
			return DC7_E_FRAMEBUF_LEN;
		}
		memcpy(pbinNotifyBuf, pNode->pbinBuffer, pNode->u32Len);
		*pu32BufLen = pNode->u32Len;
		free(pNode->pbinBuffer);
		free(pNode);
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_OK;
	}
	else
	{
		XOSMutex_Unlock(pDC7->hMutex);
		return DC7_E_WOULDBLOCK;
	}
}

//关闭DC7模块
void DC7DLL_API DC7_Close(DC7_HANDLE hDC7)
{
	TDC7 *pDC7 = NULL;
	if (hDC7 == NULL)
	{
		return ;
	}
	pDC7 = (TDC7 *)hDC7;
	if (pDC7->sock != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(pDC7->sock);
	}
	if (pDC7->pSendFrameBuf != NULL)
	{
		free(pDC7->pSendFrameBuf);
		pDC7->pSendFrameBuf = NULL;
	}
	if (pDC7->pRecvFrameBuf != NULL)
	{
		free(pDC7->pRecvFrameBuf);
		pDC7->pRecvFrameBuf = NULL;
	}
	if (pDC7->hMutex != NULL)
	{
		XOSMutex_Delete(pDC7->hMutex);
		pDC7->hMutex = NULL;
	}
	MiniList_Clear(&pDC7->lstNotify);
	free(pDC7);
	return ;
}

//////////////////////////////////////////////////////////////////////////
static XOS_BOOL CheckProxyParam(char *pszProxyType, char *pszProxyAddr, char *pszUserName, char *pszPassword)
{
	if (pszProxyType == NULL)
	{
		printf("CheckProxyParam proxy type is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}

	if (pszProxyAddr == NULL)
	{
		printf("CheckProxyParam proxy addr is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszProxyAddr) > DC7_MAX_PROXY_ADDR_LEN)
	{
		printf("CheckProxyParam invalid proxy addr length: %d.\r\n", strlen(pszProxyAddr));
		assert(0);
		return XOS_FALSE;
	}

	if (pszUserName == NULL)
	{
		printf("CheckProxyParam proxy user name is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszUserName) > DC7_MAX_PROXY_USERNAME_LEN)
	{
		printf("CheckProxyParam invalid proxy user name length: %d.\r\n", strlen(pszUserName));
		assert(0);
		return XOS_FALSE;
	}
	
	if (pszPassword == NULL)
	{
		printf("CheckProxyParam proxy password is null.\r\n");
		assert(0);
		return XOS_FALSE;
	}
	if (strlen(pszPassword) > DC7_MAX_PROXY_PASSWORD_LEN)
	{
		printf("CheckProxyParam invalid proxy password length: %d.\r\n", strlen(pszPassword));
		assert(0);
		return XOS_FALSE;
	}

	return XOS_TRUE;
}

static DC7_HANDLE doCreate(const char *cpszAddr, XOS_U16 u16Port,
						   const char *cpszToken, const char *cpszChID, const char *cpszChData,
						   const char *cpszHost,
						   XOS_BOOL bByProxy,
						   char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
						   char *pszProxyUsername, char *pszProxyPassword)
{
	TDC7 *pDC7 = NULL;
	char szContent[DC7_PACKET_MAXLEN];
	char szHost[DC7_MAX_ADDR_LEN+8];
	
	//打印版本
	printf("DC7 Version: %s.\r\n", DC7_VERSION);

	//参数检查,同时合成content
	memset(szContent, 0, sizeof(szContent));
	//Addr
	if (cpszAddr == NULL)
	{
		printf("doCreate addr is null.\r\n");
		assert(0);
		return NULL;
	}
	if (strlen(cpszAddr) > DC7_MAX_ADDR_LEN)
	{
		printf("doCreate invalid addr length: %d.\r\n", strlen(cpszAddr));
		assert(0);
		return NULL;
	}
	//Token
	if ((cpszToken == NULL) || 
		(strlen(szContent) + strlen("Token=") + strlen(cpszToken) >= sizeof(szContent)))
	{
		printf("DC7_Create check token failed.\r\n");
		assert(0);
		return NULL;
	}
	else
	{
		strcat(szContent, "Token=");
		strcat(szContent, cpszToken);
	}
	//ChID
	if (cpszChID != NULL)
	{
		if (strlen(szContent) + strlen("&ChID=") + strlen(cpszChID) >= sizeof(szContent))
		{
			printf("DC7_Create check ChID failed.\r\n");
			assert(0);
			return NULL;
		}
		else
		{
			strcat(szContent, "&ChID=");
			strcat(szContent, cpszChID);
		}
	}
	//ChData
	if (cpszChData != NULL)
	{
		if (strlen(szContent) + strlen("&ChData=") + strlen(cpszChData) >= sizeof(szContent))
		{
			printf("DC7_Create check ChData failed.\r\n");
			assert(0);
			return NULL;
		}
		else
		{
			strcat(szContent, "&ChData=");
			strcat(szContent, cpszChData);
		}
	}
	if (bByProxy)
	{
		if (!CheckProxyParam(pszProxyType, pszProxyAddr, pszProxyUsername, pszProxyPassword))
		{
			printf("llNC7_Create CheckProxyParam failed.\r\n");
			assert(0);
			return NULL;
		}
	}

	//打印参数
	printf("DC7_Create: addr=%s port=%d token=%s id=%s data=%s host=%s.\r\n", 
			cpszAddr, u16Port, cpszToken, cpszChID, cpszChData, cpszHost);
	if (bByProxy)
	{
		printf("            useproxy=%d type=%s addr=%s port=%d user=%s psw=%s.\r\n",
			bByProxy, pszProxyType, pszProxyAddr, usProxyPort, pszProxyUsername, pszProxyPassword);
	}

	//分配对象内存
	pDC7 = (TDC7 *)malloc(sizeof(TDC7));
	if (pDC7 == NULL)
	{
		return NULL;
	}
	memset(pDC7, 0, sizeof(TDC7));

	//记下传入的参数
	pDC7->sock = XOS_INVALID_SOCKET;
	strncpy(pDC7->szAddr, cpszAddr, sizeof(pDC7->szAddr)-1);
	pDC7->u16Port = u16Port;
	pDC7->bByProxy = bByProxy;
	if (pDC7->bByProxy)
	{
		strncpy(pDC7->szProxyType, pszProxyType, sizeof(pDC7->szProxyType)-1);
		strncpy(pDC7->szProxyAddr, pszProxyAddr, sizeof(pDC7->szProxyAddr)-1);
		pDC7->usProxyPort = usProxyPort;
		strncpy(pDC7->szProxyUserName, pszProxyUsername, sizeof(pDC7->szProxyUserName)-1);
		strncpy(pDC7->szProxyPassword, pszProxyPassword, sizeof(pDC7->szProxyPassword)-1);
	}
	pDC7->hMutex = XOSMutex_Create();
	if (!pDC7->hMutex)
	{
		free(pDC7);
		return NULL;
	}

	//初始化内部变量,缺省为0参数不初始化了,因为前面有个memset为0
	if ((cpszHost == NULL) || (strcmp(cpszHost, "") == 0))
	{
		//没有指定Host,使用目标IP和端口合成
		memset(szHost, 0, sizeof(szHost));
		//前面判断过cpszAddr的长度,肯定不会溢出
		sprintf(szHost, "%s:%d", cpszAddr, u16Port);
		sprintf(pDC7->binSendPacket, s_postCmd, szHost, strlen(szContent)+1, szContent);
	}
	else
	{
		//指定了Host,则使用指定的Host
		sprintf(pDC7->binSendPacket, s_postCmd, cpszHost, strlen(szContent)+1, szContent);
	}
	pDC7->u32LastSendOKTime = XOS_GetTickCount();
	pDC7->u32LastRecvOKTime = XOS_GetTickCount();
	pDC7->u32LastTryRecvTime = XOS_GetTickCount();
	pDC7->u32LastSendKATime = XOS_GetTickCount();
	pDC7->u32SendLen = strlen(pDC7->binSendPacket)+1;
	pDC7->iProbeStatus = DC7_PS_CONNECTING; //开始连接
	pDC7->uiBirthTime = time(NULL);
	MiniList_Init(&pDC7->lstNotify);
	return pDC7;
}

//尽最大能力去接收到一帧，
static int doRecvFrame(TDC7 *pDC7)
{
	int iRet;

	if (pDC7->bRecvFrameOk == XOS_TRUE)
	{
		return DC7_E_OK;
	}
	if (pDC7->bRecvHead)
	{
		//接收头
		pDC7->u32LastTryRecvTime = XOS_GetTickCount();
		iRet = XOS_TCPRecvDataNB(pDC7->sock, pDC7->binRecvPacket, &pDC7->u32HaveRecvLen, pDC7->u32RecvLen);
		if (iRet == -1)
		{
			return DC7_E_TCPRECV;
		}
		else if (iRet == 1)
		{
			return DC7_E_WOULDBLOCK;
		}
		else if (iRet == 2)
		{
			pDC7->u32LastRecvOKTime = XOS_GetTickCount();
			return DC7_E_WOULDBLOCK;
		}
		else //0
		{
			//分析数据头
			pDC7->uiTotalBytes += pDC7->u32RecvLen;
			pDC7->u32LastRecvOKTime = XOS_GetTickCount();
			memcpy(&pDC7->RecvHead, pDC7->binRecvPacket, sizeof(TDC7PacketHead));
			//ver
			if (pDC7->RecvHead.u8Ver != DC7_PACKET_VER)
			{
				//assert(0);
				return DC7_E_PACKET_VER;
			}

			//Packet Type
			if (pDC7->RecvHead.u8Type == DC7_PACKET_DATA || pDC7->RecvHead.u8Type == DC7_PACKET_NOTIFY)
			{
				//Packet Len
				if (ntohs(pDC7->RecvHead.u16Len) > DC7_PACKET_MAXLEN)
				{
					return DC7_E_PACKET_LEN;
				}
				pDC7->u32HaveRecvLen = 0;
				pDC7->u32RecvLen = ntohs(pDC7->RecvHead.u16Len);
				if (pDC7->u32RecvLen == 0)
				{
					pDC7->u32RecvLen = sizeof(TDC7PacketHead);
					pDC7->bRecvFrameOk = XOS_TRUE;
					pDC7->bRecvHead = XOS_TRUE;
					pDC7->bRecvEmptyPacketOk = XOS_TRUE;//以这个为空包标准, 这样也就支持了空帧的发送了.
					return DC7_E_OK;
				}
				else
				{
					pDC7->bRecvHead = XOS_FALSE;
				}
			}
			else if (pDC7->RecvHead.u8Type == DC7_PACKET_KEEP_ALIVE)
			{
				pDC7->u32HaveRecvLen = 0;
				pDC7->u32RecvLen = sizeof(TDC7PacketHead);
				pDC7->bRecvHead = XOS_TRUE;
			}
			else
			{
				return DC7_E_PACKET_TYPE;
			}
			iRet = doRecvFrame(pDC7);
		}
	}
	else
	{
		//接收信息体
		pDC7->u32LastTryRecvTime = XOS_GetTickCount();
		iRet = XOS_TCPRecvDataNB(pDC7->sock, pDC7->binRecvPacket, &pDC7->u32HaveRecvLen, pDC7->u32RecvLen);
		if (iRet == -1)
		{
			return DC7_E_TCPRECV;
		}
		else if (iRet == 1)
		{
			return DC7_E_WOULDBLOCK;
		}
		else if (iRet == 2)
		{
			pDC7->u32LastRecvOKTime = XOS_GetTickCount();
			return DC7_E_WOULDBLOCK;
		}
		else //0
		{
			pDC7->uiTotalBytes += pDC7->u32RecvLen;
			pDC7->u32LastRecvOKTime = XOS_GetTickCount();
			pDC7->bRecvHead = XOS_TRUE;
			pDC7->u32HaveRecvLen = 0;
			pDC7->u32RecvLen = sizeof(TDC7PacketHead);
			if (pDC7->RecvHead.u8Type == DC7_PACKET_DATA)
			{
				iRet = doRecvDataPacket(pDC7);
			}
			else if (pDC7->RecvHead.u8Type == DC7_PACKET_NOTIFY)
			{
				iRet = doRecvNotifyPacket(pDC7);
			}
			else
			{
				XOSLog_PrintMem((unsigned char *)pDC7, sizeof(TDC7));
				assert(0);
			}
			if (iRet == DC7_E_WOULDBLOCK)
			{
				//返回DC7_E_WOULDBLOCK表示还需要包才能够凑成完整的帧
				iRet = doRecvFrame(pDC7);
			}
		}
	}
	return iRet;
}

static int doRecvDataPacket(TDC7 *pDC7)
{
	static int iDatalen;
	int iTempOffset = 0;
	if (pDC7->RecvHead.u8BeginFlag == 1)
	{
		//帧头
		TDC7FrameHead frameHead;
		memcpy(&frameHead, pDC7->binRecvPacket, sizeof(TDC7FrameHead));
		
		memset(&pDC7->RecvFrameInfo, 0, sizeof(TDC7FrameInfo));
		pDC7->RecvFrameInfo.u16FrameNO = frameHead.u16FrameNO;
		pDC7->RecvFrameInfo.u32Timestamp = frameHead.u32Timestamp;
		pDC7->RecvFrameInfo.u32UTCTime = frameHead.u32UTCTime;
		pDC7->RecvFrameInfo.u8FrameType = frameHead.u8FrameType;
		pDC7->RecvFrameInfo.bIFrame = frameHead.u8IFrameFlag;

		iTempOffset += sizeof(TDC7FrameHead);

		if (frameHead.u8FrameType == DC7_FRAME_VIDEO)
		{
			memcpy(&pDC7->RecvFrameInfo.Res.video, pDC7->binRecvPacket + sizeof(TDC7FrameHead), sizeof(TDC7VideoFrameInfo));
			iTempOffset += sizeof(TDC7VideoFrameInfo);
		}
		else if (frameHead.u8FrameType == DC7_FRAME_AUDIO)
		{
			memcpy(&pDC7->RecvFrameInfo.Res.audio, pDC7->binRecvPacket + sizeof(TDC7FrameHead), sizeof(TDC7AudioFrameInfo));
			iTempOffset += sizeof(TDC7AudioFrameInfo);
		}
		else if (frameHead.u8FrameType == DC7_FRAME_GPS)
		{
			memcpy(&pDC7->RecvFrameInfo.Res.GPS, pDC7->binRecvPacket + sizeof(TDC7FrameHead), sizeof(TDC7GPSFrameInfo));
			iTempOffset += sizeof(TDC7GPSFrameInfo);
		}
		else if (frameHead.u8FrameType == DC7_FRAME_SP)
		{
			memcpy(&pDC7->RecvFrameInfo.Res.SP, pDC7->binRecvPacket + sizeof(TDC7FrameHead), sizeof(TDC7SPFrameInfo));
			iTempOffset += sizeof(TDC7SPFrameInfo);
		}
		else if (frameHead.u8FrameType == DC7_FRAME_DATA)
		{

		}
		else if (frameHead.u8FrameType == DC7_FRAME_PIC)
		{
			memcpy(&pDC7->RecvFrameInfo.Res.PIC, pDC7->binRecvPacket + sizeof(TDC7FrameHead), sizeof(TDC7PICFrameInfo));
			iTempOffset += sizeof(TDC7PICFrameInfo);
		}
		else
		{
			return DC7_E_FRAME_TYPE;
		}
		pDC7->u32RecvFrameOffset = 0;
	}	
	else if (pDC7->RecvHead.u8BeginFlag == 0)
	{
		//直接拷贝数据
		iTempOffset = 0;
	}
	else
	{
		return DC7_E_PACKET_BFLAG;
	}

	//检查接收帧缓冲
	if (pDC7->u32MaxRecvFrameLen < (ntohs(pDC7->RecvHead.u16Len) - iTempOffset + pDC7->u32RecvFrameOffset))
	{
		pDC7->pRecvFrameBuf = (char *)realloc(pDC7->pRecvFrameBuf, (ntohs(pDC7->RecvHead.u16Len) - iTempOffset) + pDC7->u32RecvFrameOffset);
		if (pDC7->pRecvFrameBuf == NULL)
		{
			return DC7_E_MALLOC;
		}
		pDC7->u32MaxRecvFrameLen = (ntohs(pDC7->RecvHead.u16Len) - iTempOffset) + pDC7->u32RecvFrameOffset;
	}
	memcpy(pDC7->pRecvFrameBuf + pDC7->u32RecvFrameOffset, pDC7->binRecvPacket + iTempOffset, ntohs(pDC7->RecvHead.u16Len) - iTempOffset);
	pDC7->u32RecvFrameOffset += (ntohs(pDC7->RecvHead.u16Len) - iTempOffset);

	//判断结束标志
	if (pDC7->RecvHead.u8EndFlag == 1)
	{
		pDC7->bRecvFrameOk = XOS_TRUE;
		return DC7_E_OK;
	}
	else if (pDC7->RecvHead.u8EndFlag == 0)
	{
		return DC7_E_WOULDBLOCK;
	}
	else
	{
		return DC7_E_PACKET_EFLAG;
	}
}

static int doRecvNotifyPacket(TDC7 *pDC7)
{
	if (pDC7->RecvHead.u8BeginFlag == 1 && pDC7->RecvHead.u8EndFlag == 1)
	{
		TDC7NotifyNode *pNode = NULL;
		//检查链表满
		if (MiniList_Length(&pDC7->lstNotify) > DC7_MAXNOTIFYLIST_LEN)
		{
			pNode = (TDC7NotifyNode *)MiniList_RemoveHead(&pDC7->lstNotify);
			free(pNode->pbinBuffer);
			free(pNode);
		}
		pNode = (TDC7NotifyNode *)malloc(sizeof(TDC7NotifyNode));
		if (pNode == NULL)
		{
			return DC7_E_MALLOC;
		}
		pNode->pbinBuffer = (char *)malloc(ntohs(pDC7->RecvHead.u16Len)); //这个的长度已经判断了
		if (pNode->pbinBuffer == NULL)
		{
			free(pNode);
			return DC7_E_MALLOC;
		}
		//拷贝数据
		memcpy(pNode->pbinBuffer, pDC7->binRecvPacket, ntohs(pDC7->RecvHead.u16Len));
		pNode->u32Len = ntohs(pDC7->RecvHead.u16Len);
		//上链表
		MiniList_AddTail(&pDC7->lstNotify, (TMiniListNode *)pNode);
		return DC7_E_WOULDBLOCK;
	}
	else
	{
		assert(0);
		return DC7_E_UNKNOW;
	}
}

static int FillPakcet(TDC7 *pDC7)
{
	if (pDC7->bFillSendPacket == XOS_TRUE)
	{
		//包中有数据
		return DC7_E_OK;
	}
	else
	{
		if (!pDC7->bSendFrameValid)
		{
			//帧缓冲中没数据
			return DC7_E_NODATA;
		}
		else
		{
			TDC7PacketHead PacketHead;
			char *pTempBuf = NULL;
			int iLeftPacketLen = DC7_SENDPACKET_MAXLEN;

			//填包头
			memset(&PacketHead, 0, sizeof(PacketHead));
			PacketHead.u8Ver = DC7_PACKET_VER;
			PacketHead.u8IFrameFlag = pDC7->SendFrameInfo.bIFrame;//??????
			PacketHead.u8Rev = 0;
			PacketHead.u8Type = DC7_PACKET_DATA;
			
			pTempBuf = pDC7->binSendPacket + sizeof(TDC7PacketHead);
			iLeftPacketLen -= sizeof(TDC7PacketHead);

			//填充帧数据
			if (pDC7->u32SendFrameOffset == 0)
			{
				//填充的公共帧头
				TDC7FrameHead FrameHead;
				memset(&FrameHead, 0, sizeof(TDC7FrameHead));
				FrameHead.u16FrameNO = pDC7->SendFrameInfo.u16FrameNO;
				FrameHead.u32Timestamp = pDC7->SendFrameInfo.u32Timestamp;
				FrameHead.u32UTCTime = pDC7->SendFrameInfo.u32UTCTime;
				FrameHead.u8DitherIdx = 0xFF;
				FrameHead.u8DitherSetsIdx = 0xFF;
				FrameHead.u8FrameType = pDC7->SendFrameInfo.u8FrameType;
				FrameHead.u8IFrameFlag = pDC7->SendFrameInfo.bIFrame;

				memcpy(pTempBuf, &FrameHead, sizeof(TDC7FrameHead));
				pTempBuf += sizeof(TDC7FrameHead);
				iLeftPacketLen -= sizeof(TDC7FrameHead);

				//填充不同帧的帧头
				if (pDC7->SendFrameInfo.u8FrameType == DC7_FRAME_VIDEO)
				{
					memcpy(pTempBuf, &pDC7->SendFrameInfo.Res.video, sizeof(TDC7VideoFrameInfo));
					pTempBuf += sizeof(TDC7VideoFrameInfo);
					iLeftPacketLen -= sizeof(TDC7VideoFrameInfo);
				}
				else if (pDC7->SendFrameInfo.u8FrameType == DC7_FRAME_AUDIO)
				{
					memcpy(pTempBuf, &pDC7->SendFrameInfo.Res.audio, sizeof(TDC7AudioFrameInfo));
					pTempBuf += sizeof(TDC7AudioFrameInfo);
					iLeftPacketLen -= sizeof(TDC7AudioFrameInfo);
				}
				else if (pDC7->SendFrameInfo.u8FrameType == DC7_FRAME_DATA)
				{
				}
				else if (pDC7->SendFrameInfo.u8FrameType == DC7_FRAME_GPS)
				{
					memcpy(pTempBuf, &pDC7->SendFrameInfo.Res.GPS, sizeof(TDC7GPSFrameInfo));
					pTempBuf += sizeof(TDC7GPSFrameInfo);
					iLeftPacketLen -= sizeof(TDC7GPSFrameInfo);
				}
				else if (pDC7->SendFrameInfo.u8FrameType == DC7_FRAME_SP)
				{
					memcpy(pTempBuf, &pDC7->SendFrameInfo.Res.SP, sizeof(TDC7SPFrameInfo));
					pTempBuf += sizeof(TDC7SPFrameInfo);
					iLeftPacketLen -= sizeof(TDC7SPFrameInfo);
				}
				else if (pDC7->SendFrameInfo.u8FrameType == DC7_FRAME_PIC)
				{
					memcpy(pTempBuf, &pDC7->SendFrameInfo.Res.PIC, sizeof(TDC7PICFrameInfo));
					pTempBuf += sizeof(TDC7PICFrameInfo);
					iLeftPacketLen -= sizeof(TDC7PICFrameInfo);
				}
				else
				{
					return DC7_E_FRAME_TYPE;
				}
				PacketHead.u8BeginFlag = 0x01;
			}
			else
			{
				PacketHead.u8BeginFlag = 0x00;
			}
			
			//复制数据了
			if (pDC7->u32SendFrameLen - pDC7->u32SendFrameOffset > (XOS_U32)iLeftPacketLen)
			{
				PacketHead.u8EndFlag = 0x00;
				memcpy(pTempBuf, pDC7->pSendFrameBuf + pDC7->u32SendFrameOffset, iLeftPacketLen);
				pDC7->u32SendFrameOffset += iLeftPacketLen;
				iLeftPacketLen -= iLeftPacketLen;
			}
			else //等于的情况也是最后一个包
			{
				PacketHead.u8EndFlag = 0x01;
				memcpy(pTempBuf, pDC7->pSendFrameBuf + pDC7->u32SendFrameOffset, (pDC7->u32SendFrameLen - pDC7->u32SendFrameOffset));
				iLeftPacketLen -= (pDC7->u32SendFrameLen - pDC7->u32SendFrameOffset);

				//可以重新填充帧缓冲了
				pDC7->u32SendFrameOffset = 0;
				pDC7->u32SendFrameLen = 0;
				pDC7->bSendFrameValid = XOS_FALSE;
			}

			//真正的填充包头
			PacketHead.u16Len = htons((XOS_U16)(DC7_SENDPACKET_MAXLEN - iLeftPacketLen - sizeof(TDC7PacketHead)));
			memcpy(pDC7->binSendPacket, &PacketHead, sizeof(TDC7PacketHead));

			pDC7->u32SendLen = DC7_SENDPACKET_MAXLEN - iLeftPacketLen;
			pDC7->u32HaveSendLen = 0;
			pDC7->bFillSendPacket = XOS_TRUE;
			return DC7_E_OK;
		}
	}
}

static int doSendFrame(TDC7 *pDC7)
{
	int iRet;
	TDC7PacketHead PacketHead;

	//装保活包
	if ((XOS_GetTickCount() - pDC7->u32LastSendKATime > DC7_KEEPALIVE_INTERVAL) && 
		(!pDC7->bFillSendPacket) && (!pDC7->bSendFrameValid))
	{
		pDC7->u32LastSendKATime = XOS_GetTickCount();
		memset(&PacketHead, 0, sizeof(TDC7PacketHead));
		PacketHead.u8Ver = 0x03;
		PacketHead.u8Type = 0x01;

		memcpy(pDC7->binSendPacket, &PacketHead, sizeof(TDC7PacketHead));		
		pDC7->u32SendLen = sizeof(TDC7PacketHead);
		pDC7->u32HaveSendLen = 0;
		pDC7->bFillSendPacket = XOS_TRUE;
	}
	//发送空包
	if (pDC7->bSendEmptyPacket && (!pDC7->bFillSendPacket) && (!pDC7->bSendFrameValid))
	{
		memset(&PacketHead, 0, sizeof(TDC7PacketHead));
		PacketHead.u8Ver = 0x03;
		PacketHead.u8Type = 0x00;
		PacketHead.u16Len = 0;
		PacketHead.u8BeginFlag = 0x01;
		PacketHead.u8EndFlag = 0x01;

		memcpy(pDC7->binSendPacket, &PacketHead, sizeof(TDC7PacketHead));		
		pDC7->u32SendLen = sizeof(TDC7PacketHead);
		pDC7->u32HaveSendLen = 0;
		pDC7->bSendEmptyPacket = XOS_FALSE;
		pDC7->bFillSendPacket = XOS_TRUE;
	}
	//装数据
	if (!pDC7->bFillSendPacket)
	{
		iRet = FillPakcet(pDC7); //没发送完也没关系,里面会管的
		if (iRet < 0)
		{
			return iRet; //错误码
		}
		else
		{
			iRet = DC7_E_OK; //要是有数据FillPakcet中会pDC7->bFillSendPacket = XOS_TRUE;
			//有数据就发送,没数据直接返回: DC7_E_OK
		}
	}

	//发送数据
	if (pDC7->bFillSendPacket)
	{
		iRet = XOS_TCPSendDataNB(pDC7->sock, pDC7->binSendPacket, &pDC7->u32HaveSendLen, pDC7->u32SendLen);
		if (iRet == -1)
		{
			return DC7_E_TCPSEND;
		}
		else if (iRet == 1)
		{
			return DC7_E_WOULDBLOCK;
		}
		else if (iRet == 2)
		{
			pDC7->u32LastSendOKTime = XOS_GetTickCount();
			return DC7_E_WOULDBLOCK;
		}
		else //0
		{
			pDC7->uiTotalBytes += pDC7->u32SendLen;
			pDC7->u32LastSendOKTime = XOS_GetTickCount();
			pDC7->bFillSendPacket = XOS_FALSE;
			pDC7->u32SendLen = 0;
			pDC7->u32HaveSendLen = 0;
			iRet = doSendFrame(pDC7);
		}
	}
	return iRet;
}

static int doRun(TDC7 *pDC7)
{
	int iRet;

	/*
	//可能对方不接收,所以还是发不出去.所以不能以这个依据断开连接.
	if (XOS_GetTickCount() - pDC7->u32LastSendOKTime > DC7_CONNECT_MAXTIME)
	{
		printf("[%08X] DC7 DoRun send timeout, timeoutinteval: %dms.\r\n", pDC7, DC7_CONNECT_MAXTIME);
		return DC7_E_TIMEOUT;
	}
	*/
	
	//如果我们上层不接收的话,就会超时.所以这个地方不能用接收时间来判断
	//后来上层保证可以不停的接收,所以可以用接收时间来判断了
	//2011-5-27 10:53:56再后来,改成上次尝试接收的时间和上次成功接收时间的差
	//这样即使上面不来接收也是没有问题的了.
//	if (pDC7->u32LastTryRecvTime - pDC7->u32LastRecvOKTime > DC7_CONNECT_MAXTIME)
//	{
//		printf("[%08X] DC7 doRun timeout, timeoutinteval: %dms.\r\n", pDC7, DC7_CONNECT_MAXTIME);
//		return DC7_E_TIMEOUT;
//	}
    if (XOS_GetTickCount() - pDC7->u32LastSendOKTime > DC7_CONNECT_MAXTIME)
    {
        printf("[%08X] DC7 doRun timeout, timeoutinteval: %dms.\r\n", pDC7, DC7_CONNECT_MAXTIME);
		return DC7_E_TIMEOUT;
    }

	switch(pDC7->iProbeStatus) 
	{
		case DC7_PS_CONNECTING:
			if (!pDC7->bByProxy)
			{
				XOS_U32 u32IP;
				u32IP = XOS_AddrToU32(pDC7->szAddr);
				if ((u32IP == 0) || (u32IP == 0xFFFFFFFF))
				{
					u32IP = DNS2IP(pDC7->szAddr);
				}
				if ((u32IP == 0) || (u32IP == 0xFFFFFFFF))
				{
					//还没有解释成功
					break;
				}
				pDC7->sock = XOS_TCPConnectNB(u32IP, pDC7->u16Port);
				if (pDC7->sock == XOS_INVALID_SOCKET)
				{
					printf("DC7 doRun ConnectNB failed.\r\n");
					return DC7_E_CONNECT;
				}
			}
			else
			{
				char szDestAddr[MAX_PROXY_DESTADDR_LEN];
				char szProxyAddr[MAX_PROXY_DESTADDR_LEN];
				printf("DC7 doRun ConnectByProxy: dest=%s:%d.\r\n", pDC7->szAddr, pDC7->u16Port);
				pDC7->pProxy = cnew(sizeof(TProxy), &PROXY);
				if (pDC7->pProxy == NULL)
				{
					printf("DC7 doRun pProxy cnew failed.\r\n");
					return DC7_E_MALLOC;
				}
				//对传入的长度进行了限制,不会越界的
				sprintf(szDestAddr, "%s:%d", pDC7->szAddr, pDC7->u16Port);
				sprintf(szProxyAddr, "%s:%d", pDC7->szProxyAddr, pDC7->usProxyPort);
				if (!pDC7->pProxy->Create(pDC7->pProxy, pDC7->szProxyType, szDestAddr,
										  szProxyAddr, pDC7->szProxyUserName, pDC7->szProxyPassword))
				{
					printf("DC7 doRun pProxy Create failed.\r\n");
					return DC7_E_CONNECT;
				}
			}
			pDC7->iProbeStatus = DC7_PS_CONNECTPROBE;
			//故意不break,接着往下走

		case DC7_PS_CONNECTPROBE:
			if (!pDC7->bByProxy)
			{
				iRet = XOS_TCPConnectProbe(pDC7->sock);
			}
			else
			{
				iRet = pDC7->pProxy->Probe(pDC7->pProxy);
				if (iRet == PROXY_PROBE_OK)
				{
					pDC7->sock = pDC7->pProxy->GetSocket(pDC7->pProxy);
					cdelete(pDC7->pProxy);
					pDC7->pProxy = NULL;
					iRet = 0;
				}
				else if (iRet == PROXY_PROBE_WOULDBLOCK)
				{
					iRet = 1;
				}
				else //其他错误
				{
					iRet = -1;
				}
			}
			if (iRet == -1)
			{
				printf("DC7 doRun ConnectProbe failed.\r\n");
				return DC7_E_CONNECT;
			}
			else if (iRet == 1)
			{
				break;
			}
			else
			{
				//xos_setsocksendbuf(pDC7->sock, 1024*1024);
				pDC7->iProbeStatus = DC7_PS_SEND_POST;
			}
			//故意不break,接着往下走

		case DC7_PS_SEND_POST:
			iRet = XOS_TCPSendDataNB(pDC7->sock, pDC7->binSendPacket, &pDC7->u32HaveSendLen, pDC7->u32SendLen);
			if (iRet == -1)
			{
				return DC7_E_TCPSEND;
			}
			else if (iRet == 1)
			{
				break;
			}
			else if (iRet == 2)
			{
				pDC7->u32LastSendOKTime = XOS_GetTickCount();
				break;
			}
			else
			{
				printf("DC7 send post ok.\r\n");
				pDC7->u32RecvLen = 1;
				pDC7->u32HaveRecvLen = 0;
				pDC7->iProbeStatus = DC7_PS_RECV_STATUS;
			}
			//故意不break,接着往下走

		case DC7_PS_RECV_STATUS:
			pDC7->u32LastTryRecvTime = XOS_GetTickCount();
			iRet = XOS_TCPRecvDataNB(pDC7->sock, pDC7->binRecvPacket, &pDC7->u32HaveRecvLen, pDC7->u32RecvLen);
			if (iRet == -1)
			{
				return DC7_E_TCPRECV;
			}
			else if (iRet == 1)
			{
				break;
			}
			else if (iRet == 2)
			{
				pDC7->u32LastRecvOKTime = XOS_GetTickCount();
				break;
			}
			else
			{
				//分析接收状态
				int iStatus;
				iStatus = (int)pDC7->binRecvPacket[0];
				if (iStatus == DC7_STATUS_OK)
				{
					pDC7->u32HaveRecvLen = 0;
					pDC7->u32RecvLen = sizeof(TDC7PacketHead);
					pDC7->bRecvHead = XOS_TRUE;
					pDC7->u32LastRecvOKTime = XOS_GetTickCount();
					pDC7->iProbeStatus = DC7_PS_OK;
					printf("DC7 Register ok.\r\n");
				}
				else if (iStatus == DC7_STATUS_ALREADY_EXIST)
				{
					return DC7_E_CHEXIST;
				}
				else if (iStatus == DC7_STATUS_TOKEN_INVALID)
				{
					return DC7_E_INVTOKEN;
				}
				else if (iStatus == DC7_STATUS_TYPE_ERROR)
				{
					return DC7_E_CHTYPEERR;
				}
				else
				{
					return DC7_E_UNKNOW;
				}
			}
			//故意不break,接着往下走

		case DC7_PS_OK:
			iRet = doSendFrame(pDC7);
			if (iRet < 0)
			{
				return iRet;
			}
			iRet = doRecvFrame(pDC7);
			if (iRet < 0)
			{
				return iRet;
			}
			//没有出错, 统计流量.
			CalculateBytes(pDC7);
			return DC7_E_OK;
 
		default:
			assert(0);
			return DC7_E_UNKNOW;
	}
	return DC7_E_WOULDBLOCK;
}

// int DC7DLL_API DC7_GetChnInfo(DC7_HANDLE hDC7, unsigned int *puiBirthTime, 
// 				   unsigned int *puiPeerIP, unsigned short *puiPeerPort, 
// 				   unsigned int *puiTotalMBytes, unsigned int *puiTotalKBytes)
// {
// 	int iRet;
// 	TDC7 *pDC7;
// 
// 	if (hDC7 == NULL)
// 	{
// 		return DC7_E_IVALHANDLE;
// 	}
// 	pDC7 = (TDC7 *)hDC7;
// 	XOSMutex_Lock(pDC7->hMutex);
// 	iRet = XOS_TCPGetPeerAddr(pDC7->sock, puiPeerIP, puiPeerPort);
// 	if (iRet == XOS_FALSE)
// 	{
// 		printf("DC7_GetChnInfo XOS_TCPGetPeerAddr failed.\r\n");
// 		XOSMutex_Unlock(pDC7->hMutex);
// 		return DC7_E_UNKNOW;
// 	}
// 	*puiBirthTime = pDC7->uiBirthTime;
// 	*puiTotalMBytes = pDC7->uiTotalMBytes;
// 	*puiTotalKBytes = pDC7->uiTotalKBytes;
// 	XOSMutex_Unlock(pDC7->hMutex);
// 	return DC7_E_OK;
// }

static void CalculateBytes(TDC7 *pDC7)
{
	assert(pDC7);
	pDC7->uiTotalKBytes += (pDC7->uiTotalBytes / 1024);
	pDC7->uiTotalBytes %= 1024;
	pDC7->uiTotalMBytes += (pDC7->uiTotalKBytes / 1024);
	pDC7->uiTotalKBytes %= 1024;
	return ;
}

