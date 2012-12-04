/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: NSS7_Defs.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xiangmm
 *  完成日期: 2010-3-24 16:53:44
 *  修正备注: 
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

//包类型定义
#define NSS7HEADER_PACKETTYPE_DATA			1
#define NSS7HEADER_PACKETTYPE_KEEPALIVE		2

#define NSS7_MAXLEN_BUF						1024

//通道注册时的状态
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

//模块数据结构
typedef struct _TNSS7 TNSS7; 
struct _TNSS7
{
	XOS_SOCKET m_sock;
	XOS_BOOL m_bDebug;
	XOS_U8 m_u8CtlFlag;
	XOS_BOOL m_bRecvHead;	//是否接收头
	TDes m_des;				//des加密模块数据
	XOS_BOOL m_bRecvReqOk;
	NSS7Status m_status;
	XOS_BOOL m_bFirstPacketSendOK;	//第一个发送的包是否发送完毕
		
	XOS_U32 m_u32SendLen;
	XOS_U32 m_u32HaveSendLen;
	XOS_U32 m_u32LastSendTime;

	XOS_U32 m_u32RecvLen;
	XOS_U32 m_u32HaveRecvLen;
	XOS_U32 m_u32LastRecvTime;

	XOS_U32 m_u32KAInterval;
	XOS_U32 uiSessionTimeOut;

	TNSS7Packet *m_pSendPacket;		//正在发送的包指针
	TNSS7Packet *m_pRecvPacket;		//正在接收的包指针
	
	TObjList *m_plistSendPacket;	//需要发送的包链表

	//只有平台登录的时候需要的
	XOS_U16 m_u16PUIPort;			//平台port
	XOS_BOOL m_bReConnect;			//是否是重定向的连接
	XOS_BOOL m_bFixPUIAddr;
	char m_binBuf[NSS7_MAXLEN_BUF];

	//向平台注册的参数
	XOS_U8 m_binPswHash[16];
	char m_szPUIAddr[NSS7_MAXLEN_PUIADDR];
	char m_szContent[NSS7_MAXLEN_CONTENT];
	unsigned char binProducerPsw[64];

	//代理参数
	XOS_BOOL m_bProxy;
	char m_szProxyType[NSS7_MAXLEN_PROXYTYPE];
	char m_szProxyAddr[NSS7_MAXLEN_PROXYADDR];
	unsigned short m_usProxyPort;
	char m_szProxyUserName[NSS7_MAXLEN_PROXYUNAME];
	char m_szProxyPassword[NSS7_MAXLEN_PROXYPSW];
	TProxy *m_pProxy;
	
	unsigned int m_uiBirthTime;					//直连进来的连接有效.
	char m_szUserID[NSS7_MAXLEN_USERID];
	char m_szClientType[NSS7_MAXLEN_CLIENTTYPE];

	//对方的地址.
	unsigned int m_uiPeerIP;
	unsigned short m_usPeerPort;
	XOS_BOOL m_bFlatform;	//是否是与平台的连接.
};

#ifdef __cplusplus
extern "C" {
#endif

int NSS7_doRun(TNSS7 *pNSS7);

#ifdef __cplusplus
}
#endif
