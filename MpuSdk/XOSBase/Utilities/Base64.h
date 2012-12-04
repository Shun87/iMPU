/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: Base64.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-02-16 08:39:50
 *  ������ע: 
 *  
 */

#ifndef __BASE64_H__
#define __BASE64_H__

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: Base64����
 *  ����˵��:
 *      char *pszEncoded [OUT]:	������Base64�ַ���,��0��β
 *      char *pData [IN]:		��Ҫ����Ķ���������
 *      int iSize [IN]:			���������ݵĳ���
 *  ����ֵ: ���ر�����ַ����ĳ���,��������β��0
 */
int Base64_Encode(char *pszEncoded, char *pData, int iSize);

/*  ��������: Base64����
 *  ����˵��:
 *      char *pszDecoded [IN]:	��Ҫ�����Base64�ַ���,��0��β,
 *								�ַ������ȱ�����4��������
 *      char *pData [OUT]:		�����Ķ��������ݵĴ�Ż����ַ
 *      int iSize [IN]:			�����Ķ��������ݵĴ���ܳ���,
 *								����Ҫ�ܹ����½���������,�����ض�
 *  ����ֵ: ���ؽ����ʵ�����Ķ����ƻ���ĳ���
 */
int Base64_Decode(char *pszDecoded, char *pData, int iSize);

#ifdef __cplusplus
}
#endif

#endif //__BASE64_H__
