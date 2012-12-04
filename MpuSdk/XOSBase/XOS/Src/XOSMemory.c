/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: OSMemory.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-11-07 09:51:58
 *  ������ע: 
 *  
 */

#include <stdlib.h>
#include <stdio.h>
void *mymalloc(int size)
{
	static int s_iTotalMalloc = 0;
	s_iTotalMalloc += size;
	printf("mymalloc size:%d/%d\r\n", size, s_iTotalMalloc);
	return malloc(size);
}

#include "../XOS.h"

typedef struct _TObj
{
	void (*Construct)(struct _TObj* pObj);
	void (*Destroy)(struct _TObj* pObj);
} TObj;

void *cnew(int size, const void *def)
{
	void *p = malloc(size);
	if (p == 0)
	{
		return 0;
	}
	else
	{
		TObj *pObj = (TObj *)p;
		memcpy(p, def, size);
		pObj->Construct(pObj);
		return p;
	}
}

void cdelete(void *p)
{
	if (p != 0)
	{
		TObj *pObj = (TObj *)p;
		pObj->Destroy(pObj);
		free(p);
	}
}
