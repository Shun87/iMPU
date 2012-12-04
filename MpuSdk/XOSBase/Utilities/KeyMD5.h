/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: KeyMD5.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-02-16 10:55:05
 *  修正备注: 
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
