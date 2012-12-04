/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: CycleStat.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ����ͳ����,��ͬʱ֧�ֶ��ͳ������ͳ��,����ͳ��������Ҫ��ͬ
 *			  ���ͳ�Ƽ���Ϊ4G,�ܹ���������ͳ����
 *				1.ͳ��������ͳ����������
 *				2.ͳ��������ͳ����������,��λ:ÿ��
 *				3.ͳ��������ͳ�����ľ�ֵ
 *				4.ͳ��������ͳ���������ֵ
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-11-12 14:46:20
 *  ������ע: 
 *  
 */

#ifndef __CYCLESTAT_H__
#define __CYCLESTAT_H__

//���ش����붨��
#define CYCLESTAT_E_OK				0	//�ɹ�
#define CYCLESTAT_E_WOULDBLOCK		1	//��ʱ���ɲ���

#define MAX_CYCLESTAT_COUNTER_NUM	4
typedef struct _TCycleStat
{
	int iStatCycle;												//ͳ������,��λ��
	unsigned int uiLastStatTime;								//�ϴ�ͳ��ʱ��
	unsigned int uiCounters[MAX_CYCLESTAT_COUNTER_NUM];			//����ֵ����
	unsigned int uiLastCounters[MAX_CYCLESTAT_COUNTER_NUM];		//�ϴεļ���ֵ����
	unsigned int uiCounterNum[MAX_CYCLESTAT_COUNTER_NUM];		//������Ŀ
	unsigned int uiLastCounterNum[MAX_CYCLESTAT_COUNTER_NUM];	//�ϴεļ�����Ŀ
	int uiMaxCounters[MAX_CYCLESTAT_COUNTER_NUM];				//������ֵ
	int uiLastMaxCounters[MAX_CYCLESTAT_COUNTER_NUM];			//�ϴε�������ֵ
} TCycleStat;

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ��ʼ������
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *      int iStatCycle [IN]:			ͳ������,��λ��
 */
void CycleStat_Init(TCycleStat *pCycleStat, int iStatCycle);

/*  ��������: ���Ӽ���
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *      int iIdx [IN]:					����������
 *      int iCounter [IN]:				���ӵļ���
 */
void CycleStat_Count(TCycleStat *pCycleStat, int iIdx, int iCounter);

/*  ��������: ���ʱ���Ƿ���
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *  ����ֵ: ������,ȡֵ��ǰ��ĺ궨��,��������:
 *				CYCLESTAT_E_OK			�ɹ�,��ʱ����
 *				CYCLESTAT_E_WOULDBLOCK	��û�е�ʱ��
 *  ��ע: �ϲ���Ҫ��ͣ�ĵ��ô˺������,������ͳ�����ڿ������ͳ�ƽ����׼
 */
int CycleStat_Check(TCycleStat *pCycleStat);

/*  ��������: ��ȡ����
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *      int iIdx [IN]:					��ȡ��ͳ�Ƽ�����������
 *  ����ֵ: ��ȡ��������
 *  ��ע: Check�������سɹ�֮��,���ô˺������ɻ����һͳ�������ڵ�ͳ��ֵ
 */
int CycleStat_GetTotal(TCycleStat *pCycleStat, int iIdx);

/*  ��������: ��ȡ����
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *      int iIdx [IN]:					��ȡ��ͳ�Ƽ�����������
 *  ����ֵ: ��ȡ��������
 *  ��ע: Check�������سɹ�֮��,���ô˺������ɻ����һͳ�������ڵ�����,��λ:ÿ��
 */
int CycleStat_GetRate(TCycleStat *pCycleStat, int iIdx);

/*  ��������: ��ȡ��ֵ
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *      int iIdx [IN]:					��ȡ��ͳ�Ƽ�����������
 *  ����ֵ: ��ȡ���ľ�ֵ,-1��ʾû��ͳ������
 *  ��ע: Check�������سɹ�֮��,���ô˺������ɻ����һͳ�������ڵľ�ֵ
 */
int CycleStat_GetAverage(TCycleStat *pCycleStat, int iIdx);

/*  ��������: ��ȡ���ֵ
 *  ����˵��:
 *      TCycleStat *pCycleStat [IN]:	�������ݽṹָ��
 *      int iIdx [IN]:					��ȡ��ͳ�Ƽ�����������
 *  ����ֵ: ��ȡ�������ֵ 
 *  ��ע: Check�������سɹ�֮��,���ô˺������ɻ����һͳ�������ڵ����ֵ
 */
int CycleStat_GetMax(TCycleStat *pCycleStat, int iIdx);

#ifdef __cplusplus
}
#endif

#endif //__CYCLESTAT_H__
