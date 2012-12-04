/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: NSS7_Defs.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: xiangmm
 *  �������: 2010-3-24 16:53:44
 *  ������ע: 
 *  
 */

#include "../XOSBase/XOS/XOS.h"
#include "../XOSBase/Utilities/MiniList.h"
#include "../XOSBase/Utilities/Proxy.h"
#include "../XOSBase/Utilities/DNS.h"
#include "../XOSBase/Utilities/md5.h"
#include "../XOSBase/Utilities/des.h"
#include "../XOSBase/Utilities/ObjList.h"
#include "../XOSBase/Utilities/mxml.h"
#include "../XOSBase/Utilities/mymxml.h"
#include "../XOSBase/Utilities/Str2Hex.h"
#include "NSS7.h"
#include "NSS7Packet.h"

//�����Ͷ���
#define NSS7HEADER_PACKETTYPE_DATA			1
#define NSS7HEADER_PACKETTYPE_KEEPALIVE		2

#define NSS7_MAXLEN_BUF						1024

//ͨ��ע��ʱ��״̬
typedef enum _NSS7Status
{
	NSS7_STATUS_CONNECT = 0x01,
	NSS7_STATUS_PROBE,
	NSS7_STATUS_SENDPOST,
	NSS7_STATUS_RECVPOSTHEAD,
	NSS7_STATUS_RECVPOSTBODY,
	NSS7_STATUS_RECVCONTENT,
	NSS7_STATUS_SENDHASH,
	NSS7_STATUS_RECVCERTIFY,
	NSS7_STATUS_RECVPARAMLEN,
	NSS7_STATUS_RECVPARAM,
	NSS7_STATUS_REGISTEROK
} NSS7Status;

//ģ�����ݽṹ
typedef struct _TNSS7 TNSS7; 
struct _TNSS7
{
	XOS_SOCKET m_sock;
	XOS_BOOL m_bDebug;
	XOS_U8 m_u8CtlFlag;
	XOS_BOOL m_bRecvHead;	//�Ƿ����ͷ
	TDes m_des;				//des����ģ������
	XOS_BOOL m_bRecvReqOk;
	NSS7Status m_status;
	XOS_BOOL m_bFirstPacketSendOK;	//��һ�����͵İ��Ƿ������
		
	XOS_U32 m_u32SendLen;
	XOS_U32 m_u32HaveSendLen;
	XOS_U32 m_u32LastSendTime;

	XOS_U32 m_u32RecvLen;
	XOS_U32 m_u32HaveRecvLen;
	XOS_U32 m_u32LastRecvTime;

	XOS_U32 m_u32KAInterval;
	XOS_U32 uiSessionTimeOut;

	TNSS7Packet *m_pSendPacket;		//���ڷ��͵İ�ָ��
	TNSS7Packet *m_pRecvPacket;		//���ڽ��յİ�ָ��
	
	TObjList *m_plistSendPacket;	//��Ҫ���͵İ�����

	//ֻ��ƽ̨��¼��ʱ����Ҫ��
	XOS_U16 m_u16PUIPort;			//ƽ̨port
	XOS_BOOL m_bReConnect;			//�Ƿ����ض��������
	XOS_BOOL m_bFixPUIAddr;
	char m_binBuf[NSS7_MAXLEN_BUF];

	//��ƽ̨ע��Ĳ���
	XOS_U8 m_binPswHash[16];
	char m_szPUIAddr[NSS7_MAXLEN_PUIADDR];
	char m_szContent[NSS7_MAXLEN_CONTENT];
	unsigned char binProducerPsw[64];

	//�������
	XOS_BOOL m_bProxy;
	char m_szProxyType[NSS7_MAXLEN_PROXYTYPE];
	char m_szProxyAddr[NSS7_MAXLEN_PROXYADDR];
	unsigned short m_usProxyPort;
	char m_szProxyUserName[NSS7_MAXLEN_PROXYUNAME];
	char m_szProxyPassword[NSS7_MAXLEN_PROXYPSW];
	TProxy *m_pProxy;
	
	unsigned int m_uiBirthTime;					//ֱ��������������Ч.
	char m_szUserID[NSS7_MAXLEN_USERID];
	char m_szClientType[NSS7_MAXLEN_CLIENTTYPE];

	//�Է��ĵ�ַ.
	unsigned int m_uiPeerIP;
	unsigned short m_usPeerPort;
	XOS_BOOL m_bFlatform;	//�Ƿ�����ƽ̨������.
};

#ifdef __cplusplus
extern "C" {
#endif

int NSS7_doRun(TNSS7 *pNSS7);

#ifdef __cplusplus
}
#endif
