/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: MemBlock.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: zhaoqq
 *  �������: 2007-01-05 15:21:48
 *  ������ע: 
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
