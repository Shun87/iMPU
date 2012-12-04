/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: FtpDownloader.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xuejiabin
 *  完成日期: 2007-7-12 12:00:50
 *  修正备注: 
 *  
 */
#include "../../XOS/XOS.h"
#include "../FtpDownloader.h"
#include "../DNS.h"

static char *s_szfLoginHead="USER %s\r\n\
PASS %s\r\n\
PWD\r\n";

#define SENDCMDTYPE_NONE			0
#define SENDCMDTYPE_USERNAME		1
#define SENDCMDTYPE_PASSWORD		2
#define SENDCMDTYPE_PWD				3
#define SENDCMDTYPE_CWDPATH			5
#define SENDCMDTYPE_GETSIZE			6
#define SENDCMDTYPE_ALLO1			7
#define SENDCMDTYPE_SIZE			8
#define SENDCMDTYPE_APPE			9

static void Construct(TFtpDownloader *cthis)
{
	cthis->m_state = FTPDOWNLOADER_STATUS_NONE;
	cthis->m_sock = XOS_INVALID_SOCKET;
	cthis->m_sockRecv = XOS_INVALID_SOCKET;

	cthis->m_pszServerURL = NULL;
	cthis->m_pszSendHead = NULL;		//通过m_pszHeadAddress释放
	cthis->m_pszHeadAddress = NULL;
	cthis->m_pszFilePath = NULL;
	cthis->m_pszFileName = NULL;
	cthis->m_pszFullFileName = NULL;
	cthis->m_pszRecvData = NULL;
	cthis->m_pszRecvBuf = NULL;

	cthis->m_serverIP = 0;
	cthis->m_recvlen = 0;
	cthis->m_iRecvDatalen = 0;
	cthis->m_sendHeadLen = 0;
	cthis->m_bSendHead = XOS_TRUE;
	cthis->m_bFirstsend = XOS_TRUE;
}

static void Destroy(TFtpDownloader *cthis)
{
	if (cthis->m_sock != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(cthis->m_sock);
	}
	if (cthis->m_sockRecv != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(cthis->m_sockRecv);
	}
	if (cthis->m_pszServerURL = NULL)
	{
		free(cthis->m_pszServerURL);
	}
	if (cthis->m_pszFileName != NULL)
	{
		free(cthis->m_pszFileName);
	}
	if (cthis->m_pszFilePath !=NULL)
	{
		free(cthis->m_pszFilePath);
	}
	if (cthis->m_pszHeadAddress != NULL)
	{
		free(cthis->m_pszHeadAddress);
	}
	if (cthis->m_pszFullFileName != NULL)
	{
		free(cthis->m_pszFullFileName);
	}
	if (cthis->m_pszRecvData != NULL)
	{
		free(cthis->m_pszRecvData);
	}
	if (cthis->m_pszRecvBuf != NULL)
	{
		free(cthis->m_pszRecvBuf);
	}
}

static int SendComAndGetRecv(TFtpDownloader *cthis)
{
	int rv;
	if (cthis->m_sendHeadLen != cthis->m_haveSendLen)
	{
		rv = XOS_TCPSendDataNB(cthis->m_sock, cthis->m_pszSendHead, &(cthis->m_haveSendLen), cthis->m_sendHeadLen);
		if(rv != 0)
		{
			if (rv == 2)
			{				
				cthis->m_uiCreateTime = XOS_GetTickCount();
			}
			if (rv < 0)
			{
				return FTPDOWNLOADER_ERROR;
			}
			else
			{
				return FTPDOWNLOADER_WOULDBLOCK;
			}
		}
		else
		{
			cthis->m_sendHeadLen = cthis->m_haveSendLen;
			cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
			memset(cthis->m_pszRecvData, 0, FTPDOWNLOADER_HEAD_LEN);
			cthis->m_recvlen = 0;
		}
	}
	rv = XOS_TCPRecvHeadNB(cthis->m_sock, cthis->m_pszRecvDataAddress, FTPDOWNLOADER_HEAD_LEN,
		&(cthis->m_recvlen),"\r\n");
	if (rv != 0)
	{
		if (rv == 2)
		{				
			cthis->m_uiCreateTime = XOS_GetTickCount();
		}
		if (rv < 0)
		{
			return FTPDOWNLOADER_ERROR;
		}
		else
		{
			return FTPDOWNLOADER_WOULDBLOCK;
		}
	}
	return FTPDOWNLOADER_OK;	
}

static XOS_BOOL Create(TFtpDownloader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword,
				   char* lpszServerPath, char* lpszFileName, char* FullFileName)
{
	cthis->m_uiCreateTime = XOS_GetTickCount();
	
	cthis->m_pszSendHead = (char *)malloc(FTPDOWNLOADER_HEAD_LEN);
	memset(cthis->m_pszSendHead, 0, FTPDOWNLOADER_HEAD_LEN);
	if (cthis->m_pszSendHead == NULL)
	{
		return XOS_FALSE;
	}
	cthis->m_pszHeadAddress = cthis->m_pszSendHead;
	cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);

	cthis->m_pszUserName= (char *)malloc(strlen(lpszUserName)+2);
	if (cthis->m_pszUserName == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszUserName, 0, strlen(lpszUserName)+2);
	memcpy(cthis->m_pszUserName, lpszUserName, strlen(lpszUserName));

	cthis->m_pszPassword = (char *)malloc(strlen(lpszPassword)+2);
	if (cthis->m_pszPassword == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszPassword, 0, strlen(lpszPassword)+2);
	memcpy(cthis->m_pszPassword, lpszPassword, strlen(lpszPassword));

	cthis->m_pszFilePath = (char *)malloc(strlen(lpszServerPath)+2);
	if (cthis->m_pszFilePath == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszFilePath, 0, strlen(lpszServerPath)+2);
	memcpy(cthis->m_pszFilePath, lpszServerPath, strlen(lpszServerPath));
	
	cthis->m_pszFullFileName = (char *)malloc(strlen(FullFileName)+2);
	if (cthis->m_pszFullFileName == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszFullFileName, 0, strlen(FullFileName)+2);
	memcpy(cthis->m_pszFullFileName, FullFileName, strlen(FullFileName));

	cthis->m_pszFileName = (char *)malloc(strlen(lpszFileName)+1);
	if (cthis->m_pszFileName == NULL)
	{
		return XOS_FALSE;
	}
	memcpy(cthis->m_pszFileName, lpszFileName, strlen(lpszFileName)+1);

	cthis->m_pszRecvData = (char *)malloc(FTPDOWNLOADER_HEAD_LEN);
	if (cthis->m_pszFileName == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszRecvData, 0, FTPDOWNLOADER_HEAD_LEN);
	cthis->m_pszRecvDataAddress = cthis->m_pszRecvData;

	cthis->m_pszRecvBuf = (char*)malloc(FTPDOWNLOADER_BUFFER_LEN);
	if (cthis->m_pszFileName == NULL)
	{
		return XOS_FALSE;
	}

	cthis->m_pszServerURL = (char *)malloc(strlen(lpszFtpServer)+1);
	if(cthis->m_pszServerURL == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszServerURL, 0, strlen(lpszFtpServer));
	strcpy(cthis->m_pszServerURL, lpszFtpServer);

	printf("FtpDownloader has created\r\n");

	return XOS_TRUE;
}

static XOS_S32 Connect(TFtpDownloader *cthis)
{
    int rv = 0;
	int n_reuseaddr = 1;
	unsigned short usPort = 21;	 
	if (cthis->m_serverIP == 0)
	{
		if (!Addr2IPAndPort(cthis->m_pszServerURL, &cthis->m_serverIP, &usPort, 1))
		{
			return FTPDOWNLOADER_WOULDBLOCK;
		}
	}

	cthis->m_sock = XOS_TCPConnectNB(cthis->m_serverIP, usPort);
	if (cthis->m_sock == XOS_INVALID_SOCKET) 
	{
		printf("create ftpDownloader connect failed!\r\n");
		return FTPDOWNLOADER_ERROR;
	}
	cthis->m_recvlen = 0;
	return FTPDOWNLOADER_OK;
}

static XOS_S32 ConnetProbe(TFtpDownloader *cthis)
{
	XOS_S32 rv = 0;
	rv = XOS_TCPConnectProbe(cthis->m_sock);
	if (rv == 0)
	{
		rv = XOS_TCPRecvHeadNB(cthis->m_sock, cthis->m_pszRecvDataAddress, FTPDOWNLOADER_HEAD_LEN,
			&(cthis->m_recvlen),"\r\n");
		if (rv == 0)
		{	
			cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
			strtok(cthis->m_pszRecvData, " ");
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_SERVER_OK) 
			{
				cthis->m_SendCmdType = SENDCMDTYPE_USERNAME;
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				memset(cthis->m_pszSendHead, 0, FTPDOWNLOADER_HEAD_LEN);
				sprintf(cthis->m_pszSendHead, "USER %s\r\n", cthis->m_pszUserName);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen =0;
				cthis->m_SendCmdType = SENDCMDTYPE_USERNAME;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				printf("the connect to server is error!\r\n");
				return FTPDOWNLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			return FTPDOWNLOADER_OK;
		}
		else
		{
			return FTPDOWNLOADER_ERROR;
		}
	}
	else if (rv == 1)
	{
		return FTPDOWNLOADER_WOULDBLOCK;
	}
	else if (rv == 2)
	{				
		cthis->m_uiCreateTime = XOS_GetTickCount();
		return FTPDOWNLOADER_WOULDBLOCK;
	}
	else
	{
		return FTPDOWNLOADER_ERROR;
	}
}

static XOS_S32 SendHead(TFtpDownloader *cthis)
{
	int rv = 0;
	int recvlen = 0;
	int iSendDataLen = 0;
	while(1)
	{
		rv = SendComAndGetRecv(cthis);
		if (rv != FTPDOWNLOADER_OK)
		{
			return rv;
		}
		cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
		strtok(cthis->m_pszRecvData, " ");
		
		switch(cthis->m_SendCmdType)
		{
		case SENDCMDTYPE_USERNAME:
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_USRNAME_OK) 
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;				
				cthis->m_pszSendHead = cthis->m_pszHeadAddress;
				memset(cthis->m_pszSendHead, 0, FTPDOWNLOADER_HEAD_LEN);
				sprintf(cthis->m_pszSendHead, "PASS %s\r\n", cthis->m_pszPassword);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_PASSWORD;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;					
				printf("the username is error!\r\n");
				return FTPDOWNLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			break;
		case SENDCMDTYPE_PASSWORD:
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_LOGIN_OK) 
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				cthis->m_pszSendHead = cthis->m_pszHeadAddress;
				memset(cthis->m_pszSendHead, 0, FTPDOWNLOADER_HEAD_LEN);
				sprintf(cthis->m_pszSendHead, "PWD\r\n");
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_PWD;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				printf("the password is error!\r\n");
				return FTPDOWNLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			break;
		case SENDCMDTYPE_PWD:
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_PWD_RECV_OK) 
			{
				memset(cthis->m_pszHeadAddress, 0, FTPDOWNLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszHeadAddress;
				sprintf(cthis->m_pszSendHead,"CWD %s\r\n", cthis->m_pszFilePath); 
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);	//为探测路径准备数据
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_CWDPATH;
				return FTPDOWNLOADER_OK;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				return FTPDOWNLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			return FTPDOWNLOADER_OK;
			break;
		default:
			return FTPDOWNLOADER_ERROR;
		}
	}
	
}

static XOS_S32 ExecuteDirectory(TFtpDownloader *cthis)
{	
	int rv = 0;
	while(1)
	{
		rv = SendComAndGetRecv(cthis);
		if (rv != FTPDOWNLOADER_OK)
		{
			return rv;
		}
		cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
		strtok(cthis->m_pszRecvData, " ");
		switch(cthis->m_SendCmdType)
		{
		case SENDCMDTYPE_CWDPATH:
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_CWD_RECV_OK)
			{
				memset(cthis->m_pszHeadAddress, 0, FTPDOWNLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszHeadAddress;
				sprintf(cthis->m_pszSendHead, "TYPE I\r\n",
					cthis->m_pszFilePath, cthis->m_pszFileName); 
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_ALLO1;
			}
			else
			{
				printf("the path is error!\r\n");
				return FTPDOWNLOADER_ERROR;
			}
			break;
		case SENDCMDTYPE_ALLO1:
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_ALLO_RECV_OK_2)
			{
				memset(cthis->m_pszHeadAddress, 0, FTPDOWNLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszHeadAddress;
				sprintf(cthis->m_pszSendHead, "SIZE %s\r\n", cthis->m_pszFileName); 
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead); 
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_GETSIZE;
			}
			else
			{
				printf("the path is error!\r\n");
				return FTPDOWNLOADER_ERROR;
			}
			break;
		case SENDCMDTYPE_GETSIZE:
			if (atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_SIZE_RECV_OK) 
			{
				cthis->m_iRecvDatalen = atoi(strtok(NULL, "\r\n"));
				cthis->m_haveSendLen = 0;
				return FTPDOWNLOADER_OK;
			}
			else
			{
				printf("the path is error!\r\n");
				return FTPDOWNLOADER_ERROR;
			}
			break;
		default:
			return FTPDOWNLOADER_ERROR;
		}
	}
	
	return FTPDOWNLOADER_OK;
}

static XOS_S32 FtpDownloaderOpenFile(TFtpDownloader *cthis)
{
	int rv = 0;
	int recvlen = 0;
	int iSendDatalen = 0;
	rv = SendComAndGetRecv(cthis);
	if (rv != FTPDOWNLOADER_OK)
	{
		return rv;
	}
	cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
	strtok(cthis->m_pszRecvData, " ");
	if((atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_APPE_RECV_OK))						
	{
		memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
		cthis->m_recvlen = 0;
		return 	FTPDOWNLOADER_OK;
	}
	else
	{
		printf("FtpDownloader open file failed !\r\n");
		return FTPDOWNLOADER_ERROR;
	}
}

static XOS_S32 CreateDataReceiveSocket(TFtpDownloader *cthis)
{
	int rv = 0;
	int recvlen = 0;
	int a0, a1, a2, a3, p0, p1;
	char *a,*p;
	struct sockaddr_in data_addr;
	rv = SendComAndGetRecv(cthis);
	if (rv != FTPDOWNLOADER_OK)
	{
		return rv;
	}
	cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
	if(atoi(cthis->m_pszRecvData) == FTPDOWNLOADER_PASV_RECV_OK)  
	{						
		if (sscanf(cthis->m_pszRecvData,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
				   &a0, &a1, &a2, &a3, &p0, &p1) != 6) 
		{				
			printf("Passive mode address scan failure. Shouldn't happen!\r\n");
			return -1;
		}
		//printf("%d,%d,%d,%d,%d,%d\n",a0,a1,a2,a3,p0,p1);
		data_addr.sin_family = AF_INET;
		a = (char *)&data_addr.sin_addr.s_addr;
		a[0] = a0 & 0xff;
		a[1] = a1 & 0xff;
		a[2] = a2 & 0xff;
		a[3] = a3 & 0xff;
		p = (char *)&data_addr.sin_port;
		p[0] = p0 & 0xff;
		p[1] = p1 & 0xff;
		
		//client请求连接a, p为远程主机IP地址和端口

		cthis->m_sockRecv = XOS_TCPConnectNB(*((XOS_U32 *)a), ntohs(*(unsigned short*)p));					
		if (cthis->m_sockRecv == XOS_INVALID_SOCKET)
		{
			printf("create ftp_senddate connect failed\r\n");
			return FTPDOWNLOADER_ERROR;
		}
		else
		{
			memset(cthis->m_pszHeadAddress, 0, FTPDOWNLOADER_HEAD_LEN);
			sprintf(cthis->m_pszHeadAddress, "RETR %s\r\n", cthis->m_pszFileName);		
			cthis->m_pszSendHead = cthis->m_pszHeadAddress;
			cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
			cthis->m_haveSendLen = 0;
			memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
			cthis->m_recvlen = 0;
			return FTPDOWNLOADER_OK;
		}
		sprintf(cthis->m_pszHeadAddress, "RETR %s\r\n", cthis->m_pszFileName);		
		cthis->m_pszSendHead = cthis->m_pszHeadAddress;
		cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
		cthis->m_haveSendLen = 0;
		memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
		cthis->m_recvlen = 0;
		return FTPDOWNLOADER_OK;
	}
	else 
	{
		
		printf("the create recvSocket error!\r\n");
		return FTPDOWNLOADER_ERROR;
	}
}

static XOS_S32 DataReceiveConnetProbe(TFtpDownloader *cthis)
{
	int rv;
	FILE * pfile;
	rv = XOS_TCPConnectProbe(cthis->m_sockRecv);
	if (rv == 0)
	{
		memset(cthis->m_pszRecvData, 0, cthis->m_recvlen);
		cthis->m_recvlen = 0;
		//看看本地是否有这个文件了,如果有则删除
		if (pfile = fopen(cthis->m_pszFullFileName, "rb"))
		{
			fclose(pfile);
			remove(cthis->m_pszFullFileName);
		}
		return FTPDOWNLOADER_OK;
	}
	else if (rv == 1)
	{
		return FTPDOWNLOADER_WOULDBLOCK;
	}
	else if (rv == -1)
	{
		printf("vap tcp connector select failed:\r\n");
		return FTPDOWNLOADER_ERROR;
	}
	return FTPDOWNLOADER_OK;
}

static XOS_S32 ReceiveData(TFtpDownloader *cthis)
{
	int rv = 0;
	FILE *pfile;
	while (1)
	{
		rv = XOS_TCPRecvNB(cthis->m_sockRecv, cthis->m_pszRecvBuf, FTPDOWNLOADER_BUFFER_LEN);
		if ( rv == -1)
		{
			printf("FtpDownLoader ReceiveData error\r\n");
			return FTPDOWNLOADER_ERROR;
		}
		else if (rv == 0)
		{
			return FTPDOWNLOADER_WOULDBLOCK;
		}

		cthis->m_uiCreateTime = XOS_GetTickCount();
		pfile = fopen(cthis->m_pszFullFileName, "ab+");
		if (pfile == NULL)
		{
			printf("FtpDownLoader ReceiveData error\r\n");
			memset(cthis->m_pszRecvBuf, 0, FTPDOWNLOADER_BUFFER_LEN);
			return FTPDOWNLOADER_ERROR;
		}
		fwrite(cthis->m_pszRecvBuf, 1, rv, pfile);
		fclose(pfile);
		memset(cthis->m_pszRecvBuf, 0, FTPDOWNLOADER_BUFFER_LEN);
		cthis->m_iRecvDatalen -= rv;
		if (cthis->m_iRecvDatalen == 0)
		{
			return FTPDOWNLOADER_OK;
		}
	}
	return FTPDOWNLOADER_ERROR;
}

static XOS_S32 OnTimer(TFtpDownloader *cthis)
{
	XOS_S32 rv;
	//检查连接是否超时
	if ((XOS_GetTickCount() - cthis->m_uiCreateTime) > 60*1000)
	{
		return FTPDOWNLOADER_TIMER_ERROR;
 	}
	switch (cthis->m_state)
	{
		case FTPDOWNLOADER_STATUS_NONE:
			rv = cthis->Connect(cthis);
			if (rv  == FTPDOWNLOADER_OK)
			{
				cthis->m_state = FTPDOWNLOADER_STATUS_CONNECTING;
				printf("ftpDownloader connect ok.\r\n");
			}
			else if (rv == FTPDOWNLOADER_ERROR)
			{
				printf("ftpDownloader connect failed.\r\n");
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			break;
		case FTPDOWNLOADER_STATUS_CONNECTING:
			rv = cthis->ConnetProbe(cthis);
			if (rv == FTPDOWNLOADER_OK)
			{
				printf("ftp connet probe ok.\r\n");
				cthis->m_state = FTPDOWNLOADER_STATUS_SENDHEADCOMAND;
			}
			else if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else //FTPDOWNLOADER_WOULDBLOCK
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			break;
		case FTPDOWNLOADER_STATUS_SENDHEADCOMAND:
			rv = cthis->SendHead(cthis);
			if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else if(rv == FTPDOWNLOADER_WOULDBLOCK)
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			else //FTPDOWNLOADER_OK
			{			
				printf("ftp send head ok.\r\n");
				cthis->m_state = FTPDOWNLOADER_STATUS_EXECUTEDIRCTORY;
			}
			break;
		case FTPDOWNLOADER_STATUS_EXECUTEDIRCTORY:
			rv = cthis->ExecuteDirectory(cthis);
			if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else if(rv == FTPDOWNLOADER_WOULDBLOCK)
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			else //FTPDOWNLOADER_OK
			{			
				memset(cthis->m_pszHeadAddress, 0, FTPDOWNLOADER_HEAD_LEN);
				memcpy(cthis->m_pszHeadAddress, "PASV\r\n", strlen("PASV\r\n"));		
				cthis->m_pszSendHead = cthis->m_pszHeadAddress;
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead) +1;
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				cthis->m_haveSendLen = 0;
				cthis->m_state = FTPDOWNLOADER_STATUS_CREATSENDDATASOCKTE;
				printf("ftp Downloader ExecuteDirectory is ok. %d\r\n", __LINE__);
			}
			break;
		case FTPDOWNLOADER_STATUS_OPENFILE:
			rv = cthis->FtpDownloaderOpenFile(cthis);
			if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else if(rv == FTPDOWNLOADER_WOULDBLOCK)
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			else //FTPDOWNLOADER_OK
			{			
				cthis->m_state = FTPDOWNLOADER_STATUS_DATASENDCONNECTING;
				printf("ftp Downloader file open ok. %d\r\n", __LINE__);
			}
			break;
		case FTPDOWNLOADER_STATUS_CREATSENDDATASOCKTE:
			rv = cthis->CreateDataReceiveSocket(cthis);
			if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else if(rv == FTPDOWNLOADER_WOULDBLOCK)
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			else //FTPDOWNLOADER_OK
			{			
				printf("ftp create data receive socket ok. %d\r\n", __LINE__);
				cthis->m_state = FTPDOWNLOADER_STATUS_OPENFILE;
			}
			break;
		case FTPDOWNLOADER_STATUS_DATASENDCONNECTING:
			rv = cthis->DataReceiveConnetProbe(cthis);
			if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else if(rv == FTPDOWNLOADER_WOULDBLOCK)
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			else //EMAILSENDER_OK
			{			
				printf("ftp data receive connect probe ok. %d\r\n", __LINE__);
				cthis->m_state = FTPDOWNLOADER_STATUS_RECVDATA;
			}
			break;
		case FTPDOWNLOADER_STATUS_RECVDATA:
			rv = cthis->ReceiveData(cthis);
			if (rv == FTPDOWNLOADER_ERROR)
			{
				return FTPDOWNLOADER_TIMER_ERROR;
			}
			else if (rv == FTPDOWNLOADER_WOULDBLOCK)
			{
				return FTPDOWNLOADER_TIMER_FREE;
			}
			else //EMAILSENDER_OK
			{
				printf("ftp downlaod ok. %d\r\n", __LINE__);
				return FTPDOWNLOADER_TIMER_OK;				
			}
			break;
		default:
			return FTPDOWNLOADER_TIMER_ERROR;
	}
	return FTPDOWNLOADER_TIMER_BUSY;
}

static void Report(TFtpDownloader *cthis, int (*pPrn)(const char *,...))
{
	struct in_addr in;
	in.s_addr = cthis->m_serverIP;
	
	pPrn("==============================FTPDOWNLOADER Report Begin=========================\r\n");
	pPrn("\tthis=%08X\r\n", cthis);
	pPrn("\tm_state: %d\r\n", cthis->m_state);
	pPrn("\tm_sock: %d\r\n", cthis->m_sock);
	pPrn("\tm_sockRecv: %d\r\n", cthis->m_sockRecv);
	pPrn("\tFirstsend: %d\r\n", cthis->m_bFirstsend);
	pPrn("\tB_sendhead: %d\r\n", cthis->m_bSendHead);
	pPrn("\tm_serverIP: %s\r\n", inet_ntoa(in));
	pPrn("\tplszfilePath: %s\r\n", cthis->m_pszFilePath);
	pPrn("\tplszfileName: %s\r\n", cthis->m_pszFileName);
	pPrn("\tm_sendCmdType: %d\r\n", cthis->m_SendCmdType);
	pPrn("\tm_uiCreateTime: %08X\r\n", cthis->m_uiCreateTime);
	pPrn("==============================FTPDOWNLOADER Report End=========================\r\n");
}

const TFtpDownloader FTPDOWNLOADER = 
{
	&Construct,
	&Destroy,
	&Create,
	&OnTimer,
	&Report,

	&Connect,
	&ConnetProbe,
	&ReceiveData,
	&SendHead,
	&ExecuteDirectory,
	&FtpDownloaderOpenFile,
	&CreateDataReceiveSocket,
	&DataReceiveConnetProbe
};







