/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSMemory.h
 *  文件标识: 
 *  摘    要: 目前只简单的实现了伪C类的cnew和cdelete函数,
 *			  没有对内存的分配(malloc,new)和释放函数(free,delete)进行封装
 * 
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-17 16:52:11
 *  修正备注: 
 *  
 */

#ifndef __XOSMEMORY_H__
#define __XOSMEMORY_H__

#ifdef __cplusplus
extern "C"  {
#endif

//#define malloc(i) mymalloc((i), __FILE__, __LINE__)
//void *mymalloc(int size, char *pszFileName, int iLine);
void *cnew(int size, const void *def);
void cdelete(void *p);

#ifdef __cplusplus
}
#endif

#endif //__XOSMEMORY_H__
