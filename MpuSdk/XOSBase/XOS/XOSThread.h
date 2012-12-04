/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOSThread.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ���߳���ض���ͺ���	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-10-16 18:40:22
 *  ������ע: 
 *  
 */

#ifndef __XOSTHREAD_H__
#define __XOSTHREAD_H__

//////////////////////////////////////////////////////////////////////////
//	�궨��
//////////////////////////////////////////////////////////////////////////

/*
	XOS_INFINITE				���޴�
	XOS_GetTickCount			��ȡ�������
	XOS_Sleep					�ȴ����ɺ���
 */

#ifdef WIN32
	#define XOS_INFINITE		INFINITE
	#define XOS_GetTickCount()  GetTickCount()
	#define XOS_Sleep(i)		Sleep(i)
#elif _CREARO_NDK
	#define XOS_INFINITE		0xFFFFFFFF
	#define XOS_Sleep(i)		TSK_sleep((i))
	#define XOS_GetTickCount()	((XOS_U32)CLK_getltime())
#else //LINUX
	#define XOS_INFINITE		0xFFFFFFFF
	//#define XOS_GetTickCount()  (10*(XOS_U32)times(NULL))
	//#define XOS_Sleep(i)		usleep((i)*1000)
	//�������������main�����е���һ�γ�ʼ��
	#ifdef __cplusplus
	extern "C" {
	#endif
		XOS_U32 XOS_GetTickCount();
		void XOS_Sleep(XOS_U32 u32);
	#ifdef __cplusplus
	}
	#endif
#endif

#ifdef _CREARO_NDK
	typedef struct _TEnv
	{
		void *pWD;
		void *pThread;
	} TEnv;
#endif

//////////////////////////////////////////////////////////////////////////
//	��������
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* thread                                                               */
/************************************************************************/

//�߳����ȼ��궨��
#define XOS_THREADPRILOW       3
#define XOS_THREADPRINORM      5
#define XOS_THREADPRIHIGH      7
#define XOS_THREADPRIKERN      9

/*  ��������: �����߳�
 *  ����˵��:
 *      int stacksize [IN]:	��ջ�Ĵ�С
 *      void *pfun [IN]:	��Ҫ�������߳�
 *		void *pParam [IN]:	���̴߳��Ĵ������
 *  ����ֵ: �̵߳ľ��, NULL��ʾ����ʧ��
 */
XOS_HANDLE XOSThread_Create(int stacksize, void *pfun, void *pParam);

/*  ��������: �����߳�
 *  ����˵��:
 * 		XOS_HANDLE hThread [IN]:	�̵߳ľ��
 */
void XOSThread_Destroy(XOS_HANDLE hThread);

/*  ��������: �����̵߳����ȼ�
 *  ����˵��:
 * 		XOS_HANDLE hThread [IN]:	�̵߳ľ��
 *      int pri [IN]:				���ȼ�,����ȡֵ��ǰ����߳����ȼ��궨��
 *  ����ֵ: �޸�֮ǰ�����ȼ�
 */
int XOSThread_Setpri(XOS_HANDLE hThread, int pri);

/*  ��������: ��ȡ�̵߳����ȼ�
 *  ����˵��:
 * 		XOS_HANDLE hThread [IN]:	�̵߳ľ��
 *  ����ֵ: ��ǰ���߳����ȼ�
 */
int XOSThread_Getpri(XOS_HANDLE hThread);

/*  ��������: ��ȡ�����̵߳ı�ʶ
 *  ����ֵ: ��ǰ�̵߳ı�ʶ
 */
XOS_U32 XOSThread_GetId();

/************************************************************************/
/* mutex                                                                */
/************************************************************************/

/*  ��������: ����������
 *  ����ֵ: �����Ļ������ľ��,NULL��ʾ����ʧ��
 */
XOS_HANDLE XOSMutex_Create();

/*  ��������: �ͷŻ�����
 *  ����˵��:
 * 		XOS_HANDLE hMutex [IN]:	�������ľ��
 */
void XOSMutex_Delete(XOS_HANDLE hMutex);

/*  ��������: ����������
 *  ����˵��:
 * 		XOS_HANDLE hMutex [IN]:	�������ľ��
 */
void XOSMutex_Lock(XOS_HANDLE hMutex);

/*  ��������: �ͷŻ�����
 *  ����˵��:
 * 		XOS_HANDLE hMutex [IN]:	�������ľ��
 */
void XOSMutex_Unlock(XOS_HANDLE hMutex);

/************************************************************************/
/* sem                                                                  */
/************************************************************************/

/*  ��������: �����ź���
 *  ����˵��:
 * 		int iInitCount [IN]: ��ʼ���ź�������
 *  ����ֵ: �������ź����ľ��,NULL��ʾ����ʧ��
 */
XOS_HANDLE XOSSem_Create(int iInitCount);

/*  ��������: �ͷ��ź���
 *  ����˵��:
 * 		XOS_HANDLE hSem [IN]: �ź����ľ��
 */
void XOSSem_Delete(XOS_HANDLE hSem);

/*  ��������: �����ź���
 *  ����˵��:
 * 		XOS_HANDLE hSem [IN]: �ź����ľ��
 *	˵��: sema_ipost,DSP���жϺ�����ʹ��
 */
void XOSSem_Post(XOS_HANDLE hSem);
void XOSSem_iPost(XOS_HANDLE hSem);

/*  ��������: �ȴ��ź���
 *  ����˵��:
 * 		XOS_HANDLE hSem [IN]: �ź����ľ��
 *		int iTimeOut [IN]:		�ȴ��ź����ĳ�ʱʱ��
 *	˵��: sema_pendt����ʱʱ��ĵȴ�,����TRUE��ʾ�ȴ��ɹ�,�����ʾʧ��
 */
void XOSSem_Pend(XOS_HANDLE hSem);
XOS_BOOL XOSSem_Pendt(XOS_HANDLE hSem, int iTimeOut);

#ifdef __cplusplus
}
#endif

#endif //__XOSTHREAD_H__
