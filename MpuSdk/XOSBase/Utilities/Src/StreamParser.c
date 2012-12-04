/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: StreamParser.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-28 09:32:05
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../GUID.h"
#include "../StreamParser.h"


static void Construct(TStreamParser *pStreamParser)
{
	pStreamParser->m_pBuf = 0;
	pStreamParser->m_iOffset = 0;
}

static void Destroy(TStreamParser *pStreamParser)
{
}

static XOS_BOOL Create(TStreamParser *pStreamParser, const void *p, int iSize, XOS_BOOL bNBO)
{
	pStreamParser->m_pBuf = (char *)p;
	assert(pStreamParser->m_pBuf);
	pStreamParser->m_iOffset = 0;
	pStreamParser->m_iSize = iSize;
	pStreamParser->bNBO = bNBO;
	return XOS_TRUE;
}

static XOS_BOOL IsEnd(TStreamParser *pStreamParser)
{
	return !pStreamParser->BeforeDecSize(pStreamParser, 1);
}

static char* GetCurPos(TStreamParser *pStreamParser)
{
	return (pStreamParser->m_pBuf + pStreamParser->m_iOffset);	
}

static int GetLeftLen(TStreamParser *pStreamParser)
{
	return pStreamParser->m_iSize;
}

static XOS_BOOL SkipLength(TStreamParser *pStreamParser, int iSize)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, iSize);
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, iSize);
	return XOS_TRUE;
}

static XOS_BOOL SkipPAD2(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	int iSkipLen;
	iSkipLen = (pStreamParser->m_iOffset & 1) ? 2 - (pStreamParser->m_iOffset & 1) : 0;
	rv = pStreamParser->BeforeDecSize(pStreamParser, iSkipLen);
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, iSkipLen);
	return XOS_TRUE;
}

static XOS_BOOL SkipPAD4(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	int iSkipLen;
	iSkipLen = (pStreamParser->m_iOffset & 3) ? 4 - (pStreamParser->m_iOffset & 3) : 0;
	rv = pStreamParser->BeforeDecSize(pStreamParser, iSkipLen);
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, iSkipLen);
	return XOS_TRUE;
}

static XOS_BOOL SkipPAD8(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	int iSkipLen;
	iSkipLen = (pStreamParser->m_iOffset & 7) ? 8 - (pStreamParser->m_iOffset & 7) : 0;
	rv = pStreamParser->BeforeDecSize(pStreamParser, iSkipLen);
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, iSkipLen);
	return XOS_TRUE;
}

static XOS_BOOL GetCHAR(TStreamParser *pStreamParser, XOS_S8 *pc)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_S8));
	if (!rv)
	{
		return XOS_FALSE;
	}
	*pc = *(pStreamParser->m_pBuf + pStreamParser->m_iOffset);
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_S8));
	return XOS_TRUE;
}

static XOS_BOOL SkipCHAR(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	//XOS_S8 c;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_S8));
	if (!rv)
	{
		return XOS_FALSE;
	}
	//c = *(pStreamParser->m_pBuf + pStreamParser->m_iOffset);
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_S8));
	return XOS_TRUE;
}

static XOS_BOOL GetUCHAR(TStreamParser *pStreamParser, XOS_U8 *puc)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U8));
	if (!rv)
	{
		return XOS_FALSE;
	}
	*puc = *(XOS_U8*)(pStreamParser->m_pBuf + pStreamParser->m_iOffset);
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U8));
	return XOS_TRUE;
}

static XOS_BOOL SkipUCHAR(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	//XOS_U8 uc;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U8));
	if (!rv)
	{
		return XOS_FALSE;
	}
	//uc = *(XOS_U8*)(pStreamParser->m_pBuf + pStreamParser->m_iOffset);
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U8));
	return XOS_TRUE;
}

static XOS_BOOL GetSHORT(TStreamParser *pStreamParser, XOS_S16 *ps)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_S16));
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(ps, pStreamParser->m_pBuf + pStreamParser->m_iOffset, sizeof(XOS_S16));
	if (pStreamParser->bNBO)
	{
		*ps = ntohs(*ps);
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_S16));
	return XOS_TRUE;
}

static XOS_BOOL SkipSHORT(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_S16));
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_S16));
	return XOS_TRUE;
}

static XOS_BOOL GetUSHORT(TStreamParser *pStreamParser, XOS_U16 *pus)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U16));
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(pus, pStreamParser->m_pBuf + pStreamParser->m_iOffset, sizeof(XOS_U16));
	if (pStreamParser->bNBO)
	{
		*pus = ntohs(*pus);
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U16));
	return XOS_TRUE;
}

static XOS_BOOL SkipUSHORT(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U16));
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U16));
	return XOS_TRUE;
}

static XOS_BOOL GetINT(TStreamParser *pStreamParser, XOS_S32 *pi)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_S32));
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(pi, pStreamParser->m_pBuf + pStreamParser->m_iOffset, sizeof(XOS_S32));
	if (pStreamParser->bNBO)
	{
		*pi = ntohl(*pi);
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_S32));
	return XOS_TRUE;
}

static XOS_BOOL SkipINT(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_S32));
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_S32));
	return XOS_TRUE;
}

static XOS_BOOL GetUINT(TStreamParser *pStreamParser, XOS_U32 *pui)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U32));
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(pui, pStreamParser->m_pBuf + pStreamParser->m_iOffset, sizeof(XOS_U32));
	if (pStreamParser->bNBO)
	{
		*pui = ntohl(*pui);
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U32));
	return XOS_TRUE;
}

static XOS_BOOL SkipUINT(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U32));
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U32));
	return XOS_TRUE;
}

static XOS_BOOL GetIP(TStreamParser *pStreamParser, XOS_U32 *pip)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U32));
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(pip, pStreamParser->m_pBuf + pStreamParser->m_iOffset, sizeof(XOS_U32));
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U32));
	return XOS_TRUE;
}

static XOS_BOOL SkipIP(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(XOS_U32));
	if (!rv)
	{
		return XOS_FALSE;
	}
	pStreamParser->DecSize(pStreamParser, sizeof(XOS_U32));
	return XOS_TRUE;
}

static XOS_BOOL GetGUID(TStreamParser *pStreamParser, TCGUID *pguid)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(TCGUID));
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(pguid, pStreamParser->m_pBuf + pStreamParser->m_iOffset, sizeof(TCGUID));
	pStreamParser->DecSize(pStreamParser, sizeof(TCGUID));
	return XOS_TRUE;
}

static TCGUID * SkipGUID(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	TCGUID *pGuid;
	rv = pStreamParser->BeforeDecSize(pStreamParser, sizeof(TCGUID));
	if (!rv)
	{
		return NULL;
	}
	pGuid = (TCGUID *)(pStreamParser->m_pBuf + pStreamParser->m_iOffset);
	pStreamParser->DecSize(pStreamParser, sizeof(TCGUID));
	return pGuid;
}

static XOS_BOOL GetString(TStreamParser *pStreamParser, char *lpsz, int len)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, strlen(pStreamParser->m_pBuf + pStreamParser->m_iOffset)+1);
	if (!rv)
	{
		return XOS_FALSE;
	}
	if ((int)strlen(pStreamParser->m_pBuf + pStreamParser->m_iOffset) >= len)
	{
		return XOS_FALSE;
	}
	lpsz[0] = 0;
	strcpy(lpsz, pStreamParser->m_pBuf + pStreamParser->m_iOffset);
	pStreamParser->DecSize(pStreamParser, strlen(pStreamParser->m_pBuf + pStreamParser->m_iOffset)+1);
	return XOS_TRUE;
}

static char *SkipString(TStreamParser *pStreamParser)
{
	XOS_BOOL rv;
	char *lpsz;
	rv = pStreamParser->BeforeDecSize(pStreamParser, strlen(pStreamParser->m_pBuf + pStreamParser->m_iOffset)+1);
	if (!rv)
	{
		return NULL;
	}
	lpsz = pStreamParser->m_pBuf + pStreamParser->m_iOffset;
	pStreamParser->DecSize(pStreamParser, strlen(pStreamParser->m_pBuf + pStreamParser->m_iOffset)+1);
	return lpsz;
}

static XOS_BOOL GetBuffer(TStreamParser *pStreamParser, char *p, int len)
{
	XOS_BOOL rv;
	rv = pStreamParser->BeforeDecSize(pStreamParser, len);
	if (!rv)
	{
		return XOS_FALSE;
	}
	memcpy(p, pStreamParser->m_pBuf + pStreamParser->m_iOffset, len);
	pStreamParser->DecSize(pStreamParser, len);
	return XOS_TRUE;
}

static char *SkipBuffer(TStreamParser *pStreamParser, int len)
{
	XOS_BOOL rv;
	char *p;
	rv = pStreamParser->BeforeDecSize(pStreamParser, len);
	if (!rv)
	{
		return NULL;
	}
	p = pStreamParser->m_pBuf + pStreamParser->m_iOffset;
	pStreamParser->DecSize(pStreamParser, len);
	return p;
}

static XOS_BOOL BeforeDecSize(TStreamParser *pStreamParser, int iSize)
{
	assert(pStreamParser->m_pBuf);
	if ((pStreamParser->m_iSize - iSize) < 0)
	{
		return XOS_FALSE;
	}
	return XOS_TRUE;
}

static XOS_BOOL DecSize(TStreamParser *pStreamParser, int iSize)
{
	assert(pStreamParser->m_pBuf);
	pStreamParser->m_iSize -= iSize;
	pStreamParser->m_iOffset += iSize;
	return XOS_TRUE;
}

const TStreamParser STREAMPARSER =
{
	&Construct,
	&Destroy,
	&Create,

	&IsEnd,
	&GetCurPos,
	&GetLeftLen,
	&SkipLength,
	&SkipPAD2,
	&SkipPAD4,
	&SkipPAD8,
	&GetCHAR,
	&SkipCHAR,
	&GetUCHAR,
	&SkipUCHAR,
	&GetSHORT,
	&SkipSHORT,
	&GetUSHORT,
	&SkipUSHORT,
	&GetINT,
	&SkipINT,
	&GetUINT,
	&SkipUINT,
	&GetIP,
	&SkipIP,
	&GetGUID,
	&SkipGUID,
	&GetString,
	&SkipString,
	&GetBuffer,
	&SkipBuffer,

	&BeforeDecSize,
	&DecSize,
};
