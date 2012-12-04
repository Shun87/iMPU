
/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: FtpUploader.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: xuejiabin
 *  �������: 2007-7-12 10:43:06
 *  ������ע: 
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
	
	/*  ��������: ����FTP�ϴ�����
	 *  ����˵��:
	 *      LPCTSTR lpszFtpServer [IN]:		FTP������(smtp)��ַ������,�����˿���ʹ��21�Ŷ˿�,
	 *										����ʹ��ָ���Ķ˿�,����:www.crearo.com:2121
	 *      LPCTSTR lpszUserName [IN]:		�û���
	 *      LPCTSTR lpszPassword [IN]:		����
	 *      LPCTSTR lpszServerPath [IN]:	�ϴ�·��,��ʽ��:"../../../.."
	 *      LPCTSTR lpszFileName [IN]:		�ϴ��ļ���
	 *      long nSendLen [IN]:				�ļ�����, ���pSendBufferΪNULL���˲���Ϊÿ�ο���д�����ݵ���󳤶�
	 *      char *pSendBuffer [IN]:			�ļ�����, ����˲���ΪNULL, �������WriteDataд������ϴ�����
	 *  ��ע:	Create	�����ڴ�,���ҿ�������
	 *			Create2	ʹ���ϲ����ݻ���
	 */
 	XOS_BOOL (*Create)(TFtpUploader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword,
				   char* lpszServerPath, char* lpszFileName, long nSendLen, char *pSendBuffer);
	XOS_BOOL (*Create2)(TFtpUploader *cthis, char* lpszFtpServer, char* lpszUserName, char* lpszPassword,
					char* lpszServerPath, char* lpszFileName, long nSendLen, char *pSendBuffer);

	/*  ��������: д��Ҫ�ϴ�������
	 *  ����˵��:
	 *		const char* pBuffer [IN]:		Ҫд������ݻ���
	 *		int iBufferLen [IN]:			���ݳ���
	 *  ����ֵ: XOS_TRUE: �ɹ�; XOS_FALSE: ʧ��, Ҫ����д��
	 *  ��ע: ʧ�ܵĿ�������:
	 *		1.�����Ĳ��ǿ���д�����ݵ�����;
	 *		2.֮ǰ������û�з���, ���OnTimer����FTPUPLOADER_TIMER_OKʱ, ��д��;
	 *		3.iBufferLen���ڴ���ʱ��ÿ�ο���д�����ݵ���󳤶�
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
	unsigned int m_uiCreateTime;				//����ʱ��
	char *m_pszPath;

	XOS_BOOL m_bCanWriteData;					// �Ƿ���Գ���д������
	int m_iMaxBufferLen;						// ÿ��д��������ݻ���
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TFtpUploader FTPUPLOADER;

#ifdef __cplusplus
}
#endif

#endif //__FTPUPLOADER_H__
