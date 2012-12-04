/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSThread.h
 *  文件标识: 
 *  摘    要: 多线程相关定义和函数	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-16 18:40:22
 *  修正备注: 
 *  
 */

#ifndef __XOSTHREAD_H__
#define __XOSTHREAD_H__

//////////////////////////////////////////////////////////////////////////
//	宏定义
//////////////////////////////////////////////////////////////////////////

/*
	XOS_INFINITE				无限大
	XOS_GetTickCount			获取毫秒计数
	XOS_Sleep					等待若干毫秒
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
	//这个函数必须在main函数中调用一次初始化
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
//	函数定义
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* thread                                                               */
/************************************************************************/

//线程优先级宏定义
#define XOS_THREADPRILOW       3
#define XOS_THREADPRINORM      5
#define XOS_THREADPRIHIGH      7
#define XOS_THREADPRIKERN      9

/*  功能描述: 创建线程
 *  参数说明:
 *      int stacksize [IN]:	堆栈的大小
 *      void *pfun [IN]:	所要创建的线程
 *		void *pParam [IN]:	此线程带的传入参数
 *  返回值: 线程的句柄, NULL表示创建失败
 */
XOS_HANDLE XOSThread_Create(int stacksize, void *pfun, void *pParam);

/*  功能描述: 销毁线程
 *  参数说明:
 * 		XOS_HANDLE hThread [IN]:	线程的句柄
 */
void XOSThread_Destroy(XOS_HANDLE hThread);

/*  功能描述: 设置线程的优先级
 *  参数说明:
 * 		XOS_HANDLE hThread [IN]:	线程的句柄
 *      int pri [IN]:				优先级,具体取值见前面的线程优先级宏定义
 *  返回值: 修改之前的优先级
 */
int XOSThread_Setpri(XOS_HANDLE hThread, int pri);

/*  功能描述: 获取线程的优先级
 *  参数说明:
 * 		XOS_HANDLE hThread [IN]:	线程的句柄
 *  返回值: 当前的线程优先级
 */
int XOSThread_Getpri(XOS_HANDLE hThread);

/*  功能描述: 获取调用线程的标识
 *  返回值: 当前线程的标识
 */
XOS_U32 XOSThread_GetId();

/************************************************************************/
/* mutex                                                                */
/************************************************************************/

/*  功能描述: 创建互斥量
 *  返回值: 创建的互斥量的句柄,NULL表示创建失败
 */
XOS_HANDLE XOSMutex_Create();

/*  功能描述: 释放互斥量
 *  参数说明:
 * 		XOS_HANDLE hMutex [IN]:	互斥量的句柄
 */
void XOSMutex_Delete(XOS_HANDLE hMutex);

/*  功能描述: 锁定互斥量
 *  参数说明:
 * 		XOS_HANDLE hMutex [IN]:	互斥量的句柄
 */
void XOSMutex_Lock(XOS_HANDLE hMutex);

/*  功能描述: 释放互斥量
 *  参数说明:
 * 		XOS_HANDLE hMutex [IN]:	互斥量的句柄
 */
void XOSMutex_Unlock(XOS_HANDLE hMutex);

/************************************************************************/
/* sem                                                                  */
/************************************************************************/

/*  功能描述: 创建信号量
 *  参数说明:
 * 		int iInitCount [IN]: 初始的信号量个数
 *  返回值: 创建的信号量的句柄,NULL表示创建失败
 */
XOS_HANDLE XOSSem_Create(int iInitCount);

/*  功能描述: 释放信号量
 *  参数说明:
 * 		XOS_HANDLE hSem [IN]: 信号量的句柄
 */
void XOSSem_Delete(XOS_HANDLE hSem);

/*  功能描述: 发送信号量
 *  参数说明:
 * 		XOS_HANDLE hSem [IN]: 信号量的句柄
 *	说明: sema_ipost,DSP的中断函数中使用
 */
void XOSSem_Post(XOS_HANDLE hSem);
void XOSSem_iPost(XOS_HANDLE hSem);

/*  功能描述: 等待信号量
 *  参数说明:
 * 		XOS_HANDLE hSem [IN]: 信号量的句柄
 *		int iTimeOut [IN]:		等待信号量的超时时间
 *	说明: sema_pendt带超时时间的等待,返回TRUE表示等待成功,否则表示失败
 */
void XOSSem_Pend(XOS_HANDLE hSem);
XOS_BOOL XOSSem_Pendt(XOS_HANDLE hSem, int iTimeOut);

#ifdef __cplusplus
}
#endif

#endif //__XOSTHREAD_H__
