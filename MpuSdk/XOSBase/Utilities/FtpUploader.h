
/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: FtpUploader.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xuejiabin
 *  完成日期: 2007-7-12 10:43:06
 *  修正备注: 
 *  
 */
#ifndef __FTPUPLOADER_H__
#define __FTPUPLOADER_H__

#define FTPUPLOADER_TIMER_ERROR					-1
#define FTPUPLOADER_TIMER_CONNECT_ERROR			-2
#define FTPUPLOADER_TIMER_OK					0
#define FTPUPLOADER_TIMER_BUSY					1
#define FTPUPLOADER_TIMER_FREE					2

#define FTPUPLOADER_ERROR						-1
#define FTPUPLOADER_OK							0
#define FTPUPLOADER_WOULDBLOCK					1

#define FTPUPLOADER_STATUS_NONE                 -1
#define FTPUPLOADER_STATUS_START				0	
#define FTPUPLOADER_STATUS_CONNECTING			1	
#define FTPUPLOADER_STATUS_SENDHEADCOMAND		2	
#define FTPUPLOADER_STATUS_EXECUTEDIRCTORY		5
#define FTPUPLOADER_STATUS_CREATPATH			6
#define FTPUPLOADER_STATUS_DATASENDCONNECTING	7
#define FTPUPLOADER_STATUS_BEFORSENDDATA		8
#define FTPUPLOADER_STATUS_CREATSENDDATASOCKTE  9
#define FTPUPLOADER_STATUS_SENDDATA				3	
#define FTPUPLOADER_STATUS_RECV					4

#define FTPUPLOADER_SERVER_OK					220
#define FTPUPLOADER_USRNAME_OK					331
#define FTPUPLOADER_LOGIN_OK					230

#define FTPUPLOADER_FILE_RECV_OK				226
#define FTPUPLOADER_LOGIN_RECV_OK				230
#define FTPUPLOADER_PASV_RECV_OK				227
#define FTPUPLOADER_CWD_RECV_OK					250
#define FTPUPLOADER_PWD_RECV_OK					257
#define FTPUPLOADER_MKD_RECV_OK					257
#define FTPUPLOADER_MKD_RECV_OK_2				250
#define FTPUPLOADER_APPE_RECV_OK				150
#define FTPUPLOADER_APPE_RECV_OK_2				125
#define FTPUPLOADER_SIZE_RECV_OK				213
#define FTPUPLOADER_ALLO_RECV_OK				202
#define FTPUPLOADER_ALLO_RECV_OK_2				200
#define FTPUPLOADER_DELE_RECV_OK				250

#define FTPUPLOADER_HEAD_LEN					256

typedef struct _TFtpUploader TFtpUploader;
struct _TFtpUploader
{
//public:
	void (*Construct)(TFtpUploader *cthis);
	void (*Destroy)(TFtpUploader *cthis);
	
	/*  功能描述: 创建FTP上传对象
	 *  参数说明:
	 *      LPCTSTR lpszFtpServer [IN]:		FTP服务器(smtp)地址或域名,不带端口则使用21号端口,
	 *										否则使用指定的端口,例如:www.crearo.com:2121
	 *      LPCTSTR lpszUserName [IN]:		用户名
	 *      LPCTSTR lpszPassword [IN]:		密码
	 *      LPCTSTR lpszServerPath [IN]:	上传路径,格式用:"../../../.."
	 *      LPCTSTR lpszFileName [IN]:		上传文件名
	 *      long nSendLen [IN]:				文件长度, 如果pSendBuffer为NULL，此参数为每次可以写入数据的最大长度
	 *      char *pSendBuffer [IN]:			文件数据, 如果此参数为NULL, 则可以用WriteData写入持续上传数据
	 *  备注:	Create	分配内存,并且拷贝数据
	 *			Create2	使用上层数据缓冲
	 */
 	XOS_BOOL (*Create)(TFtpUploader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword,
				   char* lpszServerPath, char* lpszFileName, long nSendLen, char *pSendBuffer);
	XOS_BOOL (*Create2)(TFtpUploader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword,
					char* lpszServerPath, char* lpszFileName, long nSendLen, char *pSendBuffer);

	/*  功能描述: 写入要上传的数据
	 *  参数说明:
	 *		const char* pBuffer [IN]:		要写入的数据缓冲
	 *		int iBufferLen [IN]:			数据长度
	 *  返回值: XOS_TRUE: 成功; XOS_FALSE: 失败, 要重新写入
	 *  备注: 失败的可能性有:
	 *		1.创建的不是可以写入数据的类型;
	 *		2.之前的数据没有发完, 最好OnTimer返回FTPUPLOADER_TIMER_OK时, 再写入;
	 *		3.iBufferLen大于创建时的每次可以写入数据的最大长度
	 */
	XOS_BOOL (*WriteData)(TFtpUploader *cthis, const char* pBuffer, int iBufferLen);

	XOS_S32 (*OnTimer)(TFtpUploader *cthis);
	void (*Report)(TFtpUploader *cthis, int (*pPrn)(const char *,...));

//private:
	XOS_S32 (*Connect)(TFtpUploader *cthis);
	XOS_S32 (*ConnetProbe)(TFtpUploader *cthis);
	XOS_S32 (*SendData)(TFtpUploader *cthis);
 	XOS_S32 (*SendHead)(TFtpUploader *cthis);
	XOS_S32 (*ExecuteDirectory)(TFtpUploader *cthis);
	XOS_S32 (*CreatePath)(TFtpUploader *cthis);
	XOS_S32 (*CreateDataSendSocket)(TFtpUploader *cthis);
	XOS_S32 (*DataSendConnetProbe)(TFtpUploader *cthis);
	XOS_S32 (*BeforeSendData)(TFtpUploader *cthis);
	
	int m_state;
	XOS_SOCKET m_sock;	
	XOS_SOCKET m_sockSend;
	int m_recvlen;
	XOS_BOOL m_bSendHead;
	XOS_BOOL m_bCreatePath;	
	XOS_BOOL m_bEndString;
	XOS_BOOL m_bDirectory;
	unsigned int m_serverIP;	
	char *m_pszServerURL;
	char *m_pSendBuffer;
	char *m_pSendBufferAddress;
	char *m_pszFilePath;	
	char *m_pszFilePathAddress;
	char *m_pszSendHead;
	char *m_pszSendHeadAddress;
	char *m_pszUserName;
	char *m_pszPassword;
	char *m_pszFileName;
	char *m_pszRecvData;
	char *m_pszRecvDataAddress;

	int m_SendCmdType;
	int m_sendHeadLen;
	int m_haveSendLen;
	int	m_sendDataLen;
	unsigned int m_uiCreateTime;				//创建时间
	char *m_pszPath;

	XOS_BOOL m_bCanWriteData;					// 是否可以持续写入数据
	int m_iMaxBufferLen;						// 每次写入最大数据缓冲
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TFtpUploader FTPUPLOADER;

#ifdef __cplusplus
}
#endif

#endif //__FTPUPLOADER_H__
