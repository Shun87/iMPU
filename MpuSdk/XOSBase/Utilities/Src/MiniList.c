/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: MiniList.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-11-12 10:01:18
 *  ������ע: 
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
