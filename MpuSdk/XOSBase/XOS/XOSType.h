/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSType.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-08-22 16:27:46
 *  修正备注: 
 *  
 */

#ifndef __XOSTYPE_H__
#define __XOSTYPE_H__

//逻辑类型(因为C语言本身没有逻辑类型)
#define XOS_FALSE	0
#define XOS_TRUE	1
#define XOS_BOOL	int	//所有返回此类型的函数,XOS_TRUE表示成功,XOS_FALSE表示失败

//整数类型(因为C语言的整数的字节数根据平台不同而不同,在网络协议相关时使用如下宏)
#define XOS_S8	char
#define XOS_S16	short		
#define XOS_S32	int
#ifdef WIN32
#define XOS_S64	__int64
#elif _CREARO_NDK
#else //LINUX
#define XOS_S64	long long
#endif

#define XOS_U8	unsigned char
#define XOS_U16	unsigned short	
#define XOS_U32	unsigned int
#ifdef WIN32
#define XOS_U64	__int64
#elif _CREARO_NDK
#else //LINUX
#define XOS_U64	unsigned long long
#endif

//字符串指针(因为C语言没有字符串类型,定义的时候可以使用char数组定义)
#define XOS_PSZ		char *
#define	XOS_CPSZ	const char *

//句柄型(因为C语言没有句柄类型,而很多地方需要用到这个)
#define	XOS_HANDLE	void *

#endif //__XOSTYPE_H__
