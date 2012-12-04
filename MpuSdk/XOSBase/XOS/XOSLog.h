/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOSLog.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2009-3-2 20:13:13
 *  ������ע: 
 *  
 */

#ifndef __XOSLOG_H__
#define __XOSLOG_H__

#define XLL_XX		"XX"	//������־
#define XLL_IM		"IM"	//�����ܳ��ֵ����
#define XLL_EX		"EX"	//�쳣���
#define XLL_CR		"CR"	//�ؼ�����
#define XLL_UC		"UC"	//�û�����

//¼���û���־����
#define USER_LOG_TYPE_NULL					0
#define USER_LOG_TYPE_LOGIN					1
#define USER_LOG_TYPE_LOGOUT				2
#define USER_LOG_TYPE_SYSTEM_BOOTED			3
#define USER_LOG_TYPE_REBOOT				4
#define USER_LOG_TYPE_SET_TIME				5
#define USER_LOG_TYPE_FORMATTED				6
#define USER_LOG_TYPE_OPEN_DOOR				7
#define USER_LOG_TYPE_CLOSE_DOOR			8
#define USER_LOG_TYPE_SHUTDOWN				9


/*  ��������: ��ȡ�ַ�����
 *  ����ֵ: ��ȡ�����ַ�,-1��ʾû�ж�ȡ�����ַ�
 */
typedef char (*XOSLOG_GET_CHAR)();

/*  ��������: ����ַ�����
 *  ����˵��:
 *      char c [IN]:	Ҫ������ַ�
 *	��ע: ���������ͬ�ϸ���������û���������ӵ�ʱ��ʹ����������ַ��õ�
 */
typedef void (*XOSLOG_PUT_CHAR)(char c);

/*  ��������: �������
 *  ����˵��:
 *      XOS_PSZ pszCommand [IN]:	�����ַ���
 *      XOS_PSZ pszParam [IN]:		��������ַ���
 *  ����ֵ: ����XOS_FALSE��ʾ�ϲ�û�д���,������Ҫ�ڲ�����,ģ���ڲ�����ϵͳ����
 *			����XOS_FALSE��ʾ�ϲ㴦�����������,ģ���ڲ�����Ҫ�ٴ�����.
 *  ��ע: ��Log����̨�յ�����ʱ,���ֲ����ڲ�����͵�������������ϲ㴦��
 *		  �����������Ҫʹ��nprintf���������Ϣ,�����Ͳ����ʱ��ǰ׺
 */
typedef XOS_BOOL (*XOSLOG_ON_COMMAND_IN)(XOS_PSZ pszCommand, XOS_PSZ pszParam);

/*  ��������: ����������
 *  ����˵��:
 *      XOS_PSZ pszParam [IN]:		���������ַ���,һ���Ǿ������ĳһ����İ���
 *  ����ֵ: ����XOS_FALSE��ʾ�ϲ�û�д���,������Ҫ�ڲ�����,ģ���ڲ����ӡ����֧��
 *			����XOS_FALSE��ʾ�ϲ㴦�����������,ģ���ڲ�����Ҫ�ٴ�����.
 *  ��ע: ��Log����̨�յ���������ʱ,�������ڲ�����֮��,��������������ϲ��������
 *		  �����������Ҫʹ��nprintf���������Ϣ,�����Ͳ����ʱ��ǰ׺
 */
typedef XOS_BOOL (*XOSLOG_ON_HELP_IN)(XOS_PSZ pszParam);

#ifdef __cplusplus
extern "C" {
#endif

//���Ҫ��Insure++���printf�Ĵ����������������
#define printf xprintf
//���Ҫ��Insure++���xlprintf�Ĵ�����ſ��������
//#define xlprintf printf

/*  ��������: �����������
 *  ����˵��:
 *      const char *f [IN]:		��ӡ��ʽ
 *      ... [IN]:				��ӡ���ݱ����
 *  ��ע: һ������д�ӡ��,�������ǹؼ��쳣���,����������͹ؼ����̵Ĵ�ӡ
 *		  ��xprintf�������������������ʱ��ǰ׺
 *		  ���������XOSLog_Open��ֱ�ӵ��ô˺���,�������繦��,��׼���
 */
int nprintf(const char *f, ...);

/*  ��������: �����������־�������
 *  ����˵��:
 *      const char *f [IN]:		��ӡ��ʽ
 *      ... [IN]:				��ӡ���ݱ����
 *  ��ע: һ������д�ӡ��,�������ǹؼ��쳣���,����������͹ؼ����̵Ĵ�ӡ
 *		  ��nprintf��������,���������ʱ��ǰ׺
 *		  ���������XOSLog_Open��ֱ�ӵ��ô˺���,�������繦��,��׼���
 */
int xprintf(const char *f, ...);

/*  ��������: ���������־�������
 *  ����˵��:
 *      const char *f [IN]:		��ӡ��ʽ,���������ż���Ĵ�ӡ����,
 *								�Ƽ�ʹ��"[%s]: xxxxxx.\r\n"
 *      const car *level [IN]:	��־���,��һ������������־���,�ַ���,ȡֵ����:
 *									XLL_XX�Ⱥ궨��
 *									�����Զ������,ȡ����������:ModuleName_LogName
 *									�63�ֽ�,ȫ��ʹ��Сд
 *									����: wdial_spdata,��ʾwdialģ��Ĵ������ݴ�ӡ
 *      ... [IN]:				��ӡ���ݱ����
 *  ��ע: ���������־���,����ѡ��򿪻��߹ر�,��ʱ��ǰ׺
 *		  ���������XOSLog_Open��ֱ�ӵ��ô˺���,�������繦��,��׼���
 */
#ifndef xlprintf
int xlprintf(const char *f, ...);
#endif

/*  ��������: ��ӡ�ڴ�����
 *  ����˵��:
 *      unsigned char *pMem [IN]:	�ڴ��ַ
 *      int iLen [IN]:				Ҫ��ӡ���ڴ泤��
 */
void XOSLog_PrintMem(unsigned char *pMem, int iLen);

/*  ��������: ������־ģ��
 *  ����˵��:
 *      XOS_U16 u16Port [IN]:			������־֡���˿�,����0��ʾ������������־
 *      XOSLOG_GET_CHAR get_char [IN]:	������ʱ���õĶ�ȡ�ַ�����,
 *										���Դ���NULL,��ʾ������
 *      XOSLOG_PUT_CHAR put_char [IN]:	������ʱ���õ�����ַ�����,
 *										���Դ���(XOSLOG_PUT_CHAR)&putchar,��׼���
 *      XOSLOG_ON_COMMAND_IN on_command_in [IN]: ����̨���յ����ⲿ�������
 *												 ����NULL��ʾ����Ҫ�����ⲿ����
 *      XOSLOG_ON_HELP_IN on_help_in [IN]: ���ƽ��յ���������ʱ���ⲿ�����������
 *										   ����NULL��ʾû���ⲿ�������
 */
XOS_BOOL XOSLog_Open(XOS_U16 u16Port, 
					 XOSLOG_GET_CHAR get_char, XOSLOG_PUT_CHAR put_char, 
					 XOSLOG_ON_COMMAND_IN on_command_in, XOSLOG_ON_HELP_IN on_help_in);

/*  ��������: ʹ���Զ��������־
 *  ����˵��:
 *      XOS_CPSZ cpszName [IN]:		�Զ����������,ȡֵ��xlprintf�е�˵��
 *  ��ע: ����ܹ�ʹ�ܵ��Զ�����־�������,�Ż᷵��ʧ��.һ�㲻������������
 */
XOS_BOOL XOSLog_EnableCustomLevel(XOS_CPSZ cpszName);

//¼����־
typedef void (*RECORD_LOG)(const char *pszAction, char cLogType);

/*  ��������: ע��¼����־�Ļص�
 *  ����˵��:
 *      RECORD_LOG recordLog [IN]:	������ַ
 */
void XOSLog_RegisterRecord(RECORD_LOG recordLog);

/*  ��������: ��¼¼����־
 *  ����˵��:
 *      XOS_CPSZ cpszAction [IN] : ��־��������
 *		char cLogType [IN]: �û���־����,�ο�����ĺ�
 */
void XOSLog_Record(XOS_CPSZ cpszAction, char cLogType);

/*  ��������: �ر���־ģ��
 */
void XOSLog_Close();

#ifdef __cplusplus
}
#endif

#endif //__XOSLOG_H__
