/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: KeyAndMAC2Hash.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-12-22 16:33:41
 *  ������ע: 
 *  
 */

#ifndef __KEYANDMAC2HASH_H__
#define __KEYANDMAC2HASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	CR300PT�������ȡ����
 */
void GetPrivateKey(unsigned char *pKey);		//˽����Կ
void GetPrivateKey_reg(unsigned char *pKey);	//˽��ע����Կ

/*  ��������: ��Key��MAC���ɹ�ϣֵ
 *  ����˵��:
 *      char *pKey [IN]:	����,16�ֽ�
 *      char *pMac [IN]:	MAC,6�ֽ�
 *		char binHash[16] [OUT]: ��ȡ����HASHֵ
 */
void KeyAndMAC2Hash0(char *pKey, char *pMac, char binHash[16]);
void KeyAndMAC2Hash1(char *pKey, char *pMac, char binHash[16]);
void KeyAndMAC2Hash2(char *pKey, char *pMac, char binHash[16]);

/*  ��������: ������ͺŻ�ȡ��ϣֵ
 *  ����˵��:
 *      char *pszSoftModel [IN]:	����ͺ�
 *		char binHash[16] [OUT]: ��ȡ����HASHֵ
 */
void SoftModel2Hash(char *pszSoftModel, char binHash[16]);

#ifdef __cplusplus
}
#endif

#endif //__KEYANDMAC2HASH_H__
