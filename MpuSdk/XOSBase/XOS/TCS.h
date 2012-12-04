/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: TCS.h
 *  文件标识: 
 *  摘    要: 线程调用栈监控模块,为每个线程记录一个函数调用栈.
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2005-09-08 13:22:16
 *  修正备注: 
 *  
 */

#ifndef __TCS_H__
#define __TCS_H__

/*
	使用方法:
		1.在程序启动的时候调用TCS_Init
		2.中间在需要的地方调用各个功能函数
		3.在程序退出的时候调用,调用TCS_Close
 */

#ifndef WINCE
#define _TDEBUG
#endif

#ifdef _TDEBUG

	/*
	 *	不直接使用如下函数,而是使用对应的宏定义
	 *	如果不想TCS给代码带来影响,注释宏定义:"#define _TDEBUG"即可
	 */

	#ifdef __cplusplus
	extern "C"  {
	#endif

		/*  功能描述: 初始化TCS模块
		 *  参数说明:
		 *      int iMaxThreadNum [IN]:	TCS模块支持的最大线程数
		 *  返回值: 0表示成功,其他表示失败
		 *  备注:	全局调用一次即可
		 */
		int TCS_Init(int iMaxThreadNum);

		/*  功能描述: 让调用线程加入到TCS模块
		 *  参数说明:
		 *      int iUserData [IN]:		用户自定义数据,用于区分同名线程的不同实例
		 *  备注:	在一个线程中调用一次即可,一般放在线程刚刚启动的时候
		 */
		void TCS_Join(int iUserData);

		/*  功能描述: 让调用线程退出TCS模块
		 *  备注:	在线程结束之前一定要调用此函数,否则会造成TCS模块资源泄漏
		 */
		void TCS_Leave();

		/*  功能描述: 记录一个线程调用函数
		 *  参数说明:
		 *      const char *pszFunctionName [IN]:	函数名
		 *      const char *pszFileName [IN]:		调用处的文件名,使用预订义宏"__FILE__"获取
		 *      int iLineNo [IN]:					调用处的文件行号,使用预订义宏"__LINE__"获取
		 *											在特殊场合也用做记录其他整数.
		 *  备注:	在调用一个函数之前添加,在VC60编辑器中可以通过宏快速添加
		 */
		void TCS_Push(const char *pszFunctionName, const char *pszFileName, int iLineNo);

		/*  功能描述: 弹出一个线程调用函数
		 *  备注:	在调用一个函数之后添加,在VC60编辑器中可以通过宏快速添加
		 */
		void TCS_Pop();

		/*  功能描述: 增加线程内部计数,以确定线程还在运转
		 */
		void TCS_Count();

		/*  功能描述: 关闭TCS模块
		 *  备注:	在程序退出的时候调用,全局调用一次即可
		 */
		void TCS_Close();

		/*  功能描述: 取得堵塞的时间 行 文件 函数,便于录像记下日志
		 *  备注: WD在程序快要超时重启时调用
		 */
		char *TCS_GetBlockString(int iThreadNum);

		/*  功能描述:	报告TCS内部状况
		 *  参数说明:
		 *      int (*pPrn)(const char *,...) [IN]
		 *  备注:	就是靠这个函数来监控系统中线程的函数状态的.
		 */
		void TCS_Report(int (*pPrn)(const char *,...));

	#ifdef __cplusplus
	}
	#endif

	//下面这些宏的用法见对应函数的说明.
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
