/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: ObjList.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: αC���������
 *			  �����п���Ϊÿ�����ָ��һ����������
 *			  ����ʹ������������ݽ��в��ҺͱȽϵȲ���
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-09-29 16:43:38
 *  ������ע: 
 *  
 */

#ifndef __OBJLIST_H__
#define __OBJLIST_H__

//�б����������������
typedef enum {
	OBJLISTNODE_DATATYPE_NODEF=0, 
	OBJLISTNODE_DATATYPE_BINARY, 
	OBJLISTNODE_DATATYPE_STRING
} TObjListNodeDataType;

//////////////////////////////////////////////////////////////////////////
//	αC�������������
//////////////////////////////////////////////////////////////////////////

typedef struct _TObjListNode TObjListNode;
struct _TObjListNode
{
//public:
	void (*Construct)(TObjListNode *pObjListNode);
	void (*Destroy)(TObjListNode *pObjListNode);

	/*  ��������: ����������
	 *  ����˵��:
	 *      TObjListNodeDataType dataType [IN]:	���������������,��ǰ���enum����
	 *      const void *pData [IN]:				�����������ָ��
	 *											��dataTypeΪOBJLISTNODE_DATATYPE_STRING��
	 *												OBJLISTNODE_DATATYPE_STRING��ʱ����Ч
	 *											��dataTypeΪOBJLISTNODE_DATATYPE_NODEF����NULL
	 *      int iDataLen [IN]:					����������ݳ���
	 *											����dataTypeΪOBJLISTNODE_DATATYPE_BINARYʱ��Ч
	 *											��dataTypeΪOBJLISTNODE_DATATYPE_STRING��ʱ��,iDataLen��1.
	 *											��dataTypeΪOBJLISTNODE_DATATYPE_NODEF��ʱ��,iDataLen��0.
	 */
	XOS_BOOL (*Create)(TObjListNode *pObjListNode, 
						TObjListNodeDataType dataType, const void *pData, int iDataLen);

	/*  ��������: ��ȡ���ָ��Ķ���ָ��
	 *  ����ֵ: ���ָ��Ķ���ָ��
	 */
	void* (*GetObj)(TObjListNode *pObjListNode);

	/*  ��������: ���ý��ָ��Ķ���ָ��
	 *  ����˵��:
	 *      void *pObj [IN]:	�����õĶ���ָ��
	 */
	void (*SetObj)(TObjListNode *pObjListNode, void *pObj);

	/*  ��������: �ȽϽ�����������
	 *  ����˵��:
	 *      const void *pData [IN]:	Ҫ�Ƚϵ���������
	 *  ����ֵ: XOS_TRUE��ʾ��ͬ,����ͬ 
	 *  ��ע: �����������ͱ���һ��	
	 */
	XOS_BOOL (*CompareData)(TObjListNode *pObjListNode, const void *pData);

	/*  ��������: ����������������
	 *  ����˵��:
	 *      void *pData [OUT]:	��ſ�������������ݵĻ���
	 *      int iDataLen [IN]:	����ĳ���
	 *  ��ע: Ŀǰ��֧��OBJLISTNODE_DATATYPE_BINARY���͵���������	
	 */
	XOS_BOOL (*CopyData)(TObjListNode *pObjListNode, void *pData, int iDataLen);

	TObjListNode *m_pNext;	//������һ�����

//private:
	char *m_pData;		//�����������ݴ�Ż����ַ
	int m_iDataLen;		//�������ݳ���
	TObjListNodeDataType m_dataType;	//������������
	void *m_pObj;		//���Ķ���ָ��
};

//////////////////////////////////////////////////////////////////////////
//	αC��������
//////////////////////////////////////////////////////////////////////////

typedef struct _TObjList TObjList;
struct _TObjList
{
//public:
	void (*Construct)(TObjList *pObjList);
	void (*Destroy)(TObjList *pObjList);
	
	/*  ��������: ������������
	 *  ����˵��:
	 *      TObjListNodeDataType dataType [IN]:	����������������,��ǰ���enum����
	 *      int iDataLen [IN]:	�������ݳ���
	 *							����dataTypeΪOBJLISTNODE_DATATYPE_BINARYʱ��Ч
	 *							��dataTypeΪOBJLISTNODE_DATATYPE_STRING��ʱ��,iDataLen��1.
	 *							��dataTypeΪOBJLISTNODE_DATATYPE_NODEF��ʱ��,iDataLen��0.
	 *	��ע: ͬһ�������е����н����������ݱ���һ��
	 */
	XOS_BOOL (*Create)(TObjList *pObjList, TObjListNodeDataType dataType, int iDataLen);

	/*  ��������: ��ȡ����ĳ���
	 *  ����ֵ: ����ĳ���
	 */
	int (*Length)(TObjList *pObjList);

	/*  ��������: ��ӽڵ����
	 *  ����˵��:
	 *      void *pObj [IN]:		Ҫ��ӵ�αC����
	 *      const void *pData [IN]: �������������
	 */
	XOS_BOOL (*AddTail)(TObjList *pObjList, void *pObj, const void *pData);

	/*  ��������: �Ƴ�����ͷ�ڵ����
	 *  ����ֵ: �Ƴ���αC����ָ��,NULL��ʾʧ��
	 */
	void* (*RemoveHead)(TObjList *pObjList);

	/*  ��������: �Ƴ�����ͷ�ڵ����,���Ҵ�������������
	 *  ����˵��:
	 *      void *pData [OUT]:	�������Ľ������
	 *      int iDataLen [IN]:	������ݵĳ���
	 *  ����ֵ: �Ƴ���αC����ָ��,NULL��ʾʧ��
	 *  ��ע: Ŀǰ��֧��OBJLISTNODE_DATATYPE_BINARY���͵���������
	 */
	void* (*RemoveHeadWithData)(TObjList *pObjList, void *pData, int iDataLen);
	//��ȡͷ�ڵ����

	/*  ��������: ��ȡ����ͷ�ڵ����
	 *  ����ֵ: ��ȡ������ͷ�ڵ����ָ��,NULL��ʾʧ��
	 */
	void* (*GetHead)(TObjList *pObjList);

	/*  ��������: ��ȡ����ͷ�ڵ����,���Ҵ�������������
	 *  ����˵��:
	 *      void *pData [OUT]:	�������Ľ������
	 *      int iDataLen [IN]:	������ݵĳ���
	 *  ����ֵ: ��ȡ������ͷ�ڵ����ָ��,NULL��ʾʧ��
	 *  ��ע: Ŀǰ��֧��OBJLISTNODE_DATATYPE_BINARY���͵���������
	 */
	void* (*GetHeadWithData)(TObjList *pObjList, void *pData, int iDataLen);

	/*  ��������: ��ȡ��һ���ڵ����
	 *  ����˵��:
	 *      XOS_BOOL bHead [IN]:	�Ƿ��ȡͷ���,�����ȡ��һ�����
	 *  ����ֵ: ��ȡ�Ľڵ����ָ��,NULL��ʾʧ��
	 *  ��ע: ����������,��һ�ε��ô���XOS_TRUE,�Ժ���XOS_FALSE,
	 *		  ֱ������NULL,��ʾ�������
	 *		  �ڱ����Ĺ�����,���Զ����������ɾ������
	 */
	void* (*FindNextObj)(TObjList *pObjList, XOS_BOOL bHead);

	/*  ��������: ʹ�ýڵ����ָ����ҽڵ����
	 *  ����˵��:
	 *      const void * pObj [IN]:	�����Ӳ��ҵĽڵ����ָ��
	 *  ����ֵ: �ҵ��Ľڵ����ָ��,NULL��ʾʧ��
	 *  ��ע: һ�������ж�ĳ�������Ƿ���������
	 */
	void* (*FindObj)(TObjList *pObjList, const void *pObj);
	
	/*  ��������: ʹ���������ݲ��ҽڵ����
	 *  ����˵��:
	 *      const void *pData [IN]:	�������ҵ���������
	 *  ����ֵ: �ҵ��Ľڵ����ָ��,NULL��ʾʧ��
	 */
	void* (*FindObjByData)(TObjList *pObjList, const void *pData);

	/*  ��������: ʹ������������ҽڵ����
	 *  ����˵��:
	 *      int iIdx [IN]:	�������ҵ��������
	 *  ����ֵ: �ҵ��Ľڵ����ָ��,NULL��ʾʧ��
	 */
	void* (*FindObjByIdx)(TObjList *pObjList, int iIdx);

	/*  ��������: �Ƴ�ָ���ڵ�
	 *  ����˵��:
	 *      const void *pObj [IN]:	Ҫ�Ƴ��Ľ�����ָ��
	 */
	void (*RemoveObj)(TObjList *pObjList, const void *pObj);

	/*  ��������: �Ƴ�ָ���������ݵĽڵ�
	 *  ����˵��:
	 *      const void *pData [IN]:	Ҫ�Ƴ��Ľڵ����������
	 *  ����ֵ: �Ƴ���αC����ָ��,NULL��ʾʧ��
	 */
	void* (*RemoveObjByData)(TObjList *pObjList, const void *pData);

	/*  ��������: �Ƴ�ָ����������Ľڵ�
	 *  ����˵��:
	 *      int iIdx [IN]:	Ҫ�Ƴ��Ľڵ�ĵ��������
	 *  ����ֵ: �Ƴ���αC����ָ��,NULL��ʾʧ��
	 */
	void* (*RemoveObjByIdx)(TObjList *pObjList, int iIdx);

	/*  ��������: �Ƴ����ͷ�ָ���ڵ�
	 *  ����˵��:
	 *      void *pObj [IN]:	Ҫ�Ƴ����ͷŵĽ�����ָ��
	 */
	void (*DeleteObj)(TObjList *pObjList, void *pObj);

	/*  ��������: �Ƴ����ͷ�ָ���������ݵĽڵ�
	 *  ����˵��:
	 *      const void *pData [IN]:	Ҫ�Ƴ����ͷŵĽڵ����������
	 */
	void (*DeleteObjByData)(TObjList *pObjList, const void *pData);

	/*  ��������:  �Ƴ����ͷ�ָ����������Ľڵ�
	 *  ����˵��:
	 *      TObjList *pObjList []:
	 *      int iIdx [IN]:	Ҫ�Ƴ����ͷŵĽڵ���������
	 */
	void (*DeleteObjByIdx)(TObjList *pObjList, int iIdx);

	/*  ��������: �Ƴ������ͷ����нڵ�
	 */
	void (*Clear)(TObjList *pObjList);

//private:
	void (*RemoveNode)(TObjList *pObjList, TObjListNode *pPrevNode, TObjListNode *pNode);
	//�Ƴ�ͷ�ڵ�
	TObjListNode* (*RemoveHeadNode)(TObjList *pObjList);
	//���β�ڵ�
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
