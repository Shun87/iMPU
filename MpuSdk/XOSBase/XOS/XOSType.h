/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOSType.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-08-22 16:27:46
 *  ������ע: 
 *  
 */

#ifndef __XOSTYPE_H__
#define __XOSTYPE_H__

//�߼�����(��ΪC���Ա���û���߼�����)
#define XOS_FALSE	0
#define XOS_TRUE	1
#define XOS_BOOL	int	//���з��ش����͵ĺ���,XOS_TRUE��ʾ�ɹ�,XOS_FALSE��ʾʧ��

//��������(��ΪC���Ե��������ֽ�������ƽ̨��ͬ����ͬ,������Э�����ʱʹ�����º�)
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

//�ַ���ָ��(��ΪC����û���ַ�������,�����ʱ�����ʹ��char���鶨��)
#define XOS_PSZ		char *
#define	XOS_CPSZ	const char *

//�����(��ΪC����û�о������,���ܶ�ط���Ҫ�õ����)
#define	XOS_HANDLE	void *

#endif //__XOSTYPE_H__
