/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: FtpUploader.c
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
#include "../FtpUploader.h"
#include "../DNS.h"

#define SENDCMDTYPE_NONE			0
#define SENDCMDTYPE_USERNAME		1
#define SENDCMDTYPE_PASSWORD		2
#define SENDCMDTYPE_PWD				3
#define SENDCMDTYPE_CWDPATH			5
#define SENDCMDTYPE_MAKEPATH		6
#define SENDCMDTYPE_ALLO1			7
#define SENDCMDTYPE_SIZE			8
#define SENDCMDTYPE_APPE			9
#define SENDCMDTYPE_DELE			10

static void Construct(TFtpUploader *cthis)
{
	cthis->m_state = FTPUPLOADER_STATUS_NONE;
	cthis->m_SendCmdType = SENDCMDTYPE_NONE;
	cthis->m_sock = XOS_INVALID_SOCKET;
	cthis->m_sockSend = XOS_INVALID_SOCKET;

	cthis->m_pszServerURL = NULL;
	cthis->m_pSendBuffer = NULL;			//分配之后由m_pSendBufferAddress释放
	cthis->m_pSendBufferAddress = NULL;
	cthis->m_pszSendHead = NULL;			//分配之后由m_pszSendHeadAddress释放
	cthis->m_pszSendHeadAddress = NULL;
	cthis->m_pszFilePath = NULL;			//分配后自己释放
	cthis->m_pszFilePathAddress = NULL;		//这个做计算偏移用
	cthis->m_pszFileName = NULL;
	cthis->m_pszRecvData = NULL;
	cthis->m_pszPath = NULL;
	cthis->m_pszUserName = NULL;
	cthis->m_pszPassword = NULL;

	cthis->m_sendDataLen = 0;
	cthis->m_sendHeadLen = 0;
	cthis->m_serverIP = 0;
	cthis->m_recvlen = 0;
	cthis->m_bCreatePath = XOS_FALSE;
	cthis->m_bDirectory = XOS_FALSE;
	cthis->m_bEndString = XOS_FALSE;
	cthis->m_bCanWriteData = XOS_FALSE;
}

static void Destroy(TFtpUploader *cthis)
{
	if (cthis->m_sock != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(cthis->m_sock);
	}
	if (cthis->m_sockSend != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(cthis->m_sockSend);
	}
	if (cthis->m_pszServerURL = NULL)
	{
		free(cthis->m_pszServerURL);
	}
	if(cthis->m_pszFileName != NULL)
	{
		free(cthis->m_pszFileName);
	}
	if (cthis->m_pszFilePath !=NULL)
	{
		free(cthis->m_pszFilePath);
	}
	if (cthis->m_pszRecvData != NULL)
	{
		free(cthis->m_pszRecvData);
	}
	if (cthis->m_pszPath != NULL)
	{
		free(cthis->m_pszPath);
	}
	if (cthis->m_pszUserName != NULL)
	{
		free(cthis->m_pszUserName);
	}	
	if (cthis->m_pszSendHeadAddress != NULL)
	{
		free(cthis->m_pszSendHeadAddress);
	}
	
	if (cthis->m_pszPassword != NULL)
	{
		free(cthis->m_pszPassword);
	}
	if (cthis->m_pSendBufferAddress != NULL)
	{
		free(cthis->m_pSendBufferAddress);
	}
	
}

static XOS_BOOL Create(TFtpUploader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword, char* lpszServerPath, char* lpszFileName, long nSendLen, char *pSendBuffer)
{
	char *pBuffer = (char*)malloc(nSendLen);
	if (pBuffer == NULL)
	{
		return XOS_FALSE;
	}

	// 为NULL说明创建可以持续写入数据的Uploader
	if (pSendBuffer == NULL)
	{
		cthis->m_bCanWriteData = XOS_TRUE;
		cthis->m_iMaxBufferLen = nSendLen;
		nSendLen = 0;
	}
	else
	{
		memcpy(pBuffer, pSendBuffer, nSendLen);
	}

    cthis->m_pSendBufferAddress = pBuffer;
	return cthis->Create2(cthis, lpszFtpServer, lpszUserName, lpszPassword, lpszServerPath, lpszFileName, nSendLen, pBuffer);
}

static XOS_BOOL Create2(TFtpUploader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword, char* lpszServerPath, char* lpszFileName, long nSendLen, char *pSendBuffer)
{
	cthis->m_uiCreateTime = XOS_GetTickCount();
	
	cthis->m_pszSendHead = (char *)malloc(FTPUPLOADER_HEAD_LEN+1);
	memset(cthis->m_pszSendHead, 0, FTPUPLOADER_HEAD_LEN);
	if (cthis->m_pszSendHead == NULL)
	{
		return XOS_FALSE;
	}
	cthis->m_pszSendHeadAddress = cthis->m_pszSendHead;
	cthis->m_sendHeadLen = 0;

	cthis->m_pszFilePath = (char *)malloc(strlen(lpszServerPath)+2);
	if (cthis->m_pszFilePath == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszFilePath, 0, strlen(lpszServerPath)+2);
	if (lpszServerPath[0] == '/')
	{
		strcpy(cthis->m_pszFilePath, lpszServerPath);
	}
	else
	{
		sprintf(cthis->m_pszFilePath, "/%s" , lpszServerPath);
	}
	cthis->m_pszFilePathAddress = cthis->m_pszFilePath;
	
	cthis->m_pszFileName = (char *)malloc(strlen(lpszFileName)+1);
	if (cthis->m_pszFileName == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszFileName, 0, strlen(lpszFileName)+1);
	strcpy(cthis->m_pszFileName, lpszFileName);

	cthis->m_pszUserName = (char *)malloc(strlen(lpszUserName)+2);
	if (cthis->m_pszUserName == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszUserName, 0, strlen(lpszUserName)+1);
	strcpy(cthis->m_pszUserName, lpszUserName);

	
	cthis->m_pszPassword = (char *)malloc(strlen(lpszPassword)+2);
	if (cthis->m_pszPassword == NULL)
	{
		return XOS_FALSE;
	}
	memset(cthis->m_pszPassword, 0, strlen(lpszPassword)+1);
	strcpy(cthis->m_pszPassword, lpszPassword);

	cthis->m_pSendBuffer = pSendBuffer;
	cthis->m_sendDataLen = nSendLen;

	cthis->m_pszRecvData = (char *)malloc(FTPUPLOADER_HEAD_LEN);
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

	printf("FtpUploader has created ok\r\n");

	return XOS_TRUE;
}

static int SendComAndGetRecv(TFtpUploader *cthis)
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
				return FTPUPLOADER_ERROR;
			}
			else
			{
				return FTPUPLOADER_WOULDBLOCK;
			}
		}
		else
		{
			cthis->m_sendHeadLen = cthis->m_haveSendLen;// XOS_TCPSendDataNB发送完后会把*piSendLen置0，影响上面的判断
			cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
			memset(cthis->m_pszRecvData, 0, FTPUPLOADER_HEAD_LEN);
			cthis->m_recvlen = 0;
		}
	}

	rv = XOS_TCPRecvHeadNB(cthis->m_sock, cthis->m_pszRecvData, FTPUPLOADER_HEAD_LEN,
		&(cthis->m_recvlen),"\r\n");
	if (rv != 0)
	{
		if (rv == 2)
		{				
			cthis->m_uiCreateTime = XOS_GetTickCount();
		}
		if (rv < 0)
		{
			return FTPUPLOADER_ERROR;
		}
		else
		{
			return FTPUPLOADER_WOULDBLOCK;
		}
	}

	// 收到了"\r\n",过滤掉非FTP响应命令
	if (cthis->m_pszRecvData[3] != ' ')
	{
		cthis->m_recvlen = 0;
		return FTPUPLOADER_WOULDBLOCK;
	}

	return FTPUPLOADER_OK;
}

static XOS_S32 Connect(TFtpUploader *cthis)
{
    XOS_S32 rv = 0;
	XOS_S16 usPort = 21;
	if (cthis->m_serverIP == 0)
	{
		if (!Addr2IPAndPort(cthis->m_pszServerURL, &cthis->m_serverIP, &usPort, XOS_TRUE))
		{
			return FTPUPLOADER_WOULDBLOCK;
		}
	}

	cthis->m_sock = XOS_TCPConnectNB(cthis->m_serverIP, usPort);
	if (cthis->m_sock != XOS_INVALID_SOCKET)
	{
		cthis->m_uiCreateTime = XOS_GetTickCount();		
		cthis->m_pszRecvDataAddress = cthis->m_pszRecvData;
		cthis->m_recvlen = 0;
		return FTPUPLOADER_OK;
	}
	else
	{
		return FTPUPLOADER_ERROR;
	}
}

static XOS_S32 ConnetProbe(TFtpUploader *cthis)
{
	int rv;
	rv = XOS_TCPConnectProbe(cthis->m_sock);
	if (rv == 0)
	{
		rv = XOS_TCPRecvHeadNB(cthis->m_sock, cthis->m_pszRecvDataAddress, FTPUPLOADER_HEAD_LEN,
			&(cthis->m_recvlen), "\r\n");
		if (rv == 0)
		{	
			cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
			// 收到了"\r\n",过滤掉非FTP响应命令
			rv = atoi(cthis->m_pszRecvData);
			if (cthis->m_pszRecvData[3] != ' ' || rv < 100 || rv > 999)
			{
				cthis->m_recvlen = 0;
				return FTPUPLOADER_WOULDBLOCK;
			}

			strtok(cthis->m_pszRecvData, " ");
			if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_SERVER_OK) 
			{
				cthis->m_SendCmdType = SENDCMDTYPE_USERNAME;
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				memset(cthis->m_pszSendHead, 0, FTPUPLOADER_HEAD_LEN);
				sprintf(cthis->m_pszSendHead, "USER %s\r\n", cthis->m_pszUserName);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				return FTPUPLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
		}
		else if (rv == 1)
		{
			return FTPUPLOADER_WOULDBLOCK;
		}
		else if (rv == 2)
		{
			cthis->m_uiCreateTime = XOS_GetTickCount();
			return FTPUPLOADER_WOULDBLOCK;
		}
		else
		{
			return FTPUPLOADER_ERROR;
		}
		return FTPUPLOADER_OK;
	}
	else if (rv == 1)
	{
		return FTPUPLOADER_WOULDBLOCK;
	}
	else 
	{
		printf(" tcpUploader ConnetProbe error!\r\n");
		return FTPUPLOADER_ERROR;
	}
}

static XOS_S32 SendHead(TFtpUploader *cthis)
{
	int rv;
	while (1)
	{	
		rv = SendComAndGetRecv(cthis);
		if (rv != FTPUPLOADER_OK)
		{
			return rv;
		}
		cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
		strtok(cthis->m_pszRecvData, " ");

		switch(cthis->m_SendCmdType)
		{
		case SENDCMDTYPE_USERNAME:
			if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_USRNAME_OK) 
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				memset(cthis->m_pszSendHead, 0, FTPUPLOADER_HEAD_LEN);
				sprintf(cthis->m_pszSendHead, "PASS %s\r\n", cthis->m_pszPassword);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_PASSWORD;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				return FTPUPLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			break;
		case SENDCMDTYPE_PASSWORD:
			if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_LOGIN_OK) 
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				memset(cthis->m_pszSendHead, 0, FTPUPLOADER_HEAD_LEN);
				sprintf(cthis->m_pszSendHead, "PWD\r\n");
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_PWD;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				return FTPUPLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			break;
		case SENDCMDTYPE_PWD:
			if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_PWD_RECV_OK) 
			{
				memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
				sprintf(cthis->m_pszSendHead,"CWD %s\r\n", cthis->m_pszFilePath); 
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);	//为探测路径准备数据
				cthis->m_haveSendLen = 0;
				return FTPUPLOADER_OK;
			}
			else
			{
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;	
				return FTPUPLOADER_ERROR;
			}
			cthis->m_uiCreateTime = XOS_GetTickCount();
			return FTPUPLOADER_OK;
			break;
		default:
			return FTPUPLOADER_ERROR;
		}
	}

}

static XOS_S32 ExecuteDirectory(TFtpUploader *cthis)
{	
	int rv = 0;
	rv = SendComAndGetRecv(cthis);
	if (rv != FTPUPLOADER_OK)
	{
		return rv;
	}
	cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
	strtok(cthis->m_pszRecvData, " ");
	if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_CWD_RECV_OK)  
	{
		cthis->m_bCreatePath = XOS_FALSE;
		memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
		cthis->m_recvlen = 0;
		return FTPUPLOADER_OK;
	}
	else
	{
		cthis->m_bCreatePath = XOS_TRUE;
		memset(cthis->m_pszRecvData, 0, cthis->m_recvlen); 
		cthis->m_recvlen = 0;
		return FTPUPLOADER_OK;
	}
	
	cthis->m_uiCreateTime = XOS_GetTickCount();
	return FTPUPLOADER_OK;
}

static XOS_S32 CreatePath(TFtpUploader *cthis)
{
	int rv = 0;
	int recvlen = 0;
	int iSendDataLen = 0;
	while (1)
	{
		if (cthis->m_bCreatePath)
		{
			memset(cthis->m_pszSendHead, 0, sizeof(cthis->m_pszSendHead));
			if (cthis->m_bEndString == XOS_TRUE)
			{
				memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
				memcpy(cthis->m_pszSendHead, "PASV\r\n", strlen("PASV\r\n"));						
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
				cthis->m_recvlen = 0;
				printf("FtpUploader CreatePath OK!\r\n");
				return FTPUPLOADER_OK;
			}

			//取到'/'为止的字符串
			sscanf(cthis->m_pszFilePathAddress, "%[^/]", cthis->m_pszSendHead);
			if (strlen(cthis->m_pszSendHead) == 0)
			{
				cthis->m_pszFilePathAddress++;
				//取到'/'为止的字符串
				sscanf(cthis->m_pszFilePathAddress, "%[^/]", cthis->m_pszSendHead);
			}
					
			if (!strstr(cthis->m_pszFilePath, cthis->m_pszSendHead))
			{
				return FTPUPLOADER_ERROR;
			}
			
			cthis->m_pszFilePathAddress = cthis->m_pszFilePathAddress+strlen(cthis->m_pszSendHead)+1;
			if (*(--cthis->m_pszFilePathAddress) == '\0')
			{
				cthis->m_bEndString = XOS_TRUE;
			}
			if (cthis->m_pszPath != NULL)
			{
				free(cthis->m_pszPath);
				cthis->m_pszPath = NULL;
			}
			cthis->m_pszPath = (char *)malloc(strlen(cthis->m_pszSendHead) + 1);
			if(cthis->m_pszPath == NULL)
			{
				return FTPUPLOADER_ERROR;
			}
			memcpy(cthis->m_pszPath, cthis->m_pszSendHead, strlen(cthis->m_pszSendHead) + 1);
			memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
			sprintf(cthis->m_pszSendHeadAddress, "CWD %s\r\n", cthis->m_pszPath);
			cthis->m_SendCmdType = SENDCMDTYPE_CWDPATH;
			cthis->m_bCreatePath = XOS_FALSE;
			cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
			cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
			cthis->m_haveSendLen = 0;
			cthis->m_bDirectory = XOS_FALSE;
		}
		while (!cthis->m_bCreatePath)
		{
			rv = SendComAndGetRecv(cthis);
			if (rv != FTPUPLOADER_OK)
			{
				return rv;
			}
			switch(cthis->m_SendCmdType) 
			{
			case SENDCMDTYPE_CWDPATH:				
				cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
				strtok(cthis->m_pszRecvData, " ");
				if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_CWD_RECV_OK)  
				{
					memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
					cthis->m_recvlen = 0;
					cthis->m_bCreatePath = XOS_TRUE;
					free(cthis->m_pszPath);
					cthis->m_pszPath = NULL;
				}
				else if ((atoi(cthis->m_pszRecvData) != FTPUPLOADER_CWD_RECV_OK) && cthis->m_bDirectory)
				{
					free(cthis->m_pszPath);
					cthis->m_pszPath = NULL;
					return FTPUPLOADER_ERROR;
				}
				else
				{
					sprintf(cthis->m_pszSendHead, "MKD %s\r\n", cthis->m_pszPath);
					cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
					cthis->m_haveSendLen = 0;
					cthis->m_bDirectory = XOS_TRUE;
					memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
					cthis->m_recvlen = 0;
					cthis->m_SendCmdType = SENDCMDTYPE_MAKEPATH;
				}					
				cthis->m_uiCreateTime = XOS_GetTickCount();
				break;
			case SENDCMDTYPE_MAKEPATH:		
				cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
				strtok(cthis->m_pszRecvData, " ");
				if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_MKD_RECV_OK || atoi(cthis->m_pszRecvData) == FTPUPLOADER_MKD_RECV_OK_2)
				{
					memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
					cthis->m_recvlen = 0;
					memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
					sprintf(cthis->m_pszSendHeadAddress, "CWD %s\r\n", cthis->m_pszPath);
					cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
					cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
					cthis->m_haveSendLen = 0;
					cthis->m_bDirectory = XOS_FALSE;
					cthis->m_SendCmdType = SENDCMDTYPE_CWDPATH;
				}
				else if ((atoi(cthis->m_pszRecvData) != FTPUPLOADER_MKD_RECV_OK && atoi(cthis->m_pszRecvData) != FTPUPLOADER_MKD_RECV_OK_2) && cthis->m_bDirectory)
				{
					free(cthis->m_pszPath);
					cthis->m_pszPath = NULL;
					return FTPUPLOADER_ERROR;
				}
				
				break;
			default:
				return FTPUPLOADER_ERROR;
			}
		}
	}
}

static XOS_S32 CreateDataSendSocket(TFtpUploader *cthis)
{
	int rv = 0;
	int recvlen = 0;
	int a0, a1, a2, a3, p0, p1;
	char *a,*p;
	struct sockaddr_in data_addr;
	int iSendDataLen = 0;
	rv = SendComAndGetRecv(cthis);
	if (rv != FTPUPLOADER_OK)
	{
		return rv;
	}
	cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
	if(atoi(cthis->m_pszRecvData) == FTPUPLOADER_PASV_RECV_OK)  
	{
		if (sscanf(cthis->m_pszRecvData,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
				   &a0, &a1, &a2, &a3, &p0, &p1) != 6) 
		{				
			printf("Passive mode address scan failure. Shouldn't happen!\n");
			return FTPUPLOADER_ERROR;
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
		
		//client请求连接server, a和p为远程主机IP地址和端口
		cthis->m_sockSend = XOS_TCPConnectNB(*((XOS_U32 *)a), ntohs(*(unsigned short*)p));
		if (cthis->m_sockSend == XOS_INVALID_SOCKET)
		{
			printf("create ftp_senddate connect failed:%d\r\n", rv);
			return FTPUPLOADER_ERROR;
		}
		else
		{
			return FTPUPLOADER_OK;
		}
	}
	return FTPUPLOADER_ERROR;
}

static XOS_S32 DataSendConnetProbe(TFtpUploader *cthis)
{
	int rv;
	rv = XOS_TCPConnectProbe(cthis->m_sockSend);
	if (rv == 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// 有些FTP服务器不认ALLO，跳过
// 		memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
// 		cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
// 		sprintf(cthis->m_pszSendHead, "ALLO\r\n");
// 		cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
// 		cthis->m_haveSendLen = 0;
// 		cthis->m_SendCmdType = SENDCMDTYPE_ALLO1;
		//////////////////////////////////////////////////////////////////////////
		memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
		cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
		sprintf(cthis->m_pszSendHead, "DELE %s\r\n", cthis->m_pszFileName);
		cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
		cthis->m_haveSendLen = 0;
		cthis->m_SendCmdType = SENDCMDTYPE_DELE;
		return FTPUPLOADER_OK;		
	}
	else if (rv == 1)
	{
		return FTPUPLOADER_WOULDBLOCK;
	}
	else
	{
//		assert(0);
		return FTPUPLOADER_ERROR;
	}
}

static XOS_S32 BeforeSendData(TFtpUploader *cthis)
{
	int rv = 0;
	int recvlen = 0;
	int iSendDataLen = 0;
	while (1)
	{
		rv = SendComAndGetRecv(cthis);
		if (rv != FTPUPLOADER_OK)
		{
			return rv;
		}
		
		cthis->m_pszRecvData = cthis->m_pszRecvDataAddress;
		strtok(cthis->m_pszRecvData, " ");
		
		switch(cthis->m_SendCmdType)
		{
		case SENDCMDTYPE_ALLO1:
			if ((atoi(cthis->m_pszRecvData) == FTPUPLOADER_ALLO_RECV_OK) || 
				(atoi(cthis->m_pszRecvData) == FTPUPLOADER_ALLO_RECV_OK_2))
			{
				memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
				sprintf(cthis->m_pszSendHead, "SIZE %s\r\n", cthis->m_pszFileName);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_SIZE;
			}
			else
			{
				return FTPUPLOADER_ERROR;
			}
			break;
		case SENDCMDTYPE_DELE:
			if (atoi(cthis->m_pszRecvData) == FTPUPLOADER_DELE_RECV_OK ||
				(atoi(cthis->m_pszRecvData) == 550))
			{
				memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
				sprintf(cthis->m_pszSendHead, "SIZE %s\r\n", cthis->m_pszFileName);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_SIZE;
			}
			break;
				
		case SENDCMDTYPE_SIZE:
			if(atoi(cthis->m_pszRecvData) == FTPUPLOADER_SIZE_RECV_OK ||
				(atoi(cthis->m_pszRecvData) == 550))
			{
//				recvlen = atoi(strtok(NULL,"\0"));
//				cthis->m_pSendBuffer = cthis->m_pSendBuffer + recvlen;
//				cthis->m_sendDataLen -= recvlen;

//				if (cthis->m_sendDataLen < 0)
//				{
//					cthis->m_sendDataLen = 0;
//				}

				memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
				cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
				sprintf(cthis->m_pszSendHead, "APPE %s\r\n", cthis->m_pszFileName);
				cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
				cthis->m_haveSendLen = 0;
				cthis->m_SendCmdType = SENDCMDTYPE_APPE;
			}
			else
			{
				return FTPUPLOADER_ERROR;
			}
			break;
		case SENDCMDTYPE_APPE:
			if((atoi(cthis->m_pszRecvData) == FTPUPLOADER_APPE_RECV_OK))
			{
				cthis->m_haveSendLen = 0;
				return FTPUPLOADER_OK;
			}
			else
			{
				return FTPUPLOADER_ERROR;
			}
			break;
		default:
			return FTPUPLOADER_ERROR;
		}
	}
	return FTPUPLOADER_ERROR;
}

static XOS_S32 SendData(TFtpUploader *cthis)
{
	int rv = 0;
	rv = XOS_TCPSendDataNB(cthis->m_sockSend, cthis->m_pSendBuffer, &(cthis->m_haveSendLen), cthis->m_sendDataLen);
	if (rv != 0)
	{
		if (rv == 1)
		{
			return FTPUPLOADER_WOULDBLOCK;
		}
		else if (rv == 2)
		{
			cthis->m_uiCreateTime = XOS_GetTickCount();
			return FTPUPLOADER_WOULDBLOCK;
		}
		else
		{
			printf("ftpupload SendData send failed:%d\r\n", rv);
			return FTPUPLOADER_ERROR;
		}
	}
	cthis->m_uiCreateTime = XOS_GetTickCount();

	if (rv == 0)
	{
		if (cthis->m_bCanWriteData == XOS_TRUE)// 是可以写入数据的Uploader，发送完后不关闭
		{
			cthis->m_sendDataLen = 0;
			return FTPUPLOADER_OK;
		}
		else
		{
			XOS_CloseSocket(cthis->m_sockSend);
			cthis->m_sockSend = XOS_INVALID_SOCKET;
			return FTPUPLOADER_OK;
		}
		
	}
	else
	{
		return FTPUPLOADER_WOULDBLOCK;
	}
}

static XOS_BOOL WriteData(TFtpUploader *cthis, const char* pBuffer, int iBufferLen)
{
	// 判断是否支持写入数据
	static iLen;
	if (cthis->m_bCanWriteData == XOS_FALSE)
	{
		return XOS_FALSE;
	}

	// 判断是否发送完了
	if (cthis->m_sendDataLen != 0 )
	{
		return XOS_FALSE;
	}
	
	// 判断是否大于最大缓冲
	if (iBufferLen > cthis->m_iMaxBufferLen)
	{
		return XOS_FALSE;
	}

	memcpy(cthis->m_pSendBufferAddress, pBuffer, iBufferLen);
	cthis->m_pSendBuffer = cthis->m_pSendBufferAddress;
	cthis->m_sendDataLen = iBufferLen;
	cthis->m_haveSendLen = 0;
	iLen += iBufferLen;

	return XOS_TRUE;
}

static XOS_S32 OnTimer(TFtpUploader *cthis)
{
	XOS_S32 rv;
	//检查连接是否超时
	if ((XOS_GetTickCount() - cthis->m_uiCreateTime) > 60*1000)
	{
		return FTPUPLOADER_TIMER_ERROR;
 	}
	switch (cthis->m_state)
	{
		case FTPUPLOADER_STATUS_NONE:
			rv = cthis->Connect(cthis);
			if (rv  == FTPUPLOADER_OK)
			{
				cthis->m_state = FTPUPLOADER_STATUS_CONNECTING;
				printf("ftpUploader connect ok.\r\n");
			}
			else if (rv == FTPUPLOADER_ERROR)
			{
				printf("ftpUploader connect failed.\r\n");
				return FTPUPLOADER_TIMER_ERROR;
			}
			else
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			break;
		case FTPUPLOADER_STATUS_CONNECTING:
			rv = cthis->ConnetProbe(cthis);
			if (rv == FTPUPLOADER_OK)
			{
				printf("ftp connet probe ok.\r\n");
				cthis->m_state = FTPUPLOADER_STATUS_SENDHEADCOMAND;
			}
			else if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else //FTPUPLOADER_WOULDBLOCK
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			break;
		case FTPUPLOADER_STATUS_SENDHEADCOMAND:
			rv = cthis->SendHead(cthis);
			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if(rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{			
				printf("ftp send head ok.\r\n");
				cthis->m_state = FTPUPLOADER_STATUS_EXECUTEDIRCTORY;
			}
			break;
		case FTPUPLOADER_STATUS_EXECUTEDIRCTORY:
			rv = cthis->ExecuteDirectory(cthis);
			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if(rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{			
				if (cthis->m_bCreatePath)
				{
					printf("ftp execute directory ok. %d\r\n", __LINE__);
					cthis->m_state = FTPUPLOADER_STATUS_CREATPATH;
				}
				else
				{
					memset(cthis->m_pszSendHeadAddress, 0, FTPUPLOADER_HEAD_LEN);
					strcpy(cthis->m_pszSendHeadAddress, "PASV\r\n");
					cthis->m_pszSendHead = cthis->m_pszSendHeadAddress;
					cthis->m_sendHeadLen = strlen(cthis->m_pszSendHead);
					cthis->m_haveSendLen = 0;
					memset(cthis->m_pszRecvData, 0 , cthis->m_recvlen); 
					cthis->m_recvlen = 0;
					printf("ftp execute directory ok. %d\r\n", __LINE__);
					cthis->m_state = FTPUPLOADER_STATUS_CREATSENDDATASOCKTE;
				}
			}
			break;
		case FTPUPLOADER_STATUS_CREATPATH:
			rv = cthis->CreatePath(cthis);
 			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if(rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{			
				printf("ftp create path ok. %d\r\n", __LINE__);
				cthis->m_state = FTPUPLOADER_STATUS_CREATSENDDATASOCKTE;
			}
			break;
		case FTPUPLOADER_STATUS_CREATSENDDATASOCKTE:
			rv = cthis->CreateDataSendSocket(cthis);
			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if(rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{			
				printf("ftp create data send socket ok. %d\r\n", __LINE__);
				cthis->m_state = FTPUPLOADER_STATUS_DATASENDCONNECTING;
			}
			break;
		case FTPUPLOADER_STATUS_DATASENDCONNECTING:
			rv = cthis->DataSendConnetProbe(cthis);
			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if(rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{			
				printf("ftp data send connect probe ok. %d\r\n", __LINE__);
				cthis->m_state = FTPUPLOADER_STATUS_BEFORSENDDATA;
			}
			break;
		case FTPUPLOADER_STATUS_BEFORSENDDATA:
			rv = cthis->BeforeSendData(cthis);
			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if(rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{			
				printf("BeforeSendData ok. %d\r\n", __LINE__);
				cthis->m_haveSendLen = 0;
				cthis->m_state = FTPUPLOADER_STATUS_SENDDATA;
			}
			break;
		case FTPUPLOADER_STATUS_SENDDATA:
			// 没有要发的数据
			if (cthis->m_sendDataLen == 0)
			{
				return FTPUPLOADER_TIMER_OK;
			}

			rv = cthis->SendData(cthis);
			if (rv == FTPUPLOADER_ERROR)
			{
				return FTPUPLOADER_TIMER_ERROR;
			}
			else if (rv == FTPUPLOADER_WOULDBLOCK)
			{
				return FTPUPLOADER_TIMER_FREE;
			}
			else //FTPUPLOADER_OK
			{
				if (cthis->m_bCanWriteData == XOS_FALSE)
				{
					printf("ftp send data ok. %d\r\n", __LINE__);
				}
				return FTPUPLOADER_TIMER_OK;
			}
		default:
			return FTPUPLOADER_TIMER_ERROR;
	}
	return FTPUPLOADER_TIMER_BUSY;
}

static void Report(TFtpUploader *cthis, int (*pPrn)(const char *,...))
{
	struct in_addr in;
	in.s_addr = cthis->m_serverIP;
	
	pPrn("==============================FTPUPLoader Report Begin=========================\r\n");
	pPrn("\tthis=%08X\r\n", cthis);
	pPrn("\tm_state: %d\r\n", cthis->m_state);
	pPrn("\tm_sock: %d\r\n", cthis->m_sock);
	pPrn("\tm_sockSend: %d\r\n", cthis->m_sockSend);
	pPrn("\tm_m_SendCmdType: %d\r\n", cthis->m_SendCmdType);
	pPrn("\tB_creatPath: %d\r\n", cthis->m_bCreatePath);
	pPrn("\tm_serverIP: %s\r\n", inet_ntoa(in));
	pPrn("\tm_pszfilePath: %s\r\n", cthis->m_pszFilePath);
	pPrn("\tm_pszfileName: %s\r\n", cthis->m_pszFileName);
	pPrn("\tB_directoryflag: %d\r\n", cthis->m_bDirectory);
	pPrn("\tm_uiCreateTime: %08X\r\n", cthis->m_uiCreateTime);
	pPrn("==============================FTPUPLoader Report End=========================\r\n");
}

const TFtpUploader FTPUPLOADER = 
{
	&Construct,
	&Destroy,
 	&Create,
	&Create2,
	&WriteData,
	&OnTimer,
	&Report,

	&Connect,
	&ConnetProbe,
	&SendData,
	&SendHead,
	&ExecuteDirectory,
	&CreatePath,
	&CreateDataSendSocket,
	&DataSendConnetProbe,
	&BeforeSendData
};








