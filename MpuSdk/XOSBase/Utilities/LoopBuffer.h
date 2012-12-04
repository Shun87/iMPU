/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: LoopBuffer.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-11-10 14:04:35
 *  ������ע: 
 *  
 */

#ifndef __LOOPBUFFER_H__
#define __LOOPBUFFER_H__

typedef struct _TLoopBuffer TLoopBuffer;
struct _TLoopBuffer
{
//public:
	void (*Construct)(TLoopBuffer *pLoopBuffer);
	void (*Destroy)(TLoopBuffer *pLoopBuffer);
	XOS_BOOL (*Create)(TLoopBuffer *pLoopBuffer, XOS_U32 uiBufferLen);

	/*  ��������: ���λ���д������
	 *  ����˵��:
	 *      char *pData [IN]:		����ָ��
	 *      XOS_U32 uiDataLen [IN]:	���ݳ���
	 *      XOS_BOOL bCover [IN]:		�Ƿ񸲸�,�������,������֮��,����ֵ����ʵ��д����ֽ���
	 *  ����ֵ: ��ӳɹ����ֽ���,���ѡ�񸲸�,�����ֵ����uiDataLen;
	 */
	XOS_U32 (*WriteData)(TLoopBuffer *pLoopBuffer, char *pData, XOS_U32 uiDataLen, XOS_BOOL bCover);

	/*  ��������: �ӻ��λ����ȡ����
	 *  ����˵��:
	 *      char *pData [IN]:		����ָ��
	 *      XOS_U32 *puiDataLen [IN, OUT]:	��ȥ��ʱ����pData�Ĵ�С,������ʱ����ʵ�������ֽ���.
	 */
	XOS_BOOL (*ReadData)(TLoopBuffer *pLoopBuffer, char *pData, XOS_U32 *puiDataLen);

//private:
	char* (*pAdd)(TLoopBuffer *pLoopBuffer, char *p);

	char *m_pBuf;
	XOS_U32 m_uiBufferLen;
	char *m_pRead;
	char *m_pWrite;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TLoopBuffer LOOPBUFFER;

#ifdef __cplusplus
}
#endif

#endif //__LOOPBUFFER_H__
