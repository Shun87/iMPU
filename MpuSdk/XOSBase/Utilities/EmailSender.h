/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: EmailSender.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: xuejiabin
 *  �������: 2007-7-5 9:55:48
 *  ������ע: 
 *  
 */
#ifndef __EMAILSENDER_H__
#define __EMAILSENDER_H__

#define EMAILSENDER_TIMER_ERROR					-1
#define EMAILSENDER_TIMER_OK					0
#define EMAILSENDER_TIMER_BUSY					1
#define EMAILSENDER_TIMER_FREE					2

#define EMAILSENDER_ERROR						-1
#define EMAILSENDER_OK							0
#define EMAILSENDER_WOULDBLOCK					1

#define EMAILSENDER_STATUS_NONE                 -1
#define EMAILSENDER_STATUS_START				0	//��ʼ״̬
#define EMAILSENDER_STATUS_CONNECTING			1	//�����ʼ�������
#define EMAILSENDER_STATUS_SENDHEADCOMAND		2	//���������֤
#define EMAILSENDER_STATUS_SENDDATA				3	//���ż�����
#define EMAILSENDER_STATUS_RECV					4	

typedef struct _TEmailSender TEmailSender;
struct _TEmailSender
{
//public:
	void (*Construct)(TEmailSender *cthis);
	void (*Destroy)(TEmailSender *cthis);

	/*  ��������: 
	 *  ����˵��:
	 *      TEmailSender *cthis [IN]:
	 *      LPCTSTR lpszSmtpServer [IN]:	�ʼ�������(smtp)��ַ������
	 *      LPCTSTR  lpszUserName [IN]:		�û���(����smtpЭ��ʱ����Ϊ���ַ���)
	 *      BOOL bHasPassword [IN]:			�ж��Ƿ�ʹ��(ESMTPЭ��),����TRUE,����FALSE
	 *      LPCTSTR lpszPassword [IN]:		Ϊ�û������Ӧ������
	 *      LPCTSTR lpszSenderEmail [IN]:	�������ַ
	 *      LPCTSTR lpszReceiverEmail [IN]:	�ռ����ַ
	 *      LPCTSTR lpszSubject [IN]:		�ʼ�����
	 *      LPCTSTR lpszContent [IN]:		�ʼ�����
	 *      char *pAttachmentBuffer [IN]:	��������
	 *      int iAttachmentLen [IN]:		�������� Ϊ0�Ͳ�������
	 *      char* pszAttachName [IN]:		��������
	 */
 	XOS_BOOL (*Create)(TEmailSender *cthis, char* lpszSmtpServer,
				   char*  lpszUserName, XOS_BOOL bHasPassword, char* lpszPassword,
				   char* lpszSenderEmail, char* lpszReceiverEmail,
				   char* lpszSubject, char* lpszContent,
				   char *pAttachmentBuffer, int iAttachmentLen, char *pszAttachName);

	XOS_S32 (*OnTimer)(TEmailSender *cthis);
	void (*Report)(TEmailSender *cthis, int (*pPrn)(const char *,...));

//private:
	XOS_S32 (*Connect)(TEmailSender *cthis);
	XOS_S32 (*ConnetProbe)(TEmailSender *cthis);
	XOS_S32 (*SendData)(TEmailSender *cthis);
 	XOS_S32 (*SendHead)(TEmailSender *cthis);
	
	int m_state;
	char *m_pHeadAddress;
	char *m_pDataAddress;
	XOS_SOCKET m_sock;	
	int m_recvlen;
	XOS_BOOL m_bSendHead;
	XOS_BOOL m_bHavePassword;
	unsigned int m_serverIP;
	char *m_pszSenderEmail;
	char *m_pszReceiverEmail;
	char *m_pszUserName64;					
	char *m_pszPassword64;					
	char *m_pszSendBuffer;
	char *m_pszSendHead;
	char *m_pszRcvData;
	char *m_pszRcvDataAddress;
	int m_sendHeadlen;
	int m_haveSendLen;
	char *m_pszServerURL;
	XOS_BOOL m_bFirstsend;
	int	m_sendDatalen;
	int m_sendCmdType;
	unsigned int m_uiCreateTime;					//����ʱ��
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TEmailSender EMAILSENDER;

#ifdef __cplusplus
}
#endif

#endif //__EMAILSENDER_H__
