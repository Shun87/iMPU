/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: MemBlock.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: zhaoqq
 *  完成日期: 2007-01-05 15:21:48
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../MemBlock.h"

static void Construct(TMemBlock *cthis)
{
	cthis->m_pBuffer = NULL;
	cthis->m_iSize = 0;
	cthis->m_iMaxSize = 0;
}

static void Destroy(TMemBlock *cthis)
{
	if (cthis->m_pBuffer != NULL)
	{
		free(cthis->m_pBuffer);
	}
}

static XOS_BOOL Create(TMemBlock *cthis, int iSize)
{
	cthis->m_iSize = iSize;
	cthis->m_iMaxSize = iSize;
	cthis->m_pBuffer = (char *)malloc(cthis->m_iSize);
	if (cthis->m_pBuffer == NULL)
	{
		return XOS_FALSE;
	}
	return XOS_TRUE;
}

static char *GetBuffer(TMemBlock *cthis)
{
	return cthis->m_pBuffer;
}

static int GetSize(TMemBlock *cthis)
{
	return cthis->m_iSize;
}

static XOS_BOOL SetSize(TMemBlock *cthis, int iSize)
{
	if (iSize > cthis->m_iMaxSize)
	{
		return XOS_FALSE;
	}
	cthis->m_iSize = iSize;
	return XOS_TRUE;
}

const TMemBlock MEMBLOCK =
{
	&Construct,
	&Destroy,
	&Create,

	&GetBuffer,
	&GetSize,
	&SetSize
};
