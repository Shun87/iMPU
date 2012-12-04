/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: TCS.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: �̵߳���ջ���ģ��,Ϊÿ���̼߳�¼һ����������ջ.
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2005-09-08 13:22:16
 *  ������ע: 
 *  
 */

#ifndef __TCS_H__
#define __TCS_H__

/*
	ʹ�÷���:
		1.�ڳ���������ʱ�����TCS_Init
		2.�м�����Ҫ�ĵط����ø������ܺ���
		3.�ڳ����˳���ʱ�����,����TCS_Close
 */

#ifndef WINCE
#define _TDEBUG
#endif

#ifdef _TDEBUG

	/*
	 *	��ֱ��ʹ�����º���,����ʹ�ö�Ӧ�ĺ궨��
	 *	�������TCS���������Ӱ��,ע�ͺ궨��:"#define _TDEBUG"����
	 */

	#ifdef __cplusplus
	extern "C"  {
	#endif

		/*  ��������: ��ʼ��TCSģ��
		 *  ����˵��:
		 *      int iMaxThreadNum [IN]:	TCSģ��֧�ֵ�����߳���
		 *  ����ֵ: 0��ʾ�ɹ�,������ʾʧ��
		 *  ��ע:	ȫ�ֵ���һ�μ���
		 */
		int TCS_Init(int iMaxThreadNum);

		/*  ��������: �õ����̼߳��뵽TCSģ��
		 *  ����˵��:
		 *      int iUserData [IN]:		�û��Զ�������,��������ͬ���̵߳Ĳ�ͬʵ��
		 *  ��ע:	��һ���߳��е���һ�μ���,һ������̸߳ո�������ʱ��
		 */
		void TCS_Join(int iUserData);

		/*  ��������: �õ����߳��˳�TCSģ��
		 *  ��ע:	���߳̽���֮ǰһ��Ҫ���ô˺���,��������TCSģ����Դй©
		 */
		void TCS_Leave();

		/*  ��������: ��¼һ���̵߳��ú���
		 *  ����˵��:
		 *      const char *pszFunctionName [IN]:	������
		 *      const char *pszFileName [IN]:		���ô����ļ���,ʹ��Ԥ�����"__FILE__"��ȡ
		 *      int iLineNo [IN]:					���ô����ļ��к�,ʹ��Ԥ�����"__LINE__"��ȡ
		 *											�����ⳡ��Ҳ������¼��������.
		 *  ��ע:	�ڵ���һ������֮ǰ���,��VC60�༭���п���ͨ����������
		 */
		void TCS_Push(const char *pszFunctionName, const char *pszFileName, int iLineNo);

		/*  ��������: ����һ���̵߳��ú���
		 *  ��ע:	�ڵ���һ������֮�����,��VC60�༭���п���ͨ����������
		 */
		void TCS_Pop();

		/*  ��������: �����߳��ڲ�����,��ȷ���̻߳�����ת
		 */
		void TCS_Count();

		/*  ��������: �ر�TCSģ��
		 *  ��ע:	�ڳ����˳���ʱ�����,ȫ�ֵ���һ�μ���
		 */
		void TCS_Close();

		/*  ��������: ȡ�ö�����ʱ�� �� �ļ� ����,����¼�������־
		 *  ��ע: WD�ڳ����Ҫ��ʱ����ʱ����
		 */
		char *TCS_GetBlockString(int iThreadNum);

		/*  ��������:	����TCS�ڲ�״��
		 *  ����˵��:
		 *      int (*pPrn)(const char *,...) [IN]
		 *  ��ע:	���ǿ�������������ϵͳ���̵߳ĺ���״̬��.
		 */
		void TCS_Report(int (*pPrn)(const char *,...));

	#ifdef __cplusplus
	}
	#endif

	//������Щ����÷�����Ӧ������˵��.
	#define TCS_INIT(d) TCS_Init(d) 
	#define TCS_JOIN(d) TCS_Join(d)
	#define TCS_LEAVE TCS_Leave()
	#define TCS_PUSH(fun) TCS_Push(fun, __FILE__, __LINE__)
	#define TCS_PUSH2(fun, i) TCS_Push(fun, __FILE__, i)
	#define TCS_POP	TCS_Pop()
	#define TCS_COUNT TCS_Count()
	#define TCS_REPORT(f) TCS_Report(f)

#else //#ifdef _TDEBUG

	#define TCS_INIT(d) 0
	#define TCS_JOIN(d)
	#define TCS_LEAVE
	#define TCS_PUSH(fun)
	#define TCS_PUSH2(fun, i)
	#define TCS_POP
	#define TCS_COUNT
	#define TCS_REPORT(f)
	
#endif //#ifdef _TDEBUG

#endif //__TCS_H__
