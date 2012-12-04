/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: KeyAndMAC2Hash.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-12-22 16:33:41
 *  修正备注: 
 *  
 */

#ifndef __KEYANDMAC2HASH_H__
#define __KEYANDMAC2HASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	CR300PT的密码获取函数
 */
void GetPrivateKey(unsigned char *pKey);		//私有密钥
void GetPrivateKey_reg(unsigned char *pKey);	//私有注册密钥

/*  功能描述: 由Key和MAC生成哈希值
 *  参数说明:
 *      char *pKey [IN]:	密码,16字节
 *      char *pMac [IN]:	MAC,6字节
 *		char binHash[16] [OUT]: 获取到的HASH值
 */
void KeyAndMAC2Hash0(char *pKey, char *pMac, char binHash[16]);
void KeyAndMAC2Hash1(char *pKey, char *pMac, char binHash[16]);
void KeyAndMAC2Hash2(char *pKey, char *pMac, char binHash[16]);

/*  功能描述: 由软件型号获取哈希值
 *  参数说明:
 *      char *pszSoftModel [IN]:	软件型号
 *		char binHash[16] [OUT]: 获取到的HASH值
 */
void SoftModel2Hash(char *pszSoftModel, char binHash[16]);

#ifdef __cplusplus
}
#endif

#endif //__KEYANDMAC2HASH_H__
