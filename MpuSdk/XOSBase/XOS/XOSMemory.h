/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOSMemory.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: Ŀǰֻ�򵥵�ʵ����αC���cnew��cdelete����,
 *			  û�ж��ڴ�ķ���(malloc,new)���ͷź���(free,delete)���з�װ
 * 
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-10-17 16:52:11
 *  ������ע: 
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
