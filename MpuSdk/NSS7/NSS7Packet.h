/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: NSS7Packet.h
 *  文件标识: 
 *  摘    要: NSS7Packet类实现PU和PUI&Client之间通信协议中的数据包的封装
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2009-6-17 21:35:08
 *  修正备注: 
 *  
 */

#ifndef __NSS7PACKET_H__
#define __NSS7PACKET_H__

typedef struct _TNSS7Header
{
	XOS_U8	u8SyncFlag;		//起始字符,取'#'
	XOS_U8	u8Version;		//协议版本号,取7
	XOS_U8	u8CtrlFlag;		//控制标识,目前取0
	XOS_U8	u8PacketType;	//包类型,取值见前面的宏定义

	XOS_U8	u8MsgType;		//消息类型,取值见前面的宏定义
	XOS_U8	Rsv1[3];		//保留
	XOS_U32	u32TransID;		//消息事务号
	XOS_U32	u32MsgLen;		//消息长度
	XOS_U8	Rsv2[12];		//保留
}TNSS7Header;

typedef struct _TNSS7Packet TNSS7Packet;

struct _TNSS7Packet
{
//public:
	void (*Construct)(TNSS7Packet *cthis);
	void (*Destroy)(TNSS7Packet *cthis);
	//iBodyLen为完整消息体长度,不包括头
	XOS_BOOL (*Create)(TNSS7Packet *cthis, int iBodyLen);
	
	//设置头,pHeader为输入的头信息
	void (*SetHeader)(TNSS7Packet *cthis, TNSS7Header *pHeader);
	void (*FillHeader)(TNSS7Packet *cthis, unsigned char ucCtlflag,
					   unsigned char ucPacketType, unsigned char ucMsgType, 
					   unsigned int uiTransID);
	XOS_BOOL (*FillBody)(TNSS7Packet *cthis, char *pFill, int iFillLen);

	char* (*GetBuffer)(TNSS7Packet *cthis);
	int	 (*GetBufferLen)(TNSS7Packet *cthis);
	char* (*GetBody)(TNSS7Packet *cthis);
	int	 (*GetBodyLen)(TNSS7Packet *cthis);
	void (*GetHeader)(TNSS7Packet *cthis, TNSS7Header *pHeader);
	//取走消息体,只能被取一次,取走之后调用GetBodyLen会返回NULL
	//取走的Body指针是分配的首地址加上TNS7Header之后的地址,
	//所以释放的时候,需要往前偏移TNS7Header大小
	char* (*RemoveBody)(TNSS7Packet *cthis);
	//获取出生时间,发送消息的时候用来检查消息是否超时
	XOS_U32 (*GetBirthTime)(TNSS7Packet *cthis);

//private:
	char *m_pBuffer;
	char *m_pBody;
	int m_iBodyLen;
	XOS_U32 m_u32BirthTime;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TNSS7Packet NSS7PACKET;

#ifdef __cplusplus
}
#endif

#endif //__NSS7PACKET_H__
