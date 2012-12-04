/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: LoopBuffer.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-11-10 14:14:31
 *  修正备注: 
 *  
 */

/*
	-----------------------------------------------
	|			loop buffer						  |	
	-----------------------------------------------
	^			^
	|		    |
	pRead	   pWrite
*/

#include "../../XOS/XOS.h"
#include "../LoopBuffer.h"

static void Construct(TLoopBuffer *pLoopBuffer)
{
	pLoopBuffer->m_pBuf = NULL;
	pLoopBuffer->m_uiBufferLen = 0;
	pLoopBuffer->m_pRead = NULL;
	pLoopBuffer->m_pWrite = NULL;
}

static void Destroy(TLoopBuffer *pLoopBuffer)
{
	if (pLoopBuffer->m_pBuf != NULL)
	{
		free(pLoopBuffer->m_pBuf);
	}
}

static XOS_BOOL Create(TLoopBuffer *pLoopBuffer, XOS_U32 uiBufferLen)
{
	pLoopBuffer->m_pBuf = (char *)malloc(uiBufferLen);
	if (pLoopBuffer->m_pBuf == NULL)
	{
		return XOS_FALSE;
	}
	pLoopBuffer->m_uiBufferLen = uiBufferLen;
	pLoopBuffer->m_pRead = pLoopBuffer->m_pBuf;
	pLoopBuffer->m_pWrite = pLoopBuffer->m_pBuf;
	return XOS_TRUE;
}

static XOS_U32 WriteData(TLoopBuffer *pLoopBuffer, char *pData, XOS_U32 uiDataLen, XOS_BOOL bCover)
{
	XOS_U32 i;
	for (i=0; i<uiDataLen; i++)
	{
		pLoopBuffer->m_pWrite[0] = pData[i];
		pLoopBuffer->m_pWrite = pLoopBuffer->pAdd(pLoopBuffer, pLoopBuffer->m_pWrite);
		if (pLoopBuffer->m_pWrite == pLoopBuffer->m_pRead)
		{
			if (!bCover)
			{
				return i;
			}
			else
			{
				pLoopBuffer->m_pRead = pLoopBuffer->pAdd(pLoopBuffer, pLoopBuffer->m_pRead);
			}
		}
	}
	return i;
}

static XOS_BOOL ReadData(TLoopBuffer *pLoopBuffer, char *pData, XOS_U32 *puiDataLen)
{
	XOS_U32 uiDataLen, uiLeftDataLen;
	uiLeftDataLen = *puiDataLen;
	uiDataLen = 0;
	while ((uiLeftDataLen > 0) && (pLoopBuffer->m_pRead != pLoopBuffer->m_pWrite))
	{
		pData[0] = pLoopBuffer->m_pRead[0];
		pData ++;
		pLoopBuffer->m_pRead = pLoopBuffer->pAdd(pLoopBuffer, pLoopBuffer->m_pRead);
		uiLeftDataLen --;
		uiDataLen ++;
	}
	*puiDataLen = uiDataLen;
	return (uiDataLen != 0);
}

static char* pAdd(TLoopBuffer *pLoopBuffer, char *p)
{
	p++;
	if (p >= (pLoopBuffer->m_pBuf+pLoopBuffer->m_uiBufferLen))
	{
		p = pLoopBuffer->m_pBuf;
	}
	return p;
}

const TLoopBuffer LOOPBUFFER =
{
	&Construct,
	&Destroy,
	&Create,

	&WriteData,
	&ReadData,
	&pAdd
};
