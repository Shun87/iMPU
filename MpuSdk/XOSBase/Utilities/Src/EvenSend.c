/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: EvenSend.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-11-12 10:27:18
 *  ������ע: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../EvenSend.h"

void EvenSend_Init(TEvenSend *pEvenSend, int iTargetBitRate)
{
	pEvenSend->iTargetBitRate = iTargetBitRate;
	pEvenSend->bTargetBitRateChanged = 0;
	pEvenSend->bHungry = 1;		//һ��ʼ�ʹ��ڼ���״̬
	pEvenSend->uiBeginTIme = 0;	//����״̬��̽��ʱ�ͻ����¿�ʼ����
	pEvenSend->iSentBytes = 0;
}

int EvenSend_Probe(TEvenSend *pEvenSend, int iNewBytes)
{
	int iCanSendBytes;			//�ܹ����͵��ֽ���
	char bEvenSendReset = 0;	//���ȷ����Ƿ�����

	//�ж��Ƿ��ڼ���״̬
	if (pEvenSend->bHungry)
	{
		pEvenSend->bHungry = 0;	//��λ�������,�´����û���ݷ�,�����óɼ�����
		bEvenSendReset = 1;			//�����ñ��,���᲻��ʱ������ܹ����͵��ֽ���
	}
	if (pEvenSend->bTargetBitRateChanged)
	{
		pEvenSend->bTargetBitRateChanged = 0;
		bEvenSendReset = 1;			//�����ñ��,���᲻��ʱ������ܹ����͵��ֽ���
	}

	//���û������,���տ�ʼʱ������ܹ����͵��ֽ���
	if (!bEvenSendReset)
	{
		double dDeltaTime = (XOS_GetTickCount() - pEvenSend->uiBeginTIme)/1000.0;
		int iTotalCanSentBytes = (int)(dDeltaTime*pEvenSend->iTargetBitRate*1024.0/8);
		iCanSendBytes = iTotalCanSentBytes - pEvenSend->iSentBytes;
	}
	else
	{
		pEvenSend->uiBeginTIme = XOS_GetTickCount();
		pEvenSend->iSentBytes = 0;	//�Ѿ������ֽ�����0
		iCanSendBytes = 0;			//�ó�0,��ʾ���β���,�´��ٷ�,
									//���ǵ�Timer�������õ÷ǳ�Ƶ��,��������������
	}

	if (iCanSendBytes <= 0)
	{
		return EVENSEND_E_WOULDBLOCK;
	}
	else
	{
		//���������͹�������,����ϲ�û�з���,������ü���״̬,
		//����״̬�����¼���,���о���û��,����������Ҳ����ν
		//̽�ⲻ���͵Ļ�,����ͨ��iNewBytes����0����
		pEvenSend->iSentBytes += iNewBytes;
		return EVENSEND_E_OK;
	}
}

void EvenSend_SetHungry(TEvenSend *pEvenSend)
{
	pEvenSend->bHungry = 1;
}

void EvenSend_SetTargetBitRate(TEvenSend *pEvenSend, int iTargetBitRate)
{
	pEvenSend->iTargetBitRate = iTargetBitRate;
	pEvenSend->bTargetBitRateChanged = 1;
}
