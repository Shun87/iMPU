/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: PostContentParser.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-09-05 19:32:24
 *  ������ע: 
 *  
 */

#ifndef __POSTCONTENTPARSER_H__
#define __POSTCONTENTPARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ����name=value&...��ʽ�ַ���,һ��name=value��Ϊһ����Ŀ(Item)
 *  ����˵��:
 *      int *puiItemNum [IN, OUT]:	�����ʱ�����ܹ���ŵ���Ŀ�������Ŀ
 *									�����ʱ���Ƿ�����������Ŀ�ĸ���
 *      char **ppItems [OUT]:		������������Ŀ�����ֵ�ָ������
 *      char **ppValues [OUT]:		������������Ŀ��ֵ��ָ������,�����±�һһ��Ӧ
 *      char *pszPostContent [IN]:	Ҫ�������ַ���
 *  ��ע: �����Ĺ��̻��ƻ�Ҫ�������ַ���,����ַ�������������;,����ҪԤ�ȿ���һ��	
 */
void PostContentParser(int *piItemNum, char **ppItems, char **ppValues, char *pszPostContent);

#ifdef __cplusplus
}
#endif

#endif //__POSTCONTENTPARSER_H__
