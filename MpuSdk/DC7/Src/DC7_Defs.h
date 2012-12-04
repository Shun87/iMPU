/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: DCM7_Defs.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: xiangmm
 *  完成日期: 2010-3-23 20:09:23
 *  修正备注: 
 *  
 */

#include "../../XOSBase/XOS/XOS.h"
#include "../../XOSBase/Utilities/MiniList.h"
#include "../../XOSBase/Utilities/Proxy.h"
#include "../../XOSBase/Utilities/DNS.h"
#include "../DC7.h"

#define DC7_TOKEN_LEN			64		//Token 最大长度

#define DC7_PACKET_MAXLEN		1400	//最大的包长度
#define DC7_SENDPACKET_MAXLEN	1024	//发送包的最大长度,
										//这个长度在大部分的网络中传输效率高

//DC7内部使用错误码
#define DC7_E_NODATA			2		//没有数据

//协议版本
#define DC7_PACKET_VER			0x03	//包版本

//包类型
#define	DC7_PACKET_DATA			0x00	//数据包
#define	DC7_PACKET_KEEP_ALIVE	0x01	//保活包
#define	DC7_PACKET_NOTIFY		0x02	//通知包

//数据通道当前所处的状态
#define	DC7_PS_CONNECTING		0x01    //连接中
#define	DC7_PS_CONNECTPROBE		0x02	//探测中
#define	DC7_PS_SEND_POST		0x03	//发送POST
#define	DC7_PS_RECV_STATUS		0x04	//接收状态
#define	DC7_PS_OK				0x05	//认证成功

//建立数据通道时,流分发服务返回的状态字
#define	DC7_STATUS_OK				0	//数据通道建立成功
#define	DC7_STATUS_ALREADY_EXIST	1	//数据通道已建立
#define	DC7_STATUS_TOKEN_INVALID	2	//令牌无效
#define	DC7_STATUS_TYPE_ERROR		3	//类型错误

//包头格式
typedef struct _TDC7PacketHead
{
	XOS_U8 u8Ver;			//协议版本号(固定为3)
	XOS_U8 u8Type;			//包类型(普通数据包:0,保活数据包:1)
	XOS_U16 u16Len;			//包数据的长度(不包括包头)
	XOS_U8 u8BeginFlag;		//该包是否是一帧的开始(是:1,否:0)
	XOS_U8 u8EndFlag;		//该包是否是一帧的结束(是:1,否:0)
	XOS_U8 u8IFrameFlag;	//是否是关键帧(是:1,否:0)
	XOS_U8 u8Rev;			//保留域(固定为0)
} TDC7PacketHead;

//帧头
typedef struct _TDC7FrameHead
{
	XOS_U8 u8DitherSetsIdx;
	XOS_U8 u8DitherIdx;
	XOS_U16 u16FrameNO;
	XOS_U8 u8FrameType;
	XOS_U8 u8Rsv1[3];
	XOS_U32 u32UTCTime;		//UTC时间,单位秒
	XOS_U32 u32Timestamp;	//这一帧的时间戳,单位ms,一般取设备这次启动到现在的时间
	XOS_U8 u8IFrameFlag;
	XOS_U8 u8Rsv2[3];	//保留
} TDC7FrameHead;

typedef struct _TDC7NotifyNode
{
	struct _TDC7NotifyNode *pNext;
	XOS_U32 u32Len;
	char *pbinBuffer;
} TDC7NotifyNode;

typedef struct _TDC7
{
	//传入参数
	char szAddr[DC7_MAX_ADDR_LEN+1];
	XOS_U16 u16Port;
	char szToken[DC7_TOKEN_LEN];

	XOS_BOOL bByProxy;
	char szProxyType[64];
	char szProxyAddr[DC7_MAX_PROXY_ADDR_LEN+1];
	unsigned short usProxyPort;
	char szProxyUserName[DC7_MAX_PROXY_USERNAME_LEN+1];
	char szProxyPassword[DC7_MAX_PROXY_PASSWORD_LEN+1];
	
	//内部使用的参数
	XOS_SOCKET sock;
	char *pRecvFrameBuf;
	XOS_U32 u32MaxRecvFrameLen;   //当前接收缓冲区的最大长度
	char *pSendFrameBuf;
	XOS_U32 u32MaxSendFrameLen;   //当前发送缓冲区的最大长度
	char binRecvPacket[DC7_PACKET_MAXLEN];
	char binSendPacket[DC7_PACKET_MAXLEN+1024];	//加上1024,是因为发送Post的时候,数据长
	TProxy *pProxy;				//代理模块指针

	TDC7PacketHead RecvHead;
	XOS_U32 u32RecvLen;			//接收数据的总长度
	XOS_U32 u32HaveRecvLen;		//已经接收的数据长度
	XOS_U32 u32RecvFrameOffset; //接收帧的位移量

	XOS_U32 u32SendLen;			//发送数据的总长度
	XOS_U32 u32HaveSendLen;		//已经发送的数据长度
	XOS_U32 u32SendFrameOffset; //发送帧的位移量

	XOS_BOOL bRecvHead;			//是否接收包头
	int iProbeStatus;

	XOS_U32 u32LastTryRecvTime;
	XOS_U32 u32LastRecvOKTime;
	XOS_U32 u32LastSendOKTime;
	XOS_U32 u32LastSendKATime;	//上次发送保活包的时间
	XOS_BOOL bFillSendPacket;   //是否已经填充的发送包
	XOS_U32 u32SendFrameLen;	//从上层获得发送数据帧的长度
	XOS_BOOL bSendFrameValid;   //表示发送缓冲的帧数据是否有效

	TMiniList lstNotify;
	TDC7FrameInfo RecvFrameInfo;
	TDC7FrameInfo SendFrameInfo;

	XOS_BOOL bRecvFrameOk;
	XOS_BOOL bSendEmptyPacket;
	XOS_BOOL bRecvEmptyPacketOk;

	unsigned int uiBirthTime;
	unsigned int uiTotalMBytes;	//MB级
	unsigned int uiTotalKBytes;	//KB级
	unsigned int uiTotalBytes;	//B级
	
	XOS_HANDLE hMutex;			//保护量
} TDC7;
