/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: WD.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ��������ģ��,��ض���߳�,��������̷߳�����ʱ,��ֹͣι��
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2007-4-6 9:55:50
 *  ������ע: 
 *  
 */

#ifndef __WD_H__
#define __WD_H__

#ifndef WINCE
#define _WATCH_DOG
#endif

/*
	ʹ�÷���:
		1.�ڳ���������ʱ�����WD_Init
		2.�м�����Ҫ�ĵط����ø������ܺ���
		3.�ڳ����˳���ʱ�����,����WD_Close,���ǻ�û��ʵ��(??????)
 */

#ifdef _WATCH_DOG
	
	/*
	 *	��ֱ��ʹ�����º���,����ʹ�ö�Ӧ�ĺ궨��
	 *	�������WD���������Ӱ��,ע�ͺ궨��:"#define _WATCH_DOG"����
	 */

	#ifdef __cplusplus
	extern "C"  {
	#endif

		/*  ��������: ��ʼ��WDģ��
		 *  ����˵��:
		 *      int iMaxThreadNum [IN]:	WDģ��֧�ֵ��߳���
		 *      void (*clear_dog)(void) [IN]: ������幷����
		 *  ����ֵ: 0��ʾ�ɹ�,������ʾʧ��
		 *  ��ע:	ϵͳ������ʱ�����һ�μ���
		 */
		int WD_Init(int iMaxThreadNum, void (*clear_dog)());

		/*  ��������: �õ����̼߳��뵽���ļ����
		 *  ����˵��:
		 *      const char *pszName [IN]:	�̵߳�����
		 *      int iTimeOut [IN]:			��ʱ��ʱ��,��λΪ��
		 *  ��ע:	��һ���߳��е���һ�μ���,һ������̸߳ո�������ʱ��
		 */
		void WD_Join(const char *pszName, int iTimeOut); //iTimeOut��λΪ��

		/*  ��������: �����̵߳ĳ�ʱʱ��
		 *  ����˵��:
		 *      int iTimeOut [IN]:			��ʱ��ʱ��,��λ��
		 *  ��ע:	������ʱ�ı��̵߳ĳ�ʱʱ��,һ�����ڽ���ĳ����֪��ʱ�϶�ĺ���֮ǰ.
		 */
		void WD_SetTimeOut(int iTimeOut);

		/*  ��������: �õ����߳��˳����ļ��
		 *  ��ע:	���߳̽���֮ǰһ��Ҫ���ô˺���,��������WDģ����Դй©
		 */
		void WD_Leave();

		/*  ��������: ���̹߳�
		 *  ��ע:	һ��Ҫ��ָ���ĳ�ʱʱ��֮�ڷ������ô˺���,�������Ϊ�̳߳�ʱ
		 */
		void WD_Clear();

		/*  ��������: �����̹߳����״̬
		 *  ����˵��:
		 *      int (*pPrn)(const char *,...) [IN]	����ı��溯��
		 *  ��ע:	ʹ������������湷״̬,�Ӷ�֪���Ǹ��̳߳�ʱ���µ�����.
		 */
		void WD_Report(int (*pPrn)(const char *,...));

	#ifdef __cplusplus
	}
	#endif

	//������Щ����÷�����Ӧ������˵��.
	#define WD_INIT(i, f) WD_Init(i, f)
	#define WD_JOIN(psz, i) WD_Join(psz, i)
	#define WD_SETTIMEOUT(i) WD_SetTimeOut(i)
	#define WD_LEAVE WD_Leave()
	#define WD_CLEAR WD_Clear()
	#define WD_REPORT(f) WD_Report(f)

#else //#ifndef _WATCH_DOG

	#define WD_INIT(i, f) 0
	#define WD_JOIN(psz, i) 0
	#define WD_SETTIMEOUT(i) 0
	#define WD_LEAVE 0
	#define WD_CLEAR 0
	#define WD_REPORT(f)

#endif //#ifdef _WATCH_DOG

#endif //__WD_H__
