/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: ObjList.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-29 17:15:04
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../ObjList.h"

//////////////////////////////////////////////////////////////////////
// TObjListNode
//////////////////////////////////////////////////////////////////////

static void Construct1(TObjListNode *pObjListNode)
{
	pObjListNode->m_pData = NULL;
	pObjListNode->m_dataType = OBJLISTNODE_DATATYPE_NODEF;
	pObjListNode->m_iDataLen = 0;
	pObjListNode->m_pObj = NULL;
}

static void Destroy1(TObjListNode *pObjListNode)
{
	if (pObjListNode->m_pData != 0)
	{
		free(pObjListNode->m_pData);
	}
}

static XOS_BOOL Create1(TObjListNode *pObjListNode, 
					TObjListNodeDataType dataType, const void *pData, int iDataLen)
{
	if ((pData != 0) && (iDataLen != 0) && (dataType != OBJLISTNODE_DATATYPE_NODEF))
	{
		pObjListNode->m_dataType = dataType;
		pObjListNode->m_iDataLen = iDataLen;
		if (pObjListNode->m_dataType == OBJLISTNODE_DATATYPE_BINARY)
		{
			pObjListNode->m_pData = (char *)malloc(pObjListNode->m_iDataLen);
			if (pObjListNode->m_pData == NULL)
			{
				return XOS_FALSE;
			}
			memcpy(pObjListNode->m_pData, pData, pObjListNode->m_iDataLen);
		}
		else if (pObjListNode->m_dataType == OBJLISTNODE_DATATYPE_STRING)
		{
			pObjListNode->m_pData = (char *)malloc(strlen(pData)+1);
			if (pObjListNode->m_pData == NULL)
			{
				return XOS_FALSE;
			}
			memset(pObjListNode->m_pData, 0, strlen(pData)+1);
			strcpy(pObjListNode->m_pData, (const char *)pData);
		}
	}
	return XOS_TRUE;
}

static void* GetObj(TObjListNode *pObjListNode)
{
	return pObjListNode->m_pObj;
}

static void SetObj(TObjListNode *pObjListNode, void *pObj)
{
	pObjListNode->m_pObj = pObj;
}

static XOS_BOOL CompareData(TObjListNode *pObjListNode, const void *pData)
{
	if (pObjListNode->m_dataType == OBJLISTNODE_DATATYPE_BINARY)
	{
		return (memcmp(pObjListNode->m_pData, pData, pObjListNode->m_iDataLen) == 0);
	}
	else if (pObjListNode->m_dataType == OBJLISTNODE_DATATYPE_STRING)
	{
		return (strcmp(pObjListNode->m_pData, (const char *)pData) == 0);
	}
	else
	{
		return XOS_FALSE;
	}
}

static XOS_BOOL CopyData(TObjListNode *pObjListNode, void *pData, int iDataLen)
{
	if (pObjListNode->m_pData == NULL)
	{
		return XOS_FALSE;
	}
	memcpy(pData, pObjListNode->m_pData, iDataLen);
	return XOS_TRUE;
}

const TObjListNode OBJLISTNODE =
{
	&Construct1,
	&Destroy1,
	&Create1,

	&GetObj,
	&SetObj,
	&CompareData,
	&CopyData
};

//////////////////////////////////////////////////////////////////////
// TObjList
//////////////////////////////////////////////////////////////////////

static void Construct2(TObjList *pObjList)
{
	pObjList->m_iLength = 0;
	pObjList->m_pHeadNode = NULL;
	pObjList->m_pTailNode = NULL;
	pObjList->m_dataType = OBJLISTNODE_DATATYPE_NODEF;
	pObjList->m_iDataLen = 0;
}

static void Destroy2(TObjList *pObjList)
{
	pObjList->Clear(pObjList);
}

static XOS_BOOL Create2(TObjList *pObjList, TObjListNodeDataType dataType, int iDataLen)
{
	pObjList->m_dataType = dataType;
	pObjList->m_iDataLen = iDataLen;
	return XOS_TRUE;
}

static void RemoveNode(TObjList *pObjList, TObjListNode *pPrevNode, TObjListNode *pNode)
{
	if (pPrevNode == pNode)
	{
		//头节点
		if (pObjList->m_pHeadNode == pObjList->m_pFindNode)
		{
			pObjList->m_pFindNode = NULL;
		}
		pObjList->m_pHeadNode = pObjList->m_pHeadNode->m_pNext;
		if (pObjList->m_pHeadNode == NULL)
		{
			//只剩一个节点了,移掉之后就没有节点了.
			pObjList->m_pTailNode = NULL;
		}
	}
	else
	{
		//中间节点和尾节点
		if (pNode == pObjList->m_pFindNode)
		{
			pObjList->m_pFindNode = pPrevNode;
		}
		pPrevNode->m_pNext = pNode->m_pNext;
		if (pPrevNode->m_pNext == NULL)
		{
			//尾节点
			pObjList->m_pTailNode = pPrevNode;
		}
	}
	pObjList->m_iLength --;
}

static TObjListNode* RemoveHeadNode(TObjList *pObjList)
{
	if (pObjList->m_pHeadNode == NULL)
	{
		return NULL;
	}
	else
	{
		TObjListNode *pNode = pObjList->m_pHeadNode;
		if (pNode == pObjList->m_pFindNode)
		{
			pObjList->m_pFindNode = NULL;
		}
		pObjList->m_pHeadNode = pObjList->m_pHeadNode->m_pNext;
		pObjList->m_iLength --;
		return pNode;
	}
}

static void AddTailNode(TObjList *pObjList, TObjListNode *pNode)
{
	if (pObjList->m_pHeadNode == NULL)
	{
		//空链表
		pObjList->m_pHeadNode = pNode;
		pObjList->m_pTailNode = pNode;
		pObjList->m_pTailNode->m_pNext = NULL;
	}
	else
	{
		//非空链表
		pObjList->m_pTailNode->m_pNext = pNode;
		pNode->m_pNext = NULL;
		pObjList->m_pTailNode = pNode;
	}
	pObjList->m_iLength ++;
}

static int Length(TObjList *pObjList)
{
	return pObjList->m_iLength;
}

static XOS_BOOL AddTail(TObjList *pObjList, void *pObj, const void *pData)
{
	TObjListNode *pNode = cnew(sizeof(TObjListNode), &OBJLISTNODE);
	if (pNode == NULL)
	{
		printf("new memory failed %s:%d\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	if (!pNode->Create(pNode, pObjList->m_dataType, pData, pObjList->m_iDataLen))
	{
		cdelete(pNode);
		return XOS_FALSE;
	}
	pNode->SetObj(pNode, pObj);

	pObjList->AddTailNode(pObjList, pNode);

	return XOS_TRUE;
}

static void* RemoveHead(TObjList *pObjList)
{
	void *pObj;
	TObjListNode *pNode = pObjList->RemoveHeadNode(pObjList);
	if (pNode == NULL)
	{
		return NULL;
	}
	pObj = pNode->GetObj(pNode);
	cdelete(pNode);
	return pObj;
}

static void* RemoveHeadWithData(TObjList *pObjList, void *pData, int iDataLen)
{
	void *pObj;
	TObjListNode *pNode = pObjList->RemoveHeadNode(pObjList);
	if (pNode == NULL)
	{
		return NULL;
	}
	pObj = pNode->GetObj(pNode);
	pNode->CopyData(pNode, pData, iDataLen);
	cdelete(pNode);
	return pObj;
}

static void* GetHead(TObjList *pObjList)
{
	if (pObjList->m_pHeadNode == NULL)
	{
		return NULL;
	}
	return pObjList->m_pHeadNode->GetObj(pObjList->m_pHeadNode);
}

static void* GetHeadWithData(TObjList *pObjList, void *pData, int iDataLen)
{
	if (pObjList->m_pHeadNode == NULL)
	{
		return NULL;
	}
	pObjList->m_pHeadNode->CopyData(pObjList->m_pHeadNode, pData, iDataLen);
	return pObjList->m_pHeadNode->GetObj(pObjList->m_pHeadNode);
}

static void* FindNextObj(TObjList *pObjList, XOS_BOOL bHead)
{
	if (bHead)
	{
		pObjList->m_pFindNode = pObjList->m_pHeadNode;
	}
	else
	{
		if (pObjList->m_pFindNode == NULL)
		{
			return NULL;
		}
		pObjList->m_pFindNode = pObjList->m_pFindNode->m_pNext;
	}

	if (pObjList->m_pFindNode == NULL)
	{
		return NULL;
	}

	return pObjList->m_pFindNode->GetObj(pObjList->m_pFindNode);
}

static void* FindObj(TObjList *pObjList, const void * pObj)
{
	TObjListNode *pNode = pObjList->m_pHeadNode;
	while (pNode != NULL)
	{
		if (pNode->GetObj(pNode) == pObj)
		{
			pObjList->m_pFindNode = pNode;
			return pNode->GetObj(pNode);
		}
		pNode = pNode->m_pNext;
	}
	return NULL;
}

static void* FindObjByData(TObjList *pObjList, const void *pData)
{
	TObjListNode *pNode = pObjList->m_pHeadNode;
	while (pNode != NULL)
	{
		if (pNode->CompareData(pNode, pData))
		{
			pObjList->m_pFindNode = pNode;
			return pNode->GetObj(pNode);
		}
		pNode = pNode->m_pNext;
	}
	return NULL;
}

static void* FindObjByIdx(TObjList *pObjList, int iIdx)
{
	int iCurIdx;
	TObjListNode *pNode;

	if (iIdx >= pObjList->m_iLength)
	{
		return NULL;
	}

	pNode = pObjList->m_pHeadNode;
	for (iCurIdx=0; iCurIdx<iIdx; iCurIdx++)
	{
		pObjList->m_pFindNode = pNode;
		pNode = pNode->m_pNext;
	}
	return pNode->GetObj(pNode);
}

static void RemoveObj(TObjList *pObjList, const void * pObj)
{
	TObjListNode *pPrevNode = pObjList->m_pHeadNode;
	TObjListNode *pNode = pObjList->m_pHeadNode;
	while (pNode != NULL)
	{
		if (pNode->GetObj(pNode) == pObj)
		{
			pObjList->RemoveNode(pObjList, pPrevNode, pNode);
			cdelete(pNode);
			return ;
		}
		pPrevNode = pNode;
		pNode = pNode->m_pNext;
	}
	return ;
}

static void* RemoveObjByData(TObjList *pObjList, const void *pData)
{
	TObjListNode *pPrevNode = pObjList->m_pHeadNode;
	TObjListNode *pNode = pObjList->m_pHeadNode;
	while (pNode != NULL)
	{
		if (pNode->CompareData(pNode, pData))
		{
			void *pObj;
			pObjList->RemoveNode(pObjList, pPrevNode, pNode);
			pObj = pNode->GetObj(pNode);
			cdelete(pNode);
			return pObj;
		}
		pPrevNode = pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}

static void* RemoveObjByIdx(TObjList *pObjList, int iIdx)
{
	void *pObj;
	int iCurIdx;
	TObjListNode *pNode;
	TObjListNode *pPrevNode;

	if (iIdx >= pObjList->m_iLength)
	{
		return NULL;
	}

	pNode = pObjList->m_pHeadNode;
	pPrevNode = pObjList->m_pHeadNode;
	for (iCurIdx=0; iCurIdx<iIdx; iCurIdx++)
	{
		pPrevNode = pNode;
		pNode = pNode->m_pNext;
	}

	pObjList->RemoveNode(pObjList, pPrevNode, pNode);
	pObj = pNode->GetObj(pNode);
	cdelete(pNode);
	return pObj;
}

static void DeleteObj(TObjList *pObjList, void * pObj)
{
	pObjList->RemoveObj(pObjList, pObj);
	cdelete(pObj);
}

static void DeleteObjByData(TObjList *pObjList, const void *pData)
{
	void *pObj = pObjList->RemoveObjByData(pObjList, pData);
	if (pObj != NULL)
	{
		cdelete(pObj);
	}
}

static void DeleteObjByIdx(TObjList *pObjList, int iIdx)
{
	void *pObj = pObjList->RemoveObjByIdx(pObjList, iIdx);
	if (pObj != NULL)
	{
		cdelete(pObj);
	}
}

static void Clear(TObjList *pObjList)
{
	TObjListNode *pNode;
	while ((pNode = pObjList->RemoveHeadNode(pObjList)) != NULL)
	{
		void *pObj = pNode->GetObj(pNode);
		cdelete(pObj);
		cdelete(pNode);
	}
}

const TObjList OBJLIST =
{
	&Construct2,
	&Destroy2,
	&Create2,

	&Length,
	&AddTail,
	&RemoveHead,
	&RemoveHeadWithData,
	&GetHead,
	&GetHeadWithData,
	&FindNextObj,
	&FindObj,
	&FindObjByData,
	&FindObjByIdx,
	&RemoveObj,
	&RemoveObjByData,
	&RemoveObjByIdx,
	&DeleteObj,
	&DeleteObjByData,
	&DeleteObjByIdx,
	&Clear,

	&RemoveNode,
	&RemoveHeadNode,
	&AddTailNode
};
