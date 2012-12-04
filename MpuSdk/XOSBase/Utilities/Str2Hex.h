/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: Str2Hex.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: zhaoqq
 *  �������: 2007-02-01 11:23:09
 *  ������ע: 
 *  
 */

#ifndef __STR2HEX_H__
#define __STR2HEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ���ַ���ת��ɶ�����
 *  ����˵��:
 *      char *pStr [IN]:	��Ҫת�����ַ���
 *      char *pHex [OUT]:	ת�������Ķ��������ݴ�ŵ�ַ,�ϲ㱣֤�����С����.
 */
XOS_BOOL Str2Hex(char *pStr, char *pHex);

/*  ��������: �Ѷ�����ת����ַ���
 *  ����˵��:
 *      char *pHex [IN]:	��Ҫת���Ķ����ƻ���ĵ�ַ
 *      char *pStr [OUT]:	ת������ַ�����ŵ�ַ,�ϲ㱣֤�����С����
 *      int iHexLen [IN]:	��Ҫת���Ķ����ƻ���ĳ���
 *  ����ֵ: ת������ַ���ָ��,���ڴ����pStr,����������Ƿ��سɹ���.
 *			�����û������pStr����,���Կ��Բ��Է���ֵ�����ж�
 *  ��ע: Hex2Strת���ɴ�д��ĸ,Hex2Strlת����Сд��ĸ.
 */
char *Hex2Str(char *pHex, char *pStr, int iHexLen);
char *Hex2Strl(char *pHex, char *pStr, int iHexLen);

#ifdef __cplusplus
}
#endif

#endif //__STR2HEX_H__
