/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: EmailSender.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xuejiabin
 *  完成日期: 2007-7-5 9:55:48
 *  修正备注: 
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
#define EMAILSENDER_STATUS_START				0	//开始状态
#define EMAILSENDER_STATUS_CONNECTING			1	//链接邮件服务器
#define EMAILSENDER_STATUS_SENDHEADCOMAND		2	//进行身份验证
#define EMAILSENDER_STATUS_SENDDATA				3	//发信件类容
#define EMAILSENDER_STATUS_RECV					4	

typedef struct _TEmailSender TEmailSender;
struct _TEmailSender
{
//public:
	void (*Construct)(TEmailSender *cthis);
	void (*Destroy)(TEmailSender *cthis);

	/*  功能描述: 
	 *  参数说明:
	 *      TEmailSender *cthis [IN]:
	 *      LPCTSTR lpszSmtpServer [IN]:	邮件服务器(smtp)地址或域名
	 *      LPCTSTR  lpszUserName [IN]:		用户名(当用smtp协议时可以为空字符串)
	 *      BOOL bHasPassword [IN]:			判断是否使用(ESMTP协议),是填TRUE,否填FALSE
	 *      LPCTSTR lpszPassword [IN]:		为用户名相对应的密码
	 *      LPCTSTR lpszSenderEmail [IN]:	发件箱地址
	 *      LPCTSTR lpszReceiverEmail [IN]:	收件箱地址
	 *      LPCTSTR lpszSubject [IN]:		邮件标题
	 *      LPCTSTR lpszContent [IN]:		邮件类容
	 *      char *pAttachmentBuffer [IN]:	附件名称
	 *      int iAttachmentLen [IN]:		附件长度 为0就不发附件
	 *      char* pszAttachName [IN]:		附件类容
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
	unsigned int m_uiCreateTime;					//创建时间
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TEmailSender EMAILSENDER;

#ifdef __cplusplus
}
#endif

#endif //__EMAILSENDER_H__
