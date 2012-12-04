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

#ifndef __FTPDOWNLOADER_H__
#define __FTPDOWNLOADER_H__

#define FTPDOWNLOADER_TIMER_ERROR					-1
#define FTPDOWNLOADER_TIMER_CONNECTION_ERROR		-2
#define FTPDOWNLOADER_TIMER_OK						0
#define FTPDOWNLOADER_TIMER_BUSY					1
#define FTPDOWNLOADER_TIMER_FREE					2

#define FTPDOWNLOADER_ERROR							-1
#define FTPDOWNLOADER_OK							0
#define FTPDOWNLOADER_WOULDBLOCK					1

#define FTPDOWNLOADER_STATUS_NONE					-1
#define FTPDOWNLOADER_STATUS_START					0	
#define FTPDOWNLOADER_STATUS_CONNECTING				1	
#define FTPDOWNLOADER_STATUS_SENDHEADCOMAND			2	
#define FTPDOWNLOADER_STATUS_EXECUTEDIRCTORY		5
#define FTPDOWNLOADER_STATUS_DATASENDCONNECTING		7
#define FTPDOWNLOADER_STATUS_BEFORSENDDATA			8
#define FTPDOWNLOADER_STATUS_CREATSENDDATASOCKTE	9
#define FTPDOWNLOADER_STATUS_RECVDATA				3	
#define FTPDOWNLOADER_STATUS_RECV					4
#define FTPDOWNLOADER_STATUS_OPENFILE				6

#define FTPDOWNLOADER_SERVER_OK						220
#define FTPDOWNLOADER_USRNAME_OK					331
#define FTPDOWNLOADER_LOGIN_OK						230

#define FTPDOWNLOADER_FILE_RECV_OK					226
#define FTPDOWNLOADER_LOGIN_RECV_OK					230
#define FTPDOWNLOADER_PASV_RECV_OK					227
#define FTPDOWNLOADER_CWD_RECV_OK					250
#define FTPDOWNLOADER_PWD_RECV_OK					257
#define FTPDOWNLOADER_MKD_RECV_OK					257
#define FTPDOWNLOADER_APPE_RECV_OK					150
#define FTPDOWNLOADER_APPE_RECV_OK_2				125
#define FTPDOWNLOADER_SIZE_RECV_OK					213
#define FTPDOWNLOADER_ALLO_RECV_OK					202
#define FTPDOWNLOADER_ALLO_RECV_OK_2				200

#define FTPDOWNLOADER_HEAD_LEN						256
#define FTPDOWNLOADER_BUFFER_LEN					1024

typedef struct _TFtpDownloader TFtpDownloader;
struct _TFtpDownloader
{
//public:
	void (*Construct)(TFtpDownloader *cthis);
	void (*Destroy)(TFtpDownloader *cthis);

	/*  功能描述: 
	 *  参数说明:
	 *      LPCTSTR lpszFtpServer [IN]:		FTP服务器(smtp)地址或域名,不带端口则使用21号端口,
	 *										否则使用指定的端口,例如:www.crearo.com:2121
	 *      LPCTSTR lpszUserName [IN]:		用户名
	 *      LPCTSTR lpszPassword [IN]:		密码
	 *      LPCTSTR lpszServerPath [IN]:	下载路径,格式用:"../../../.."
	 *      LPCTSTR lpszFileName [IN]:		下载的文件名
	 *      LPCTSTR lpszFullFileName [IN]:	文件本地保存带绝对路径的文件名
	 */
	XOS_BOOL (*Create)(TFtpDownloader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword,
				   char* lpszServerPath, char* lpszFileName, char* lpszFullFileName);

	XOS_S32 (*OnTimer)(TFtpDownloader *cthis);
	void (*Report)(TFtpDownloader *cthis, int (*pPrn)(const char *,...));

//private:
	XOS_S32 (*Connect)(TFtpDownloader *cthis);
	XOS_S32 (*ConnetProbe)(TFtpDownloader *cthis);
	XOS_S32 (*ReceiveData)(TFtpDownloader *cthis);
 	XOS_S32 (*SendHead)(TFtpDownloader *cthis);
	XOS_S32 (*ExecuteDirectory)(TFtpDownloader *cthis);
	XOS_S32 (*FtpDownloaderOpenFile)(TFtpDownloader *cthis);
	XOS_S32 (*CreateDataReceiveSocket)(TFtpDownloader *cthis);
	XOS_S32 (*DataReceiveConnetProbe)(TFtpDownloader *cthis);
	
	int m_state;
	XOS_SOCKET m_sock;	
	XOS_SOCKET m_sockRecv;
	unsigned int m_recvlen;
	XOS_BOOL m_bSendHead;
	XOS_BOOL m_bFirstsend;
	unsigned int m_serverIP;
	char* m_pszServerURL;
	char *m_pszFilePath;
	char *m_pszSendHead;
	char *m_pszFileName;
	char *m_pszHeadAddress;
	char *m_pszRecvData;
	char *m_pszRecvDataAddress;
	char *m_pszFullFileName;
	char *m_pszUserName;
	char *m_pszPassword;
	int m_sendHeadLen;
	int m_SendCmdType;	
	int m_haveSendLen;
	unsigned int m_uiCreateTime;				//创建时间
	char *m_pszRecvBuf;
	int m_iRecvDatalen;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TFtpDownloader FTPDOWNLOADER;

#ifdef __cplusplus
}
#endif

#endif //__FTPDOWNLOADER_H__
