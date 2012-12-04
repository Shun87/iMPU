/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: MiniList.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-11-12 10:01:18
 *  修正备注: 
 *  
 */

#include "../MiniList.h"
#include <stdio.h>
#include <stdlib.h>
//#include <malloc.h>

void MiniList_Init(TMiniList *pList)
{
	pList->pHead = NULL;
	pList->pTail = NULL;
	pList->iLength = 0;
}

void MiniList_AddTail(TMiniList *pList, TMiniListNode *pNode)
{
	pNode->pNext = NULL;
	if (pList->pHead == NULL)
	{
		pList->pHead = pNode;
		pList->pTail = pNode;
	}
	else
	{
		pList->pTail->pNext = pNode;
		pList->pTail = pNode;
	}
	pList->iLength ++;
}

TMiniListNode * MiniList_ShowHead(TMiniList *pList)
{
	return pList->pHead;
}

TMiniListNode * MiniList_RemoveHead(TMiniList *pList)
{
	TMiniListNode *pNode;
	pNode = pList->pHead;
	if (pNode == NULL)
	{
		return NULL;
	}
	if (pList->pHead == pList->pTail)
	{
		pList->pHead = NULL;
		pList->pTail = NULL;
	}
	else
	{
		pList->pHead = pList->pHead->pNext;
	}
	pList->iLength --;
	return pNode;
}

char MiniList_IsEmpty(TMiniList *pList)
{
	return (pList->pHead == NULL);
}

int MiniList_Length(TMiniList *pList)
{
	return pList->iLength;
}

void MiniList_Clear(TMiniList *pList)
{
	while (pList->pHead != NULL)
	{
		TMiniListNode *pNode;
		pNode = pList->pHead;
		pList->pHead = pList->pHead->pNext;
		free(pNode);
	}
	pList->pTail = NULL;
	pList->iLength = 0;
}
