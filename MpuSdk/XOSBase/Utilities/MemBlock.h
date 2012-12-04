/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: MemBlock.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ʵ����һ��αC���ڴ����	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: zhaoqq
 *  �������: 2007-01-05 15:11:59
 *  ������ע: 
 *  
 */

#ifndef __MEMBLOCK_H__
#define __MEMBLOCK_H__

typedef struct _TMemBlock TMemBlock;
struct _TMemBlock  
{
//public:
	void (*Construct)(TMemBlock *pMemBlock);
	void (*Destroy)(TMemBlock *pMemBlock);

	/*  ��������: �����ڴ�����
	 *  ����˵��:
	 *      int iSize [IN]:			�ڴ���С
	 *  ����ֵ: 
	 *  ��ע:	
	 */
	XOS_BOOL (*Create)(TMemBlock *pMemBlock, int iSize);

	/*  ��������: ��ȡ�ڴ���ַ
	 *  ����ֵ: �ڴ���ַ
	 */
	char* (*GetBuffer)(TMemBlock *pMemBlock);

	/*  ��������: ��ȡ�ڴ�鳤��
	 *  ����ֵ: �ڴ�鳤��
	 */
    int (*GetSize)(TMemBlock *pMemBlock);

	/*  ��������: �����ڴ��ʵ�ʴ�С
	 *  ����˵��:
	 *      int iSize [IN]:		�ڴ��ʵ�ʴ�С
	 */
    XOS_BOOL (*SetSize)(TMemBlock *pMemBlock, int iSize);

//private:	
	char *m_pBuffer;	//�ڴ滺���ַ
	int m_iSize;		//�ڴ���ʵ�ʴ�С
	int m_iMaxSize;		//�ڴ�������С
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TMemBlock MEMBLOCK;

#ifdef __cplusplus
}
#endif

#endif //__MEMBLOCK_H__
