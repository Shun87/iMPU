/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: Proxy.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ����˴��������,֧��HTTP,SOCKET4,SOCKET5����
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2007-02-28 17:11:41
 *  ������ע: 
 *  
 */

#ifndef __PROXY_H__
#define __PROXY_H__

//�������ӳ�ʱʱ��
#define MAX_PROXY_TIMEOUT			(60*1000)

//����������
#define PROXY_PROBE_OK				0
#define PROXY_PROBE_WOULDBLOCK		1
#define PROXY_PROBE_TIMEOUT			2
#define PROXY_PROBE_ERROR			-1

//��������
#define PROXY_TYPE_NULL				0
#define PROXY_TYPE_HTTP				1
#define PROXY_TYPE_SOCKET4			2
#define PROXY_TYPE_SOCKET5			3

//�����ַ����Ϣ��󳤶�
#define MAX_PROXY_DESTADDR_LEN		256
#define MAX_PROXY_ADDR_LEN			256
#define MAX_PROXY_USERNAME_LEN		64
#define MAX_PROXY_PASSWORD_LEN		64

//�����ڲ�״̬
#define PROXY_STATUS_NULL						0
#define PROXY_STATUS_CONNECTING					1
#define PROXY_STATUS_NEGOTIATION_REQUEST		2
#define PROXY_STATUS_NEGOTIATION_RESPONSE		3
#define PROXY_STATUS_SUBNEGOTIATION_REQUEST		4
#define PROXY_STATUS_SUBNEGOTIATION_RESPONSE	5
#define PROXY_STATUS_CONNECTION_REQUEST			6
#define PROXY_STATUS_CONNECTION_RESPONSE		7
#define PROXY_STATUS_CONNECTION_FINISHED		8
#define PROXY_STATUS_FAILED						9

typedef struct _TProxy TProxy;
struct _TProxy  
{
//public:
	void (*Construct)(TProxy *pProxy);
	void (*Destroy)(TProxy *pProxy);

	/*  ��������: �����������
	 *  ����˵��:
	 *      XOS_PSZ pszProxyType [IN]:	��������,ȡֵ������:"HTTP","SOCKET4","SOCKET5"
	 *      XOS_PSZ pszDestAddr [IN]:	Ŀ���ַ,��ʽΪ:ip:port����domainname:port
	 *      XOS_PSZ pszProxyAddr [IN]:	�����ַ,��ʽͬ��
	 *      XOS_PSZ pszUserName [IN]:	���Ӵ�����û���������
	 *      XOS_PSZ pszPassword [IN]:	���Ӵ��������
	 */
	XOS_BOOL (*Create)(TProxy *pProxy, XOS_PSZ pszProxyType, XOS_PSZ pszDestAddr, XOS_PSZ pszProxyAddr, XOS_PSZ pszUserName, XOS_PSZ pszPassword);

	/*  ��������: ̽������Ƿ����ӳɹ�
	 *  ����ֵ: ����ֵ��ǰ���"����������"�궨��
	 */
	int (*Probe)(TProxy *pProxy);

	/*  ��������: ��ȡ�������ӳɹ���socket���
	 *  ����ֵ: ���ӳɹ���socket���
	 *  ��ע: ��Probe�ɹ�֮��ŵ����������,�ͻ᷵��һ���Ϸ�ֵ,���򷵻�XOS_INVALID_SOCKET
	 */
	XOS_SOCKET (*GetSocket)(TProxy *pProxy);

//private:
	int (*ProbeHttp)(TProxy *pProxy);
	int (*ProbeSocket4)(TProxy *pProxy);
	int (*ProbeSocket5)(TProxy *pProxy);

	int m_status;
	XOS_SOCKET m_sock;
	XOS_SOCKET m_sock_bak;
	int m_iProxyType;
	char m_szDestAddr[MAX_PROXY_DESTADDR_LEN];
	char m_szProxyAddr[MAX_PROXY_ADDR_LEN];
	char m_szUserName[MAX_PROXY_USERNAME_LEN];
	char m_szPassword[MAX_PROXY_PASSWORD_LEN];
	char m_tmpbuf[256];
	char m_sendbuf[1024];
	char m_recvbuf[1024];
	int	 m_isendbufLen;
	int  m_ihaveSendLen;
	int  m_irecvLen;
	XOS_U32 m_uiOperationBeginTime;
	XOS_U32 m_uiDestIP;
	XOS_U16 m_usDestPort;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TProxy PROXY;

#ifdef __cplusplus
}
#endif

#endif //__PROXY_H__
