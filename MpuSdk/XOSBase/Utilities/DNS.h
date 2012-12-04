/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: DNS.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: DNSģ��,�ڲ�ά����һ������,�����DNS��ѯ����������.		
 *  
 *  ��ǰ�汾: 
 *  ��    ��: zhaoqq
 *  �������: 2006-9-28 8:49:17
 *  ������ע: 
 *  
 */

#ifndef __DNS_H__
#define __DNS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
ʹ�÷���:
		1.����ȫ�ֳ�ʼ������DNS2IPInit
		2.Ȼ�������������ʹ���书��,���Ⱥ�˳��֮��
 */

/*  ��������: ��ʼ��ͨ��������ȡIP�Ĺ���
 */
int DNS2IPInit();

/*  ��������: ����DSNģ���ڲ�״̬
 *  ����˵��:
 *      int (*pPrn)(const char *,...) [IN]:	ʹ�õı����������
 */
void DNS2IPReport(int (*pPrn)(const char *,...));

/*  ��������: ͨ�������õ�����Ӧ��IP��ַ
 *  ����˵��:
 *      XOS_PSZ lpsz [IN]:	��Ҫ��ѯ������
 *	����ֵ: ʧ���򷵻�0,�ɹ����ػ�õ�IP��ַ.
 *	��ע:	���ܲ���һ�ξ��ܹ���ɹ���,ʧ��֮����Ҫ��������.
 */
XOS_U32 DNS2IP(XOS_CPSZ lpsz);

/*  ��������: ���ַ�����ַ����ȡIP�Ͷ˿�
 *  ����˵��:
 *      XOS_PSZ lpsz [IN]:		�����URL,��ʽ:"����:�˿�"����"IP:�˿�",����:"www.crearo.com:8888","202.38.64.10:80"
 *								Ҳ����ʡ�Զ˿�,����::"www.crearo.com","202.38.64.10",��ʱ*pusPort��ֵ���ı�
 *      XOS_U32 *puiIP [OUT]:��ȡ��IP��ַ,���������,���ں����ڲ�����������IP��ת��
 *      XOS_U16 *pusPort [OUT]:	��ǰ�Ķ˿�,���URL�����˿�,��*pusPort��ֵ���ı�
 *      XOS_BOOL bDefPort [IN]:		�Ƿ���ȱʡ�˿�,���ΪTRUE��ʾURL�п��Բ����˿�,FALSE��ʾ����Ҫ�˿�.
 *	��ע:	���URL��������,����ܲ���һ�ξ��ܹ���ɹ���,ʧ��֮����Ҫ��������.
 */
XOS_BOOL Addr2IPAndPort(XOS_CPSZ lpsz, XOS_U32 *puiIP, XOS_U16 *pusPort, XOS_BOOL bDefPort);

/*  ��������: ʹ��IP�Ͷ˿ںϳ��ַ�����ַ
 *  ����˵��:
 *      XOS_U32 uiIP [IN]:	32λIP��ַ
 *      XOS_U16 usPort [IN]:	���Ͷ˿�
 *  ����ֵ: �ϳɺ��URL�ַ���
 *	��ע:	���������֧�ֶ��߳�����
 */
XOS_PSZ IPAndPort2Addr(XOS_U32 uiIP, XOS_U16 usPort);

#ifdef __cplusplus
}
#endif

#endif //__DNS_H__
