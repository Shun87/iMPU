/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: MiniList.h
 *  文件标识: 
 *  摘    要: 迷你型链表,结点数据类型可变,只要第一个成员是下一个的指针就可以了
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-11-12 9:55:25
 *  修正备注: 
 *  
 */

#ifndef __MINILIST_H__
#define __MINILIST_H__

typedef struct _TMiniListNode
{
	struct _TMiniListNode *pNext;
} TMiniListNode;

typedef struct _TMiniList
{
	TMiniListNode *pHead;
	TMiniListNode *pTail;
	int iLength;
} TMiniList;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 初始化 
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 */
void MiniList_Init(TMiniList *pList);

/*  功能描述: 添加尾结点
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 *      TMiniListNode *pNode [IN]:	添加的结点指针
 */
void MiniList_AddTail(TMiniList *pList, TMiniListNode *pNode);

/*  功能描述: 获得头结点 
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 *  返回值: 头结点指针,NULL表示链表空
 */
TMiniListNode * MiniList_ShowHead(TMiniList *pList);

/*  功能描述: 移除头结点
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 *  返回值: 移除的头结点指针,NULL表示链表空
 */
TMiniListNode * MiniList_RemoveHead(TMiniList *pList);

/*  功能描述: 判断是否空
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 *  返回值: 1表示空,0表示非空 
 */
char MiniList_IsEmpty(TMiniList *pList);

/*  功能描述: 获取链表长度
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 *  返回值: 链表的长度
 */
int MiniList_Length(TMiniList *pList);

/*  功能描述: 清除链表,释放所有链表结点
 *  参数说明:
 *      TMiniList *pList [IN]:		对象数据结构指针
 *  返回值: void 
 *  备注:	
 */
void MiniList_Clear(TMiniList *pList);

#ifdef __cplusplus
}
#endif

#endif //__MINILIST_H__
