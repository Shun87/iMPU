/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: llNC7Packet.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2009-6-17 21:47:27
 *  修正备注: 
 *  
 */

#include "NSS7_Defs.h"

static void Construct(TNSS7Packet *cthis)
{
	cthis->m_pBuffer = NULL;
	cthis->m_pBody = NULL;
	cthis->m_iBodyLen = 0;
	cthis->m_u32BirthTime = XOS_GetTickCount();
}

static void Destroy(TNSS7Packet *cthis)
{
	if (cthis->m_pBuffer != NULL)
	{
		free(cthis->m_pBuffer);
	}
}

static XOS_BOOL Create(TNSS7Packet *cthis, int iBodyLen)
{
	cthis->m_pBuffer = (char *)malloc(sizeof(TNSS7Header) + iBodyLen + 1);
	if (cthis->m_pBuffer == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pBuffer, 0, sizeof(TNSS7Header) + iBodyLen + 1);
	cthis->m_iBodyLen = iBodyLen;
	cthis->m_pBody = cthis->m_pBuffer + sizeof(TNSS7Header);
	return XOS_TRUE;
}

static void SetHeader(TNSS7Packet *cthis, TNSS7Header *pHeader)
{
	TNSS7Header *pTmpHeader;
	memcpy(cthis->m_pBuffer, pHeader, sizeof(TNSS7Header));
	pTmpHeader = (TNSS7Header *)cthis->m_pBuffer;
	pTmpHeader->u32TransID = xos_htonu32(pTmpHeader->u32TransID);
	pTmpHeader->u32MsgLen = xos_htonu32(pTmpHeader->u32MsgLen);
}

static void FillHeader(TNSS7Packet *cthis, unsigned char ucCtlflag,
					   unsigned char ucPacketType, unsigned char ucMsgType, 
					   unsigned int uiTransID)
{
	TNSS7Header *pHeader;
	pHeader = (TNSS7Header *)cthis->m_pBuffer;
	memset(pHeader, 0, sizeof(TNSS7Header));

	pHeader->u8SyncFlag = '#';
	pHeader->u8Version = 7;
	pHeader->u8CtrlFlag = ucCtlflag;
	pHeader->u8PacketType = ucPacketType;

	pHeader->u8MsgType = ucMsgType;
	pHeader->u32TransID = xos_htonu32(uiTransID);
	pHeader->u32MsgLen = xos_htonu32(cthis->m_iBodyLen);
}

static XOS_BOOL FillBody(TNSS7Packet *cthis, char *pFill, int iFillLen)
{
	if (iFillLen > cthis->m_iBodyLen)
	{
		printf("llNC7Packet FillBody overflow: %d > %d.\r\n", 
				iFillLen, cthis->m_iBodyLen);
		assert(0);
		return XOS_FALSE;
	}
	if (iFillLen > 0)
	{
		memcpy(cthis->m_pBody, pFill, iFillLen);
	}
	return XOS_TRUE;
}

static char* GetBuffer(TNSS7Packet *cthis)
{
	return cthis->m_pBuffer;
}

static int GetBufferLen(TNSS7Packet *cthis)
{
	return (sizeof(TNSS7Header) + cthis->m_iBodyLen);
}

char* GetBody(TNSS7Packet *cthis)
{
	return cthis->m_pBody;
}

static int GetBodyLen(TNSS7Packet *cthis)
{
	return cthis->m_iBodyLen;
}

static void GetHeader(TNSS7Packet *cthis, TNSS7Header *pHeader)
{
	memcpy(pHeader, cthis->m_pBuffer, sizeof(TNSS7Header));
	//转换字节序
	pHeader->u32TransID = xos_htonu32(pHeader->u32TransID);
	pHeader->u32MsgLen = xos_htonu32(pHeader->u32MsgLen);
	return;
}

static char* RemoveBody(TNSS7Packet *cthis)
{
	char *pBody;
	pBody = cthis->m_pBody;
	cthis->m_pBuffer = NULL;
	cthis->m_iBodyLen = 0;
	return pBody;
}

static XOS_U32 GetBirthTime(TNSS7Packet *cthis)
{
	return cthis->m_u32BirthTime;
}

const TNSS7Packet NSS7PACKET =
{
	&Construct,
	&Destroy,
	&Create,
	
	&SetHeader,
	&FillHeader,
	&FillBody,
	
	&GetBuffer,
	&GetBufferLen,
	&GetBody,
	&GetBodyLen,
	&GetHeader,
	&RemoveBody,
	&GetBirthTime,
};
