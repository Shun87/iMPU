/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: StreamComposer.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ���������ϳɹ���,�������������׷�ӵķ�ʽ�ϳ�Ϊһ��������������
 *			  ������ɴ������ֽ��������ֽ����ת��
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-09-28 11:29:20
 *  ������ע: 
 *  
 */

#ifndef __STREAMCOMPOSER_H__
#define __STREAMCOMPOSER_H__

typedef struct _TStreamComposer TStreamComposer;
struct _TStreamComposer  
{
//public:
	void (*Construct)(TStreamComposer *pStreamComposer);
	void (*Destroy)(TStreamComposer *pStreamComposer);

	/*  ��������: �������������ϳɹ���
	 *  ����˵��:
	 *      void *pBuf [IN]:	��źϳɺ�Ķ��������Ļ����ַ
	 *      int iMaxSize [IN]:	��źϳɺ�Ķ��������Ļ������󳤶�
	 *		XOS_BOOL bNBO [IN]:	�Ƿ��������ֽ���
	 *  ��ע: �˺����������ڴ����
	 */
	XOS_BOOL (*Create)(TStreamComposer *pStreamComposer, void *pBuf, int iMaxSize, XOS_BOOL bNBO);
	
	//���������Ϊ���ڴ����,����ע�Ͳ���
	//XOS_BOOL (*Create2)(TStreamComposer *pStreamComposer, int iMaxSize);

	/*  ��������: ��ȡ����������ŵĻ����ַ
	 *  ����ֵ: ����������ŵĻ����ַ
	 */
	char* (*GetBuffer)(TStreamComposer *pStreamComposer);

	/*  ��������: ��ȡ����������ǰ��䵽�Ļ����ַ
	 *  ����ֵ: ����������ǰ��䵽�Ļ����ַ
	 *	��ע: �����Ĺ�����,��ǰ��䵽�Ļ����ַ��������
	 */
	char* (*GetCurPos)(TStreamComposer *pStreamComposer);

	/*  ��������: ��ȡ�Ѿ����Ķ��������ĳ���
	 *  ����ֵ: �Ѿ����Ķ��������ĳ���
	 */
	int (*GetLength)(TStreamComposer *pStreamComposer);

	/*  ��������: ��ȡʣ�µĻ��ܹ������ֽ���
	 *  ����ֵ: ʣ�µĻ��ܹ������ֽ���
	 */
	int (*LeftLength)(TStreamComposer *pStreamComposer);
	
	/*  ��������: ���������ֽڲ����
	 *  ����˵��:
	 *      int iSize [IN]:	�������ֽ���
	 *  ��ע: ֮��ֱ������ָ���ֽ�������µ�����
	 */
	XOS_BOOL (*SkipLength)(TStreamComposer *pStreamComposer, int iSize);

	/*  ��������: ���һ��2�ֽڶ������
	 */
	XOS_BOOL (*AddPAD2)(TStreamComposer *pStreamComposer);

	/*  ��������: ���һ��4�ֽڶ������
	 */
	XOS_BOOL (*AddPAD4)(TStreamComposer *pStreamComposer);

	/*  ��������: ���һ��8�ֽڶ������
	 */
	XOS_BOOL (*AddPAD8)(TStreamComposer *pStreamComposer);

	/*  ��������: ���һ���ַ�
	 *  ����˵��:
	 *      XOS_S8 c [IN]:	Ҫ��ӵ��ַ�
	 */
	XOS_BOOL (*AddCHAR)(TStreamComposer *pStreamComposer, XOS_S8 c);

	/*  ��������: ���һ���޷����ַ�
	 *  ����˵��:
	 *      XOS_U8 uc [IN]:	Ҫ��ӵ��ַ�
	 */
	XOS_BOOL (*AddUCHAR)(TStreamComposer *pStreamComposer, XOS_U8 uc);

	/*  ��������: ���һ������������(2�ֽ�)
	 *  ����˵��:
	 *      XOS_S16 s [IN]:	Ҫ��ӵĶ���������
	 */
	XOS_BOOL (*AddSHORT)(TStreamComposer *pStreamComposer, XOS_S16 s);

	/*  ��������: ���һ���޷��Ŷ���������(2�ֽ�)
	 *  ����˵��:
	 *      XOS_S16 s [IN]:	Ҫ��ӵ��޷��Ŷ���������
	 */
	XOS_BOOL (*AddUSHORT)(TStreamComposer *pStreamComposer, XOS_U16 us);

	/*  ��������: ���һ���޷�����������(4�ֽ�)
	 *  ����˵��:
	 *      XOS_S32 i [IN]:	Ҫ��ӵ���������
	 */
	XOS_BOOL (*AddINT)(TStreamComposer *pStreamComposer, XOS_S32 i);

	/*  ��������: ���һ���޷�����������(4�ֽ�)
	 *  ����˵��:
	 *      XOS_U32 ui [IN]:	Ҫ��ӵ���������
	 */
	XOS_BOOL (*AddUINT)(TStreamComposer *pStreamComposer, XOS_U32 ui);

	/*  ��������: ���һ��4�ֽ�IP��ַ
	 *  ����˵��:
	 *      XOS_U32 ui [IN]:	Ҫ��ӵ�IP��ַ
	 */
	XOS_BOOL (*AddIP)(TStreamComposer *pStreamComposer, XOS_U32 ip);

	/*  ��������: ���һ��16�ֽ�GUID����
	 *  ����˵��:
	 *      const TCGUID * pGuid [IN]:	Ҫ��ӵ�GUID����
	 */
	XOS_BOOL (*AddGUID)(TStreamComposer *pStreamComposer, const TCGUID* pGuid);

	/*  ��������: ���һ����0��β���ַ���
	 *  ����˵��:
	 *      XOS_PSZ psz [IN]:	Ҫ��ӵ��ַ���
	 */
	XOS_BOOL (*AddString)(TStreamComposer *pStreamComposer, XOS_CPSZ cpsz);

	/*  ��������: ��������ֽڵĶ���������
	 *  ����˵��:
	 *      const void *pBuf [IN]:	Ҫ��ӵĶ��������ݵĵ�ַ
	 *      int iLen [IN]:			Ҫ��ӵĶ��������ݵĳ���
	 */
	XOS_BOOL (*AddBuffer)(TStreamComposer *pStreamComposer, const void *pBuf, int iLen);

//private:
	XOS_BOOL (*BeforeIncSize)(TStreamComposer *pStreamComposer, int iSize);
	XOS_BOOL (*IncSize)(TStreamComposer *pStreamComposer, int iSize);
	
	char *m_pBuf;
	int m_iSize;
	int m_iMaxSize;
	XOS_BOOL m_bDeleteBuf;
	XOS_BOOL bNBO;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TStreamComposer STREAMCOMPOSER;

#ifdef __cplusplus
}
#endif

#endif //__STREAMCOMPOSER_H__
