/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: ObjList.h
 *  文件标识: 
 *  摘    要: 伪C类对象链表
 *			  链表中可以为每个结点指定一个特征数据
 *			  可以使用这个特征数据进行查找和比较等操作
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-29 16:43:38
 *  修正备注: 
 *  
 */

#ifndef __OBJLIST_H__
#define __OBJLIST_H__

//列表结点的特征数据类型
typedef enum {
	OBJLISTNODE_DATATYPE_NODEF=0, 
	OBJLISTNODE_DATATYPE_BINARY, 
	OBJLISTNODE_DATATYPE_STRING
} TObjListNodeDataType;

//////////////////////////////////////////////////////////////////////////
//	伪C类对象链表结点类
//////////////////////////////////////////////////////////////////////////

typedef struct _TObjListNode TObjListNode;
struct _TObjListNode
{
//public:
	void (*Construct)(TObjListNode *pObjListNode);
	void (*Destroy)(TObjListNode *pObjListNode);

	/*  功能描述: 创建链表结点
	 *  参数说明:
	 *      TObjListNodeDataType dataType [IN]:	结点特征数据类型,见前面的enum定义
	 *      const void *pData [IN]:				结点特征数据指针
	 *											当dataType为OBJLISTNODE_DATATYPE_STRING和
	 *												OBJLISTNODE_DATATYPE_STRING的时候有效
	 *											当dataType为OBJLISTNODE_DATATYPE_NODEF传入NULL
	 *      int iDataLen [IN]:					结点特征数据长度
	 *											仅在dataType为OBJLISTNODE_DATATYPE_BINARY时有效
	 *											当dataType为OBJLISTNODE_DATATYPE_STRING的时候,iDataLen传1.
	 *											当dataType为OBJLISTNODE_DATATYPE_NODEF的时候,iDataLen传0.
	 */
	XOS_BOOL (*Create)(TObjListNode *pObjListNode, 
						TObjListNodeDataType dataType, const void *pData, int iDataLen);

	/*  功能描述: 获取结点指向的对象指针
	 *  返回值: 结点指向的对象指针
	 */
	void* (*GetObj)(TObjListNode *pObjListNode);

	/*  功能描述: 设置结点指向的对象指针
	 *  参数说明:
	 *      void *pObj [IN]:	新设置的对象指针
	 */
	void (*SetObj)(TObjListNode *pObjListNode, void *pObj);

	/*  功能描述: 比较结点的特征数据
	 *  参数说明:
	 *      const void *pData [IN]:	要比较的特征数据
	 *  返回值: XOS_TRUE表示相同,否则不同 
	 *  备注: 特征数据类型必须一致	
	 */
	XOS_BOOL (*CompareData)(TObjListNode *pObjListNode, const void *pData);

	/*  功能描述: 拷贝结点的特征数据
	 *  参数说明:
	 *      void *pData [OUT]:	存放拷贝后的特征数据的缓冲
	 *      int iDataLen [IN]:	缓冲的长度
	 *  备注: 目前仅支持OBJLISTNODE_DATATYPE_BINARY类型的特征数据	
	 */
	XOS_BOOL (*CopyData)(TObjListNode *pObjListNode, void *pData, int iDataLen);

	TObjListNode *m_pNext;	//结点的下一个结点

//private:
	char *m_pData;		//结点的特征数据存放缓冲地址
	int m_iDataLen;		//特殊数据长度
	TObjListNodeDataType m_dataType;	//特征数据类型
	void *m_pObj;		//结点的对象指针
};

//////////////////////////////////////////////////////////////////////////
//	伪C类链表类
//////////////////////////////////////////////////////////////////////////

typedef struct _TObjList TObjList;
struct _TObjList
{
//public:
	void (*Construct)(TObjList *pObjList);
	void (*Destroy)(TObjList *pObjList);
	
	/*  功能描述: 创建对象链表
	 *  参数说明:
	 *      TObjListNodeDataType dataType [IN]:	链表特征数据类型,见前面的enum定义
	 *      int iDataLen [IN]:	特征数据长度
	 *							仅在dataType为OBJLISTNODE_DATATYPE_BINARY时有效
	 *							当dataType为OBJLISTNODE_DATATYPE_STRING的时候,iDataLen传1.
	 *							当dataType为OBJLISTNODE_DATATYPE_NODEF的时候,iDataLen传0.
	 *	备注: 同一个链表中的所有结点的特征数据必须一致
	 */
	XOS_BOOL (*Create)(TObjList *pObjList, TObjListNodeDataType dataType, int iDataLen);

	/*  功能描述: 获取链表的长度
	 *  返回值: 链表的长度
	 */
	int (*Length)(TObjList *pObjList);

	/*  功能描述: 添加节点对象
	 *  参数说明:
	 *      void *pObj [IN]:		要添加的伪C对象
	 *      const void *pData [IN]: 对象的特征数据
	 */
	XOS_BOOL (*AddTail)(TObjList *pObjList, void *pObj, const void *pData);

	/*  功能描述: 移除链表头节点对象
	 *  返回值: 移除的伪C对象指针,NULL表示失败
	 */
	void* (*RemoveHead)(TObjList *pObjList);

	/*  功能描述: 移除链表头节点对象,并且带出来特征数据
	 *  参数说明:
	 *      void *pData [OUT]:	带出来的结点数据
	 *      int iDataLen [IN]:	结点数据的长度
	 *  返回值: 移除的伪C对象指针,NULL表示失败
	 *  备注: 目前仅支持OBJLISTNODE_DATATYPE_BINARY类型的特征数据
	 */
	void* (*RemoveHeadWithData)(TObjList *pObjList, void *pData, int iDataLen);
	//获取头节点对象

	/*  功能描述: 获取链表头节点对象
	 *  返回值: 获取的链表头节点对象指针,NULL表示失败
	 */
	void* (*GetHead)(TObjList *pObjList);

	/*  功能描述: 获取链表头节点对象,并且带出来特征数据
	 *  参数说明:
	 *      void *pData [OUT]:	带出来的结点数据
	 *      int iDataLen [IN]:	结点数据的长度
	 *  返回值: 获取的链表头节点对象指针,NULL表示失败
	 *  备注: 目前仅支持OBJLISTNODE_DATATYPE_BINARY类型的特征数据
	 */
	void* (*GetHeadWithData)(TObjList *pObjList, void *pData, int iDataLen);

	/*  功能描述: 获取下一个节点对象
	 *  参数说明:
	 *      XOS_BOOL bHead [IN]:	是否获取头结点,否则获取下一个结点
	 *  返回值: 获取的节点对象指针,NULL表示失败
	 *  备注: 遍历链表用,第一次调用传入XOS_TRUE,以后传入XOS_FALSE,
	 *		  直到返回NULL,表示遍历完毕
	 *		  在遍历的过程中,可以对链表结点进行删除操作
	 */
	void* (*FindNextObj)(TObjList *pObjList, XOS_BOOL bHead);

	/*  功能描述: 使用节点对象指针查找节点对象
	 *  参数说明:
	 *      const void * pObj [IN]:	用来从查找的节点对象指针
	 *  返回值: 找到的节点对象指针,NULL表示失败
	 *  备注: 一般用来判断某个对象是否在链表中
	 */
	void* (*FindObj)(TObjList *pObjList, const void *pObj);
	
	/*  功能描述: 使用特征数据查找节点对象
	 *  参数说明:
	 *      const void *pData [IN]:	用来查找的特征数据
	 *  返回值: 找到的节点对象指针,NULL表示失败
	 */
	void* (*FindObjByData)(TObjList *pObjList, const void *pData);

	/*  功能描述: 使用序号索引查找节点对象
	 *  参数说明:
	 *      int iIdx [IN]:	用来查找的序号索引
	 *  返回值: 找到的节点对象指针,NULL表示失败
	 */
	void* (*FindObjByIdx)(TObjList *pObjList, int iIdx);

	/*  功能描述: 移除指定节点
	 *  参数说明:
	 *      const void *pObj [IN]:	要移除的结点对象指针
	 */
	void (*RemoveObj)(TObjList *pObjList, const void *pObj);

	/*  功能描述: 移除指定特征数据的节点
	 *  参数说明:
	 *      const void *pData [IN]:	要移除的节点的特征数据
	 *  返回值: 移除的伪C对象指针,NULL表示失败
	 */
	void* (*RemoveObjByData)(TObjList *pObjList, const void *pData);

	/*  功能描述: 移除指定序号索引的节点
	 *  参数说明:
	 *      int iIdx [IN]:	要移除的节点的的序号索引
	 *  返回值: 移除的伪C对象指针,NULL表示失败
	 */
	void* (*RemoveObjByIdx)(TObjList *pObjList, int iIdx);

	/*  功能描述: 移除并释放指定节点
	 *  参数说明:
	 *      void *pObj [IN]:	要移除并释放的结点对象指针
	 */
	void (*DeleteObj)(TObjList *pObjList, void *pObj);

	/*  功能描述: 移除并释放指定特征数据的节点
	 *  参数说明:
	 *      const void *pData [IN]:	要移除并释放的节点的特征数据
	 */
	void (*DeleteObjByData)(TObjList *pObjList, const void *pData);

	/*  功能描述:  移除并释放指定序号索引的节点
	 *  参数说明:
	 *      TObjList *pObjList []:
	 *      int iIdx [IN]:	要移除并释放的节点的序号索引
	 */
	void (*DeleteObjByIdx)(TObjList *pObjList, int iIdx);

	/*  功能描述: 移除并且释放所有节点
	 */
	void (*Clear)(TObjList *pObjList);

//private:
	void (*RemoveNode)(TObjList *pObjList, TObjListNode *pPrevNode, TObjListNode *pNode);
	//移除头节点
	TObjListNode* (*RemoveHeadNode)(TObjList *pObjList);
	//添加尾节点
	void (*AddTailNode)(TObjList *pObjList, TObjListNode *pNode);

	int m_iLength;
	TObjListNode *m_pHeadNode;
	TObjListNode *m_pTailNode;
	TObjListNode *m_pFindNode;
	TObjListNodeDataType m_dataType;
	int m_iDataLen;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TObjListNode OBJLISTNODE;
extern const TObjList OBJLIST;

#ifdef __cplusplus
}
#endif

#endif //__OBJLIST_H__
