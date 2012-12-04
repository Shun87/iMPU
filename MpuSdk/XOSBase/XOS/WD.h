/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: WD.h
 *  文件标识: 
 *  摘    要: 软件狗监控模块,监控多个线程,如果任意线程发生超时,则停止喂狗
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-4-6 9:55:50
 *  修正备注: 
 *  
 */

#ifndef __WD_H__
#define __WD_H__

#ifndef WINCE
#define _WATCH_DOG
#endif

/*
	使用方法:
		1.在程序启动的时候调用WD_Init
		2.中间在需要的地方调用各个功能函数
		3.在程序退出的时候调用,调用WD_Close,可是还没有实现(??????)
 */

#ifdef _WATCH_DOG
	
	/*
	 *	不直接使用如下函数,而是使用对应的宏定义
	 *	如果不想WD给代码带来影响,注释宏定义:"#define _WATCH_DOG"即可
	 */

	#ifdef __cplusplus
	extern "C"  {
	#endif

		/*  功能描述: 初始化WD模块
		 *  参数说明:
		 *      int iMaxThreadNum [IN]:	WD模块支持的线程数
		 *      void (*clear_dog)(void) [IN]: 传入的清狗函数
		 *  返回值: 0表示成功,其他表示失败
		 *  备注:	系统启动的时候调用一次即可
		 */
		int WD_Init(int iMaxThreadNum, void (*clear_dog)());

		/*  功能描述: 让调用线程加入到狗的监控中
		 *  参数说明:
		 *      const char *pszName [IN]:	线程的名字
		 *      int iTimeOut [IN]:			超时的时间,单位为秒
		 *  备注:	在一个线程中调用一次即可,一般放在线程刚刚启动的时候
		 */
		void WD_Join(const char *pszName, int iTimeOut); //iTimeOut单位为秒

		/*  功能描述: 设置线程的超时时间
		 *  参数说明:
		 *      int iTimeOut [IN]:			超时的时间,单位秒
		 *  备注:	用于临时改变线程的超时时间,一般用在进行某个已知耗时较多的函数之前.
		 */
		void WD_SetTimeOut(int iTimeOut);

		/*  功能描述: 让调用线程退出狗的监控
		 *  备注:	在线程结束之前一定要调用此函数,否则会造成WD模块资源泄漏
		 */
		void WD_Leave();

		/*  功能描述: 清线程狗
		 *  备注:	一定要在指定的超时时间之内反复调用此函数,否则会认为线程超时
		 */
		void WD_Clear();

		/*  功能描述: 报告线程狗监控状态
		 *  参数说明:
		 *      int (*pPrn)(const char *,...) [IN]	输入的报告函数
		 *  备注:	使用这个函数报告狗状态,从而知道那个线程超时导致的重启.
		 */
		void WD_Report(int (*pPrn)(const char *,...));

	#ifdef __cplusplus
	}
	#endif

	//下面这些宏的用法见对应函数的说明.
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
