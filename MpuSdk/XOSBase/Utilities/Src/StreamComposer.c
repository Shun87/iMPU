/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: StreamComposer.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-28 11:52:50
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../GUID.h"
#include "../StreamComposer.h"


static void Construct(TStreamComposer *pStreamComposer)
{
	pStreamComposer->m_pBuf = NULL;
	pStreamComposer->m_iSize = 0;
	pStreamComposer->m_iMaxSize = 0;
	pStreamComposer->m_bDeleteBuf = XOS_FALSE;
}

static void Destroy(TStreamComposer *pStreamComposer)
{
	if (pStreamComposer->m_bDeleteBuf)
	{
		free(pStreamComposer->m_pBuf);
	}
}

static XOS_BOOL Create(TStreamComposer *pStreamComposer, void *pBuf, int iMaxSize, XOS_BOOL bNBO)
{
	pStreamComposer->m_pBuf = (char *)pBuf;
	pStreamComposer->m_iSize = 0;
	pStreamComposer->m_iMaxSize = iMaxSize;
	pStreamComposer->m_bDeleteBuf = XOS_FALSE;
	pStreamComposer->bNBO = bNBO;
	return XOS_TRUE;
}

/*
static XOS_BOOL Create2(TStreamComposer *pStreamComposer, int iMaxSize)
{
	pStreamComposer->m_pBuf = (char *)malloc(iMaxSize);
	if (pStreamComposer->m_pBuf == 0)
	{
		printf("new memory failed %s:%d\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	pStreamComposer->m_iSize = 0;
	pStreamComposer->m_iMaxSize = iMaxSize;
	pStreamComposer->m_bDeleteBuf = XOS_TRUE;
	return XOS_TRUE;
}
*/

static char* GetBuffer(TStreamComposer *pStreamComposer)
{
	return pStreamComposer->m_pBuf;	
}

static char* GetCurPos(TStreamComposer *pStreamComposer)
{
	return (pStreamComposer->m_pBuf + pStreamComposer->m_iSize);	
}

static int GetLength(TStreamComposer *pStreamComposer)
{
	return pStreamComposer->m_iSize;
}

static int LeftLength(TStreamComposer *pStreamComposer)
{
	return (pStreamComposer->m_iMaxSize - pStreamComposer->m_iSize);
}

static XOS_BOOL SkipLength(TStreamComposer *pStreamComposer, int iSize)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, iSize);
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	pStreamComposer->IncSize(pStreamComposer, iSize);
	return XOS_TRUE;
}

static XOS_BOOL AddPAD2(TStreamComposer *pStreamComposer)
{
	XOS_BOOL rv;
	int iPadLen;
	iPadLen = (pStreamComposer->m_iSize & 1) ? 2 - (pStreamComposer->m_iSize & 1) : 0;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, iPadLen);
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	pStreamComposer->IncSize(pStreamComposer, iPadLen);
	return XOS_TRUE;
}

static XOS_BOOL AddPAD4(TStreamComposer *pStreamComposer)
{
	XOS_BOOL rv;
	int iPadLen;
	iPadLen = (pStreamComposer->m_iSize & 3) ? 4 - (pStreamComposer->m_iSize & 3) : 0;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, iPadLen);
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	pStreamComposer->IncSize(pStreamComposer, iPadLen);
	return XOS_TRUE;
}

static XOS_BOOL AddPAD8(TStreamComposer *pStreamComposer)
{
	XOS_BOOL rv;
	int iPadLen;
	iPadLen = (pStreamComposer->m_iSize & 7) ? 8 - (pStreamComposer->m_iSize & 7) : 0;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, iPadLen);
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	pStreamComposer->IncSize(pStreamComposer, iPadLen);
	return XOS_TRUE;
}

static XOS_BOOL AddCHAR(TStreamComposer *pStreamComposer, XOS_S8 c)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_S8));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	*(XOS_S8 *)(pStreamComposer->m_pBuf + pStreamComposer->m_iSize) = c;
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_S8));
	return XOS_TRUE;
}

static XOS_BOOL AddUCHAR(TStreamComposer *pStreamComposer, XOS_U8 uc)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_U8));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	*(XOS_U8 *)(pStreamComposer->m_pBuf + pStreamComposer->m_iSize) = uc;
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_U8));
	return XOS_TRUE;
}

static XOS_BOOL AddSHORT(TStreamComposer *pStreamComposer, XOS_S16 s)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_S16));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	if (pStreamComposer->bNBO)
	{
		s = htons(s);
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, &s, sizeof(XOS_S16));
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_S16));
	return XOS_TRUE;
}

static XOS_BOOL AddUSHORT(TStreamComposer *pStreamComposer, XOS_U16 us)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_U16));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	if (pStreamComposer->bNBO)
	{
		us = htons(us);
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, &us, sizeof(XOS_U16));
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_U16));
	return XOS_TRUE;
}

static XOS_BOOL AddINT(TStreamComposer *pStreamComposer, XOS_S32 i)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_S32));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	if (pStreamComposer->bNBO)
	{
		i = htonl(i);
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, &i, sizeof(XOS_S32));
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_S32));
	return XOS_TRUE;
}

static XOS_BOOL AddUINT(TStreamComposer *pStreamComposer, XOS_U32 ui)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_U32));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	if (pStreamComposer->bNBO)
	{
		ui = htonl(ui);
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, &ui, sizeof(XOS_U32));
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_U32));
	return XOS_TRUE;
}

static XOS_BOOL AddIP(TStreamComposer *pStreamComposer, XOS_U32 ip)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(XOS_U32));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, &ip, sizeof(XOS_U32));
	pStreamComposer->IncSize(pStreamComposer, sizeof(XOS_U32));
	return XOS_TRUE;
}

static XOS_BOOL AddGUID(TStreamComposer *pStreamComposer, const TCGUID * pGuid)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, sizeof(TCGUID));
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, pGuid, sizeof(TCGUID));
	pStreamComposer->IncSize(pStreamComposer, sizeof(TCGUID));
	return XOS_TRUE;
}

static XOS_BOOL AddString(TStreamComposer *pStreamComposer, XOS_CPSZ cpsz)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, strlen(cpsz)+1);
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	pStreamComposer->m_pBuf[pStreamComposer->m_iSize] = 0;
	strcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, cpsz);
	pStreamComposer->IncSize(pStreamComposer, strlen(cpsz)+1);
	return XOS_TRUE;
}

static XOS_BOOL AddBuffer(TStreamComposer *pStreamComposer, const void * pBuf, int iLen)
{
	XOS_BOOL rv;
	rv = pStreamComposer->BeforeIncSize(pStreamComposer, iLen);
	if (!rv)
	{
		printf("stream composer overflow\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	memcpy(pStreamComposer->m_pBuf + pStreamComposer->m_iSize, pBuf, iLen);
	pStreamComposer->IncSize(pStreamComposer, iLen);
	return XOS_TRUE;
}

static XOS_BOOL BeforeIncSize(TStreamComposer *pStreamComposer, int iSize)
{
	if ((pStreamComposer->m_iSize + iSize) > pStreamComposer->m_iMaxSize) 
	{
		return XOS_FALSE;
	}
	return XOS_TRUE;
}

static XOS_BOOL IncSize(TStreamComposer *pStreamComposer, int iSize)
{
	pStreamComposer->m_iSize += iSize;
	return XOS_TRUE;
}

const TStreamComposer STREAMCOMPOSER =
{
	&Construct,
	&Destroy,
	&Create,
	//&Create2,

	&GetBuffer,
	&GetCurPos,
	&GetLength,
	&LeftLength,
	&SkipLength,
	&AddPAD2,
	&AddPAD4,
	&AddPAD8,
	&AddCHAR,
	&AddUCHAR,
	&AddSHORT,
	&AddUSHORT,
	&AddINT,
	&AddUINT,
	&AddIP,
	&AddGUID,
	&AddString,
	&AddBuffer,

	&BeforeIncSize,
	&IncSize
};
