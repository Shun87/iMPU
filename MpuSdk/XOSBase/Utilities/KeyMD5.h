/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: KeyMD5.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-02-16 10:55:05
 *  ������ע: 
 *  
 */

#ifndef __KEYMD5_H__
#define __KEYMD5_H__

#ifdef __cplusplus
extern "C" {
#endif

int KeyMD5_Encode(char* Encoded, char* Data, int iSize, char *Key, int iKeyLen);

#ifdef __cplusplus
}
#endif

#endif //__KEYMD5_H__
