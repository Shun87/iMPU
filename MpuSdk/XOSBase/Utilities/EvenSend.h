/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: EvenSend.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ʵ�������ݵľ��ȷ���	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-11-12 10:27:16
 *  ������ע: 
 *  
 */

#ifndef __EVENSEND_H__
#define __EVENSEND_H__

//���ش����붨��
#define EVENSEND_E_OK				0	//�ɹ�
#define EVENSEND_E_WOULDBLOCK		1	//��ʱ���ɲ���

typedef struct _TEvenSend
{
	int iTargetBitRate;			//Ŀ�귢������,��λKbps;
	char bTargetBitRateChanged;	//Ŀ�귢�������Ƿ�ı�
	char bHungry;				//�Ƿ��ڼ���״̬
	unsigned int uiBeginTIme;	//���ȷ��Ϳ�ʼʱ��
	int iSentBytes;				//�ӿ�ʼʱ�俪ʼ,�Ѿ����͵��ֽ���
} TEvenSend;

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ��ʼ��
 *  ����˵��:
 *      TEvenSend *pEvenSend [IN]:		�������ݽṹָ��
 *      int iBitRate []:
 *  ����ֵ: void 
 *  ��ע:	
 */
void EvenSend_Init(TEvenSend *pEvenSend, int iTargetBitRate);

/*  ��������: ̽���Ƿ���Է�
 *  ����˵��:
 *      TEvenSend *pEvenSend [IN]:		�������ݽṹָ��
 *      int iNewBytes [IN]:				����Ҫ���͵�,0��ʾֻ̽��,������
 *  ����ֵ: ������,ȡֵ��ǰ��ĺ궨��,��������:
 *				EVENSEND_E_OK			�ɹ�,���Է���
 *				EVENSEND_E_WOULDBLOCK	��ʱ�����Է���
 */
int EvenSend_Probe(TEvenSend *pEvenSend, int iNewBytes);

/*  ��������: ���ü���״̬
 *  ����˵��:
 *      TEvenSend *pEvenSend [IN]:		�������ݽṹָ��
 *  ��ע: ���̽�⵽���Է�,����û�����ݿɷ����������
 */
void EvenSend_SetHungry(TEvenSend *pEvenSend);

/*  ��������: ����Ŀ�귢������
 *  ����˵��:
 *      TEvenSend *pEvenSend [IN]:		�������ݽṹָ��
 *      int iTargetBitRate [IN]:		Ŀ�귢������
 */
void EvenSend_SetTargetBitRate(TEvenSend *pEvenSend, int iTargetBitRate);

#ifdef __cplusplus
}
#endif

#endif //__EVENSEND_H__
