/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: EmailSender.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xuejiabin
 *  完成日期: 2008-12-5 11:07:00
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../DNS.h"
#include "../Base64.h"
#include "../EmailSender.h"

#define EMAILSENDER_HEADDATA_LEN  512

#define CMDTYPE_HELO		1
#define CMDTYPE_AUTHLOGIN   2
#define CMDTYPE_USERNAME	3
#define CMDTYPE_PASSWORD	4
#define CMDTYPE_MAIL		5
#define CMDTYPE_RCPT		6
#define CMDTYPE_DATA		7

static char *s_szfSTMPHead = "HELO\r\n\
MAIL FROM:<%s>\r\n\
RCPT TO:<%s>\r\n\
DATA\r\n";

static char *s_szfESTMPhead = "EHLO %s\r\n\
AUTH LOGIN\r\n\
%s\r\n\
%s\r\n\
MAIL FROM:<%s>\r\n\
RCPT TO:<%s>\r\n\
DATA\r\n";

static char *s_szfData ="From:<%s>\r\n\
To:<%s>\r\n\
Subject:%s\r\n\
MIME-Version: 1.0\r\n\
Content-Type: multipart/mixed; boundary=\"NextPart_000_0001_01C7BA55.C134D4C0\"\r\n\
\r\n\
--NextPart_000_0001_01C7BA55.C134D4C0\r\n\
Content-Type: text/plain;\r\n\
	charset=\"GB2312\"\r\n\
Content-Transfer-Encoding: 8bit\r\n\
\r\n\
%s\r\n\
%s\r\n\
\r\n\
--NextPart_000_0001_01C7BA55.C134D4C0--\r\n\
\r\n\
.\r\n\
quit\r\n\
";

char * s_szfAttach ="\r\n\
--NextPart_000_0001_01C7BA55.C134D4C0\r\n\
Content-Type: image/jpg; name=\"%s\"\r\n\
Content-Transfer-Encoding: base64\r\n\
Content-Disposition: attachment\r\n\
\r\n\
\r\n\
%s\r\n\
";

static int GetBase64Len(int len)
{
	return ((len + 2) / 3 * 4);
}

static void Construct(TEmailSender *cthis)
{
	cthis->m_state = EMAILSENDER_STATUS_NONE;
	cthis->m_sock = XOS_INVALID_SOCKET;

	cthis->m_pszPassword64 = NULL;
	cthis->m_pszUserName64 = NULL;
	cthis->m_pszSendBuffer = NULL;	//通过m_pDataAddress释放
	cthis->m_pDataAddress = NULL;
	cthis->m_pszSendHead = NULL;	//通过m_pHeadAddress释放
	cthis->m_pHeadAddress = NULL;
	cthis->m_pszServerURL = NULL;
	cthis->m_pszRcvData = NULL;
	cthis->m_pszSenderEmail = NULL;
	cthis->m_pszReceiverEmail = NULL;
	cthis->m_pszRcvDataAddress = NULL;

	cthis->m_sendDatalen = 0;
	cthis->m_serverIP = 0;
	cthis->m_recvlen = 0;
	cthis->m_bSendHead = XOS_TRUE;
	cthis->m_bFirstsend = XOS_TRUE;
}

static void Destroy(TEmailSender *cthis)
{
	if (cthis->m_sock != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(cthis->m_sock);
	}
	if(cthis->m_pszPassword64 != NULL)
	{
		free(cthis->m_pszPassword64);
	}
	if(cthis->m_pszUserName64 != NULL)
	{
		free(cthis->m_pszUserName64);
	}
	if(cthis->m_pDataAddress != NULL)
	{
		free(cthis->m_pDataAddress);
	}
	if(cthis->m_pHeadAddress != NULL)
	{
		free(cthis->m_pHeadAddress);
	}
	if(cthis->m_pszServerURL != NULL)
	{
		free(cthis->m_pszServerURL);
	}
	if(cthis->m_pszRcvDataAddress !=NULL)
	{
		free(cthis->m_pszRcvDataAddress);
	}
	if(cthis->m_pszSenderEmail != NULL)
	{
		free(cthis->m_pszSenderEmail);
	}
	if(cthis->m_pszReceiverEmail !=NULL)
	{
		free(cthis->m_pszReceiverEmail);
	}
	printf("distroy email success!\r\n");
}

static int SendComAndGetRecv(TEmailSender *cthis)
{
	int rv;
	if (cthis->m_sendHeadlen != cthis->m_haveSendLen)
	{
		rv = XOS_TCPSendDataNB(cthis->m_sock, cthis->m_pszSendHead, &(cthis->m_haveSendLen),
			cthis->m_sendHeadlen);
		if(rv != 0)
		{
			if (rv == 2)
			{				
				cthis->m_uiCreateTime = XOS_GetTickCount();
			}
			if (rv < 0)
			{
				return EMAILSENDER_ERROR;
			}
			else
			{
				return EMAILSENDER_WOULDBLOCK;
			}
		}
		else
		{
			cthis->m_sendHeadlen = cthis->m_haveSendLen;
			cthis->m_pszRcvData = cthis->m_pszRcvDataAddress;
			memset(cthis->m_pszRcvData, 0, EMAILSENDER_HEADDATA_LEN);
			cthis->m_recvlen = 0;
		}
	}
	rv = XOS_TCPRecvHeadNB(cthis->m_sock, cthis->m_pszRcvData, EMAILSENDER_HEADDATA_LEN,
		&(cthis->m_recvlen),"\r\n");
	if (rv != 0)
	{
		if (rv == 2)
		{				
			cthis->m_uiCreateTime = XOS_GetTickCount();
		}
		if (rv < 0)
		{
			return EMAILSENDER_ERROR;
		}
		else
		{
			return EMAILSENDER_WOULDBLOCK;
		}
	}
	else
	{
		cthis->m_haveSendLen = 0;
	}
	return EMAILSENDER_OK;	
}

static XOS_BOOL Create(TEmailSender *cthis, char* lpszSmtpServer,
				   char*  lpszUserName, XOS_BOOL bHasPassword, char* lpszPassword,
				   char* lpszSenderEmail, char* lpszReceiverEmail,
				   char* lpszSubject, char* lpszContent,
				   char *pAttachmentBuffer, int iAttachmentLen, char* pszAttachName)
{
	int len = 0;
	char *pszAttach;
	char *pszAttachment;
	int UserNamelen;
	int ReceiverEmaillen;
	int SendEmaillen;
	int Subjectlen;
	int Contentlen;
	int AttachNamelen;
	int pszAttachlen;
	int pszuserName64len;
	int pszpassword64len;

	//判断参数合法性
	if (bHasPassword && (lpszUserName == NULL))
	{
		return XOS_FALSE;
	}
	if (bHasPassword && (lpszPassword == NULL))
	{
		return XOS_FALSE;
	}

	//接收邮件地址
	if (lpszReceiverEmail == NULL)
	{
		ReceiverEmaillen = 0;
	}
	else
	{
		ReceiverEmaillen = strlen(lpszReceiverEmail);
	}
	if (pszAttachName == NULL)
	{
		AttachNamelen = 0;
	}
	else
	{
		AttachNamelen = strlen(pszAttachName);
	}

	//发送邮件地址
	if (lpszSenderEmail == NULL)
	{
		SendEmaillen = 0;
	}
	else
	{
		SendEmaillen = strlen(lpszSenderEmail);
	}

	if (lpszUserName == NULL)
	{
		UserNamelen = 0;
	}
	else
	{
		UserNamelen = strlen(lpszUserName);
	}

	//邮件标题
	if (lpszSubject == NULL)
	{
		Subjectlen = 0;
	}
	else
	{
		Subjectlen = strlen(lpszSubject);
	}

	//邮件内容
	if (lpszContent == NULL)
	{
		Contentlen = 0;
	}
	else
	{
		Contentlen = strlen(lpszContent);
	}

	pszAttachlen = GetBase64Len(iAttachmentLen)+1;
	pszAttach = (char *)malloc(pszAttachlen);
	if(pszAttach == NULL)
	{
		printf("Attach malloc failed.\r\n", __LINE__);
		return XOS_FALSE;
	}
	cthis->m_uiCreateTime = XOS_GetTickCount();
	memset(pszAttach, 0, pszAttachlen);
	Base64_Encode(pszAttach, pAttachmentBuffer, iAttachmentLen);

	cthis->m_pszRcvData = (char *)malloc(EMAILSENDER_HEADDATA_LEN);
	if(cthis->m_pszRcvData == NULL)
	{
		free(pszAttach);
		printf("rcvdata malloc failed.\r\n");
		return XOS_FALSE;
	}
	memset(cthis->m_pszRcvData, 0, EMAILSENDER_HEADDATA_LEN);
	cthis->m_pszRcvDataAddress = cthis->m_pszRcvData;

	pszuserName64len = GetBase64Len(strlen(lpszUserName))+1;
	cthis->m_pszUserName64 = (char *)malloc(pszuserName64len);
	if (cthis->m_pszUserName64 == NULL)
	{
		free(pszAttach);
		return XOS_FALSE;
	}
	memset(cthis->m_pszUserName64, 0, pszuserName64len);
	len = Base64_Encode(cthis->m_pszUserName64, (char *)lpszUserName, strlen(lpszUserName));

	pszpassword64len = GetBase64Len(strlen(lpszPassword))+1;
	cthis->m_pszPassword64 = (char *)malloc(pszpassword64len);
	if(cthis->m_pszPassword64 == NULL)
	{
		free(pszAttach);
		return XOS_FALSE;
	}
	memset(cthis->m_pszPassword64, 0, pszpassword64len);
	len = Base64_Encode(cthis->m_pszPassword64, (char *)lpszPassword, strlen(lpszPassword)); 
	
	cthis->m_pszSendHead = (char *)malloc(EMAILSENDER_HEADDATA_LEN);
	if(cthis->m_pszSendHead == NULL)
	{
		free(pszAttach);
		return XOS_FALSE;
	}	
	cthis->m_sendHeadlen = 0;
	cthis->m_pHeadAddress = cthis->m_pszSendHead;
	memset(cthis->m_pszSendHead, 0, len);
	
	lpszSenderEmail, lpszReceiverEmail;
	cthis->m_pszSenderEmail = (char *)malloc(strlen(lpszSenderEmail) +2);
	memset(cthis->m_pszSenderEmail, 0, strlen(lpszSenderEmail) +2);
	strcpy(cthis->m_pszSenderEmail, lpszSenderEmail);
	
	cthis->m_pszReceiverEmail = (char *)malloc(strlen(lpszReceiverEmail) +2);
	memset(cthis->m_pszReceiverEmail, 0, strlen(lpszReceiverEmail) +2);
	strcpy(cthis->m_pszReceiverEmail, lpszReceiverEmail);

	cthis->m_bHavePassword = bHasPassword;
	if(bHasPassword)
	{
		sprintf(cthis->m_pszSendHead, "EHLO %s\r\n", lpszUserName);
		cthis->m_sendDatalen = strlen(cthis->m_pszSendHead);

/*
		sprintf(cthis->m_pszsendHead, s_szfESTMPhead, lpszUserName,
				cthis->m_pszuserName64, cthis->m_pszpassword64, 
				lpszSenderEmail, lpszReceiverEmail);
		cthis->m_sendHeadlen = strlen(cthis->m_pszsendHead);
*/
	}
	else
	{
		sprintf(cthis->m_pszSendHead, "HELO\r\n");
		cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
		
/*
		sprintf(cthis->m_pszSendHead, s_szfSTMPHead, lpszSenderEmail, lpszReceiverEmail);
		cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
*/
	}
	cthis->m_haveSendLen = 0;
	cthis->m_sendCmdType = CMDTYPE_HELO;

	if (lpszContent != NULL)
	{
		len = strlen(s_szfAttach) + strlen(s_szfData) + pszAttachlen + ReceiverEmaillen + SendEmaillen +
			  Subjectlen + Contentlen + AttachNamelen;
	}
	cthis->m_pszSendBuffer = (char *)malloc(len);
	if(cthis->m_pszSendBuffer == NULL)
	{
		free(pszAttach);
		return XOS_FALSE;
	}
	pszAttachment = (char *)malloc(strlen(s_szfAttach) + AttachNamelen + pszAttachlen + 2);
	if(pszAttachment == NULL)
	{
		free(pszAttach);
		return XOS_FALSE;
	}
	cthis->m_pDataAddress = cthis->m_pszSendBuffer;
	memset(cthis->m_pszSendBuffer, 0, len);
	sprintf(pszAttachment, s_szfAttach, pszAttachName, pszAttach);
	if(iAttachmentLen == 0)
	{
		sprintf(cthis->m_pszSendBuffer, s_szfData, lpszSenderEmail, lpszReceiverEmail, lpszSubject, lpszContent, "");
	}
	else
	{
		sprintf(cthis->m_pszSendBuffer, s_szfData, lpszSenderEmail, lpszReceiverEmail, lpszSubject, lpszContent, pszAttachment);
	}
	cthis->m_sendDatalen = strlen(cthis->m_pszSendBuffer);
	free(pszAttach);
	free(pszAttachment);

	cthis->m_pszServerURL = (char *)malloc(strlen(lpszSmtpServer)+1);
	if(cthis->m_pszServerURL == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszServerURL, 0, strlen(lpszSmtpServer));
	strcpy(cthis->m_pszServerURL, lpszSmtpServer);
	printf("create email success!\r\n");
	return XOS_TRUE;
}

static XOS_S32 Connect(TEmailSender *cthis)
{
    int rv = 0;
	unsigned short usPort = 25;
	if (cthis->m_serverIP == 0)
	{
		if (!Addr2IPAndPort(cthis->m_pszServerURL, &cthis->m_serverIP, &usPort, 1))
		{
			return EMAILSENDER_WOULDBLOCK;
		}
	}
	
	cthis->m_sock = XOS_TCPConnectNB(cthis->m_serverIP, usPort);
	if (cthis->m_sock == XOS_INVALID_SOCKET)
	{
		printf("create email_server connect failed.\r\n");
		return EMAILSENDER_ERROR;
	}
	else
	{
		cthis->m_recvlen = 0;
		return EMAILSENDER_OK;
	}
	assert(0);
	return EMAILSENDER_OK;
}

static XOS_S32 ConnetProbe(TEmailSender *cthis)
{
	int rv;
	rv = XOS_TCPConnectProbe(cthis->m_sock);
	if (rv == 0)
	{
		rv = XOS_TCPRecvHeadNB(cthis->m_sock, cthis->m_pszRcvData, EMAILSENDER_HEADDATA_LEN,
			&(cthis->m_recvlen),"\r\n");
		if (rv == 0)
		{	
			cthis->m_pszRcvData = cthis->m_pszRcvDataAddress;
			strtok(cthis->m_pszRcvData, " ");
			if (atoi(cthis->m_pszRcvData) != 220)
			{
				memset(cthis->m_pszRcvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				printf("the connect to server is error!\r\n");
				return EMAILSENDER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			printf("email connect success!\r\n");
			return EMAILSENDER_OK;
		}
		else
		{
			if (rv == 1)
			{
				return EMAILSENDER_WOULDBLOCK;
			}
			else if (rv == 2)
			{				
				cthis->m_uiCreateTime = XOS_GetTickCount();
				return EMAILSENDER_WOULDBLOCK;
			}
			else
			{
				return EMAILSENDER_ERROR;
			}
		}
	}
	else if (rv == 1)
	{
		return EMAILSENDER_WOULDBLOCK;
	}
	else//rv=-1
	{
		printf("send email tcp connector select failed.\r\n");
		return EMAILSENDER_ERROR;
	}
}

static XOS_S32 SendData(TEmailSender *cthis)
{
	int rv = 0;

	rv = XOS_TCPSendDataNB(cthis->m_sock, cthis->m_pszSendBuffer, &(cthis->m_haveSendLen), cthis->m_sendDatalen);
	if (rv == 0)
	{
		printf("email send success!\r\n");
		return EMAILSENDER_OK;
	}
	else if (rv == -1)
	{
		printf("email send failed.\r\n");
		return EMAILSENDER_ERROR;
	}
	else if (rv == 2)
	{				
		cthis->m_uiCreateTime = XOS_GetTickCount();
		return EMAILSENDER_WOULDBLOCK;
	}
	else
	{
		return EMAILSENDER_WOULDBLOCK;
	}	
}

static XOS_S32 SendHead(TEmailSender *cthis)
{
	int rv = 0;	
	int rcv = 0;

	//m_bSendHead的含义:为TRUE的时候表示发送头,为FALSE的时候表示接收响应
	while(1)
	{
		rv = SendComAndGetRecv(cthis);
		if (rv != EMAILSENDER_OK)
		{
			return rv;
		}
		cthis->m_pszRcvData = cthis->m_pszRcvDataAddress;
		strtok(cthis->m_pszRcvData, " ");
		switch(cthis->m_sendCmdType)
		{
		case CMDTYPE_HELO:
			if ((strcmp(cthis->m_pszRcvData, "250") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			if (cthis->m_bHavePassword)
			{
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "AUTH LOGIN\r\n");
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_AUTHLOGIN;
			}
			else
			{
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "MAIL FROM:<%s>\r\n", cthis->m_pszSenderEmail);
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_MAIL;
			}
			break;
		case CMDTYPE_AUTHLOGIN:
			if ((strcmp(cthis->m_pszRcvData, "250") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			else
			{
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "%s\r\n", cthis->m_pszUserName64);
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_USERNAME;
			}
			break;
		case CMDTYPE_USERNAME:
			if ((strcmp(cthis->m_pszRcvData, "334") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			else
			{				
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "%s\r\n", cthis->m_pszPassword64);
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_PASSWORD;
			}
			break;
		case CMDTYPE_PASSWORD:
			if ((strcmp(cthis->m_pszRcvData, "335") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			else
			{
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "MAIL FROM:<%s>\r\n", cthis->m_pszSenderEmail);
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_MAIL;
			}
			break;
		case CMDTYPE_MAIL:
			if ((strcmp(cthis->m_pszRcvData, "250") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			else
			{
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "RCPT TO:<%s>\r\n", cthis->m_pszReceiverEmail);
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_RCPT;
			}
			break;
		case CMDTYPE_RCPT:
			if ((strcmp(cthis->m_pszRcvData, "250") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			else
			{
				cthis->m_pszSendHead = cthis->m_pHeadAddress;
				memset(cthis->m_pszSendHead, 0, EMAILSENDER_HEADDATA_LEN);
				sprintf(cthis->m_pszSendHead, "DATA\r\n");
				cthis->m_sendHeadlen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_sendCmdType = CMDTYPE_DATA;
			}
			break;
		case CMDTYPE_DATA:
			if ((strcmp(cthis->m_pszRcvData, "354") != 0))
			{
				printf("email send head failed!\r\n");
				return EMAILSENDER_ERROR;
			}
			else
			{				
				cthis->m_haveSendLen = 0;
				return EMAILSENDER_OK;
			}
			break;
		default:
			return EMAILSENDER_ERROR;
		}
	}
		
}

static XOS_S32 OnTimer(TEmailSender *cthis)
{
	int rv;
	//检查连接是否超时
	if ((XOS_GetTickCount() - cthis->m_uiCreateTime) > 60*1000)
	{
		return EMAILSENDER_TIMER_ERROR;
 	}
	switch (cthis->m_state)
	{
		case EMAILSENDER_STATUS_NONE:
			rv = cthis->Connect(cthis);
			if (rv == EMAILSENDER_OK)
			{
				printf("EmailSender connect ok.\r\n");
				cthis->m_state = EMAILSENDER_STATUS_CONNECTING;
			}
			else if (rv == EMAILSENDER_ERROR)
			{
				printf("EmailSender connect failed.\r\n");
				return EMAILSENDER_TIMER_ERROR;
			}
			else
			{
				return EMAILSENDER_TIMER_FREE;
			}
			break;
		case EMAILSENDER_STATUS_CONNECTING:
			rv = cthis->ConnetProbe(cthis);
			if (rv == EMAILSENDER_OK)
			{
				printf("EmailSender ConnetProbe ok.\r\n");
				cthis->m_state = EMAILSENDER_STATUS_SENDHEADCOMAND;
			}
			else if (rv == EMAILSENDER_ERROR)
			{
				printf("EmailSender ConnetProbe failed.\r\n");
				return EMAILSENDER_TIMER_ERROR;
			}
			else
			{
				return EMAILSENDER_TIMER_FREE;
			}
			break;
		case EMAILSENDER_STATUS_SENDHEADCOMAND:
			rv = cthis->SendHead(cthis);
			if (rv == EMAILSENDER_ERROR)
			{
				printf("EmailSender SendHead failed.\r\n");
				return EMAILSENDER_TIMER_ERROR;
			}
			else if(rv == EMAILSENDER_WOULDBLOCK)
			{
				return EMAILSENDER_TIMER_FREE;
			}
			else //EMAILSENDER_OK
			{			
				printf("EmailSender SendHead ok.\r\n");
				cthis->m_state = EMAILSENDER_STATUS_SENDDATA;
			}
			break;
		case EMAILSENDER_STATUS_SENDDATA:
			rv = cthis->SendData(cthis);
			if (rv == EMAILSENDER_ERROR)
			{
				printf("EmailSender SendData failed.\r\n");
				return EMAILSENDER_TIMER_ERROR;
			}
			else if (rv == EMAILSENDER_WOULDBLOCK)
			{
				return EMAILSENDER_TIMER_FREE;
			}
			else //EMAILSENDER_OK
			{
				printf("EmailSender SendData ok.\r\n");
				return EMAILSENDER_TIMER_OK;
			}
		default:
			return EMAILSENDER_TIMER_ERROR;
	}
	return EMAILSENDER_TIMER_BUSY;
}

static void Report(TEmailSender *cthis, int (*pPrn)(const char *,...))
{
	struct in_addr in;
	in.s_addr = cthis->m_serverIP;
	
	pPrn("==============================EmailSender Report Begin=========================\r\n");
	pPrn("\tthis=%08X\r\n", cthis);
	pPrn("\tm_state: %d\r\n", cthis->m_state);
	pPrn("\tm_sock: %d\r\n", cthis->m_sock);
	pPrn("\tm_bFirstsend: %d\r\n", cthis->m_bFirstsend);
	pPrn("\tm_bSendHead: %d\r\n", cthis->m_bSendHead);
	pPrn("\tm_sendCmdType: %d\r\n", cthis->m_sendCmdType);
	pPrn("\tm_serverIP: %s\r\n", inet_ntoa(in));
	pPrn("\tm_uiCreateTime: %08X\r\n", cthis->m_uiCreateTime);
	pPrn("==============================EmailSender Report End=========================\r\n");
}

const TEmailSender EMAILSENDER = 
{
	&Construct,
	&Destroy,
 	&Create,
	&OnTimer,
	&Report,

	&Connect,
	&ConnetProbe,
	&SendData,
	&SendHead
};
