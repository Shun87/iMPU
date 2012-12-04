/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: NSS7.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: NSS7 = NRCAP Server Session 7.0
 *  
 *  ��ǰ�汾: 
 *  ��    ��: ������
 *  �������: 2010-4-8 17:05:51
 *  ������ע: 
 *  
 */

#ifndef __NSS7_H__
#define __NSS7_H__

//��İ汾
#define NCS7_VERSION		"7.2011.11.14"

//���صĴ�����,С��NSS7_E_OK��Ҫ�ر�ͨ��
#define NSS7_E_WOULDBLOCK		 1		//�������ڽ�����
#define NSS7_E_OK				 0		//�ɹ�

//δ֪������
#define NSS7_E_UNKNOW			-1		//δ֪����
#define NSS7_E_PARAM			-2		//�ӿں����д���Ĳ�����Ч
#define NSS7_E_PARAMLEN			-3		//����ƽ̨ע��Ĺ�����,���յ���ͨ�ò����ĳ��ȴ���
#define NSS7_E_REDIRECT			-4		//ƽ̨���ص��ض���ʧ��,����ErrorCode��ֵ��Ϊ0���߱�ǩ��ֵ��Ϊ"OK"
#define NSS7_E_POSTLEN			-5		//��ע��Ĺ�����,���յ�POST�ĳ������, ���߽���POST�������(���ջ��峤��Ϊ1024�ֽ�)
#define NSS7_E_POST				-6		//���յ���POST����,������Э���Ҫ��,������ȱ�첲���ȵ�
#define NSS7_E_MALLOC			-7		//�ڴ濪�ٴ���
#define NSS7_E_TCPRECV			-8		//TCP���ճ���
#define NSS7_E_TCPSEND			-9		//TCP���ͳ���
#define NSS7_E_TIMEOUT			-10		//�������ݳ�ʱ
//�շ�����ʱ�ؼ�����
#define NSS7_E_SYNCFLAG			-51		//��������ʱ��ʼ�ַ�
#define NSS7_E_VERSION			-52		//��������ʱ�汾����
#define NSS7_E_CTRLFLAG			-53		//��������ʱ�����ֶδ���
#define NSS7_E_MSGTYPE			-54		//��������ʱ��Ϣ���ʹ���
#define NSS7_E_PACKETLEN		-55		//��������ʱ�����ȴ���,�����������Ҫ���ܵ�û��8�ֽڶ���
#define NSS7_E_PACKETTYPE		-56		//��������ʱ�����ʹ���
#define NSS7_E_REGISTERVER		-57		//ע��Э��汾����
//ƽ̨��ַ���ɴ������
#define NSS7_E_NETBASE			-100	//��������������ʼֵ
#define NSS7_E_NOROUTE			-101	//û·��
#define NSS7_E_NOSERVER			-102	//ƽ̨����Ϊ����(�˿ڳ���)
#define NSS7_E_UNREACHABLE		-103	//ƽ̨��ַ���ɴ�
#define NSS7_E_PROXYCONNECT		-104	//��������ʧ��
#define NSS7_E_NETBASEEND		-149	//���������������ֵ
//ƽ̨���صĴ�����
#define NSS7_E_PLATFORMBASE		-150	//ƽ̨���ػ�ֵ��ʼ(��: ��ֵ - ����Ĵ����� = ƽ̨���صĴ�����)
#define NSS7_E_PASSWORD			-151	//ƽ̨�����������
#define NSS7_E_PUIDNOTEXIST		-152	//ƽ̨����PUID������
#define NSS7_E_PUIDNOTDISABLE	-153	//ƽ̨����PUID������
#define NSS7_E_INVALIDPOST		-154	//ƽ̨����POST���ĸ�ʽ����
#define NSS7_E_PUFULL			-155	//ƽ̨����PU����������
#define NSS7_E_PRODUCERID		-156	//ƽ̨���طǷ��ĳ���ID
#define NSS7_E_ALREADYONLINE	-157	//ƽ̨����PUID�ѱ�ʹ��
#define NSS7_E_DEVIDONLINE		-158	//ƽ̨����DevID������
#define NSS7_E_PRODUCERPSW		-159	//ƽ̨���س����������
#define NSS7_E_PLATFORMBASEEND	-199	//ƽ̨���ػ�ֵ����(���صĴ�����)

//���ʱ�䳤�ȶ���
#define NSS7_KEEPALIVEINTERVAL	15		//�����ʱ����,��λ��
#define NSS7_CONNECTTIMEOUT		120		//���ӳ�ʱʱ�䳤��,��λ��
//������󳤶ȶ���
#define NSS7_MAXLEN_PACKETLIST	(16)	//����������������ĸ���
										//���ڴ�ֵʱ,���ͺ����᷵��NSS7_E_WOULDBLOCK
//�����������˵��
#define NSS7_MAXLEN_HASHPSW			16
#define NSS7_MAXLEN_PUIADDR			64	//PUI��ַ
#define NSS7_MAXLEN_PROXYTYPE		16
#define NSS7_MAXLEN_PROXYADDR		64
#define NSS7_MAXLEN_PROXYUNAME		64
#define NSS7_MAXLEN_PROXYPSW		64
#define NSS7_MAXLEN_CONTENT			2048

//�û���Ϣ
#define NSS7_MAXLEN_USERID			64
#define NSS7_MAXLEN_CLIENTTYPE		32

//��Ϣ���Ͷ���
#define NSS7_MSGTYPE_REQUEST					1	//�����¼�
#define NSS7_MSGTYPE_RESPONSE					2	//��Ӧ�¼�
#define NSS7_MSGTYPE_NOTIFY						3	//֪ͨ�¼�
#define NSS7_MSGTYPE_REPORT						4	//�����ϱ��¼�
#define NSS7_MSGTYPE_INTELL						5	//���ܷ����¼�
#define NSS7_MSGTYPE_EXTENDAPPNOTIFY			6	//��չӦ��֪ͨ�¼�

//�豸��ƽ̨ͨ�ž��,�豸��ͻ��˵�ͨ�������Э�̺�,Ҳ��ת���˾������
typedef void* NSS7_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ��ʼ��XOSģ��.
 *  ����˵��:
 *  ����ֵ: 0 ��ʾ�ɹ�, -1 ��ʾʧ��
 *  ��ע: �õ��Ķ��ģ���ж���XXX_XOSStartup������ʱ��,ֻ����һ���Ϳ�����
 */
int NSS7_XOSStartup();

/*  ��������: ��ʼ��DNS��ѯģ��
 *  ����˵��:
 *  ����ֵ: 0 ��ʾ�ɹ�, -1 ��ʾʧ��
 *  ��ע: �õ��Ķ��ģ���ж���XXX_DNS2IPInit������ʱ��,ֻ����һ���Ϳ�����
 */
int NSS7_DNS2IPInit();

/*  ��������: ����NSS7���,������ƽ̨ͨ��
 *  ����˵��:
 *      char *pszPUIAddr [IN]:				PUI��ַ,IP������.
 *      unsigned short usPUIPort [IN]:		PUI�˿�
 *      unsigned char binPswHash[16] [IN]:	��¼����Ĺ�ϣֵ
 *      char *pszPUID [IN]:					�豸��PUIDҲ���Ƿ����,����Ӫ�̷���
 *      unsigned int uiIP [IN]:				�豸���ڵ�IP,����0
 *      unsigned int uiSubnetMask [IN]:		��������,�豸���ڵ���������,����0
 *      unsigned int uiDialIP [IN]:			����IP,�豸���Ż�ȡ��IP,����3G���߲��Ż���PPPoE����,����0
 *      char *pszProducerID [IN]:			�豸�������̴���,�ɴ�������.
 *		unsigned char binProducerPsw[64] [IN]:	���̽�������,�ɴ�������.
 *      int iKeyResNum[IN]:					�ؼ���Դ��Ŀ,������Ƶ�������豸һ������Ƶ��,������ʲô��Դ������
 *											�ɴ����ڷ��䳧�̽���ID��ʱ��Ԥ����,ƽ̨��ͨ�������ֵͳ����һ
 *											����ʹ�õ���Դ��,���С�ں����ϱ���ʵ����Դ��,��ô��ƽ̨���ܹ���
 *											����ʵ����Դ���������Ϊ׼.
 *      char *pszDevID [IN]:				�豸Ψһ��ʶ,��ƽ̨�б���Ψһ,����ʹ�ó��̴������MAC��ַ����,
 *											����:"00001-112233445566".
 *      char *pszDevType [IN]:				�豸����,ȡֵ�ͺ�������:
 *												ENC		���߱�����
 *												DEC		���߽�����
 *												WENC	���߱�����
 *												WDEC	���߽�����
 *												ALARM	��������
 *												GPS		GPS�豸
 *      char *pszDevModel [IN]:				�豸�ͺ�,����������ͬ���̵Ĳ�ͬ�ͺŵ��豸.
 *      char *pszHardwareVer [IN]:			Ӳ���汾��,����������ͬ�ͺ��豸�Ĳ�ͬӲ���汾.
 *      char *pszSoftwareVer [IN]:			����汾��,����������ͬ�ͺ��豸�Ĳ�ͬ����汾
 *      char *pszExtendPost [IN]:			��ƽ̨ע��ʱ������Post����,��ʽΪ:Item=value&..., ����:"".
 *      int bProxy [IN]:					�Ƿ�ͨ�������¼ƽ̨, 1:��, 0:��
 *      char *pszProxyType [IN]:			��������, ����:""..
 *      char *pszProxyAddr [IN]:			�����������ַ, IP������, ����:"".
 *      unsigned short usProxyPort [IN]:	����������˿�, ����:0.
 *      char *pszProxyUserName [IN]:		�����û���, ����:"".
 *      char *pszProxyPassword [IN]:		����������, ����:"".
 *      int bFixPUIAddr [IN]:				�Ƿ�̶�PUI�ĵ�ַ1:��, 0:��. (ƽ̨���˶˿�ӳ����֧���ض���ʱ��Ҫ)
 *      int bDebug [IN]:					�Ƿ��ǵ��԰汾, 1��, 0:��
 *  ����ֵ: �ɹ�����NSS7���,����NULL.
 */
NSS7_HANDLE NSS7_Create(char *pszPUIAddr, unsigned short usPUIPort, unsigned char binPswHash[16],
						char *pszPUID, unsigned int uiIP, unsigned int uiSubnetMask, unsigned int uiDialIP,
						char *pszProducerID, unsigned char binProducerPsw[64], int iKeyResNum,
						char *pszDevID, char *pszDevType, char *pszDevModel,
						char *pszHardwareVer, char *pszSoftwareVer, char *pszExtendPost,
						int bProxy, char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
						char *pszProxyUserName, char *pszProxyPassword,
						int bFixPUIAddr, int bDebug);

/*  ��������: ��ȡ�����Ƿ�ɹ�
 *  ����˵��:
 *      int sock [IN]:					NSS7ģ����
 *      unsigned char ucCtlFlag [IN]:	�����㷨��ʶ,1��ʾDES����,0��ʾ������
 *      char binDesKey[8] [IN]:			DES���ܵ���Կ
 *      unsigned int uiKeepAliveInterval [IN]:	���ͱ�����ļ��,��λ����
 *      unsigned int uiSessionTimeOut [IN]:		���ӳ�ʱʱ��,��λ����
 *      char *pszUserID [IN]:	���ӽ������û�ID.
 *      char *pszClientType [IN]:	���ӽ����Ŀͻ�����.
 *  ����ֵ: �ɹ�����NSS7���,����NULL.
 */
NSS7_HANDLE NSS7_CreateWithoutChat(int sock, unsigned char ucCtlFlag, char binDesKey[8],
								   unsigned int uiKeepAliveInterval, unsigned int uiSessionTimeOut,
								   char *pszUserID, char *pszClientType);

/*  ��������: ��ȡ�����Ƿ�ɹ�
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ӳɹ�
 *				NSS7_E_WOULDBLOCK	��Ҫ����̽��
 *				С��NSS7_E_OK			���ӳ���Ĵ�����
 */
int NSS7_GetConnectStatus(NSS7_HANDLE hNSS7);

/*  ��������: ��һ�����ͱ����Ƿ������
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *  ����ֵ: 1��ʾ���ͳɹ�,0��ʾû�з��ͳɹ�
 *  ��ע: �������������ʱ��,��Ҫ���ǵ���NSS7_Run,�ڲ�û�̵߳���
 */
int NSS7_IsFirstPacketSendOK(NSS7_HANDLE hNSS7);

/*  ��������: �ڲ�����
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ȳɹ�
 *				С��NSS7_E_OK		���ӳ���Ĵ�����
 *  ��ע: Ҫ��ʱ���ô˽ӿ�,����ʵ�����ݵķ��ͽ���.
 */
int NSS7_Run(NSS7_HANDLE hNSS7);

/*  ��������: ������������
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *      char **ppMsgBody [OUT]:			���������Ϣ���ַ
 *      int *piMsgBodyLen [OUT]:		������Ϣ��ĳ���
 *      unsigned int *puiTransID [OUT]:	����������
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ճɹ�
 *				NSS7_E_WOULDBLOCK	��ʱû������
 *				С��NSS7_E_OK		���ӳ���Ĵ�����
 *  ��ע: ���ɹ����������Ϣ��Ҫ����NSS7_Free�ͷ��ڴ�,��Ȼ�����ڴ�й¶
 */
int NSS7_RecvRequest(NSS7_HANDLE hNSS7, char **ppMsgBody, int *piMsgBodyLen, unsigned int *puiTransID);

/*  ��������: ����������Ӧ
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *      char *pMsgBody [IN]:			��Ӧ��Ϣ�建��
 *      int iMsgBodyLen [IN]:			��Ӧ��Ϣ��ĳ���
 *      unsigned int uiTransID [IN]:	��Ӧ�������
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ͳɹ�
 *				NSS7_E_WOULDBLOCK	��������,��ʱ���ܷ���
 *				С��NSS7_E_OK		���ӳ���Ĵ�����
 */
int NSS7_SendResponse(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen, unsigned int uiTransID);

/*  ��������: ����֪ͨ
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *      char *pMsgBody [IN]:			֪ͨ��Ϣ�建��
 *      int iMsgBodyLen [IN]:			֪ͨ��Ϣ��ĳ���
 *      int iType [IN]:					��Ϣ����, ������"��Ϣ���Ͷ���"
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ͳɹ�
 *				NSS7_E_WOULDBLOCK	��������,��ʱ���ܷ���
 *				С��NSS7_E_OK		���ӳ���Ĵ�����
 */
int NSS7_SendNotify(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen);
int NSS7_SendNotifyEx(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen, int iType);

/*  ��������: ���������ϱ�
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *      char *pMsgBody [IN]:			�ϱ���Ϣ�建��
 *      int iMsgBodyLen [IN]:			�ϱ���Ϣ��ĳ���
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ͳɹ�
 *				NSS7_E_WOULDBLOCK	��������,��ʱ���ܷ���
 *				С��NSS7_E_OK		���ӳ���Ĵ�����
 *  ��ע: ֻ����ƽ̨�����Ӳſ��Է����ϱ�,���ڿͻ��˵����Ӵ˽ӿ���Ч
 */
int NSS7_SendReport(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen);

/*  ��������: �ͷ���Ϣ����
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *      void *pMsgBody [IN]:			Ҫ�Ƿ����Ϣ�建��
 */
void NSS7_Free(NSS7_HANDLE hNSS7, void *pMsgBody);

/*  ��������: �ͷ�NSS7ģ��
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *  ��ע: ��NSS7����������Ҫ��ʱ����Ҫ���ô˽ӿڹرվ��
 */
void NSS7_Close(NSS7_HANDLE hNSS7);

/*  ��������: ��ȡ������Ϣ
 *  ����˵��:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7ģ����
 *      unsigned int *puiLoginTime [OUT]:	���ӽ���ʱ��,UTCʱ��
 *      unsigned int *puiPeerIP [OUT]:		�Է�IP��ַ
 *		unsigned short *pusPeerPort [OUT]:	�Է��˿�
 *		char szUerID[NSS7_MAXLEN_USERID] [OUT]:		�����û���
 *		char szClientType[NSS7_MAXLEN_CLIENTTYPE] [OUT]:	��������
 *  ����ֵ: ������,ȡֵ����
 *				NSS7_E_OK			���ͳɹ�
 *				С��NSS7_E_OK		���ӳ���Ĵ�����
 */
int NSS7_GetSessionInfo(NSS7_HANDLE hNSS7, unsigned int *puiLoginTime,
						unsigned int *puiPeerIP, unsigned short *pusPeerPort,
						char szUerID[NSS7_MAXLEN_USERID], char szClientType[NSS7_MAXLEN_CLIENTTYPE]);

#ifdef __cplusplus
}
#endif

#endif //__NSS7_H__
