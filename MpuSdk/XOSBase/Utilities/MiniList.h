/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: MiniList.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ����������,����������Ϳɱ�,ֻҪ��һ����Ա����һ����ָ��Ϳ�����
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-11-12 9:55:25
 *  ������ע: 
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

/*  ��������: ��ʼ�� 
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 */
void MiniList_Init(TMiniList *pList);

/*  ��������: ���β���
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 *      TMiniListNode *pNode [IN]:	��ӵĽ��ָ��
 */
void MiniList_AddTail(TMiniList *pList, TMiniListNode *pNode);

/*  ��������: ���ͷ��� 
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 *  ����ֵ: ͷ���ָ��,NULL��ʾ�����
 */
TMiniListNode * MiniList_ShowHead(TMiniList *pList);

/*  ��������: �Ƴ�ͷ���
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 *  ����ֵ: �Ƴ���ͷ���ָ��,NULL��ʾ�����
 */
TMiniListNode * MiniList_RemoveHead(TMiniList *pList);

/*  ��������: �ж��Ƿ��
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 *  ����ֵ: 1��ʾ��,0��ʾ�ǿ� 
 */
char MiniList_IsEmpty(TMiniList *pList);

/*  ��������: ��ȡ������
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 *  ����ֵ: ����ĳ���
 */
int MiniList_Length(TMiniList *pList);

/*  ��������: �������,�ͷ�����������
 *  ����˵��:
 *      TMiniList *pList [IN]:		�������ݽṹָ��
 *  ����ֵ: void 
 *  ��ע:	
 */
void MiniList_Clear(TMiniList *pList);

#ifdef __cplusplus
}
#endif

#endif //__MINILIST_H__
