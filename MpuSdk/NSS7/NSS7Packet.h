/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: NSS7Packet.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: NSS7Packet��ʵ��PU��PUI&Client֮��ͨ��Э���е����ݰ��ķ�װ
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2009-6-17 21:35:08
 *  ������ע: 
 *  
 */

#ifndef __NSS7PACKET_H__
#define __NSS7PACKET_H__

typedef struct _TNSS7Header
{
	XOS_U8	u8SyncFlag;		//��ʼ�ַ�,ȡ'#'
	XOS_U8	u8Version;		//Э��汾��,ȡ7
	XOS_U8	u8CtrlFlag;		//���Ʊ�ʶ,Ŀǰȡ0
	XOS_U8	u8PacketType;	//������,ȡֵ��ǰ��ĺ궨��

	XOS_U8	u8MsgType;		//��Ϣ����,ȡֵ��ǰ��ĺ궨��
	XOS_U8	Rsv1[3];		//����
	XOS_U32	u32TransID;		//��Ϣ�����
	XOS_U32	u32MsgLen;		//��Ϣ����
	XOS_U8	Rsv2[12];		//����
}TNSS7Header;

typedef struct _TNSS7Packet TNSS7Packet;

struct _TNSS7Packet
{
//public:
	void (*Construct)(TNSS7Packet *cthis);
	void (*Destroy)(TNSS7Packet *cthis);
	//iBodyLenΪ������Ϣ�峤��,������ͷ
	XOS_BOOL (*Create)(TNSS7Packet *cthis, int iBodyLen);
	
	//����ͷ,pHeaderΪ�����ͷ��Ϣ
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
	//ȡ����Ϣ��,ֻ�ܱ�ȡһ��,ȡ��֮�����GetBodyLen�᷵��NULL
	//ȡ�ߵ�Bodyָ���Ƿ�����׵�ַ����TNS7Header֮��ĵ�ַ,
	//�����ͷŵ�ʱ��,��Ҫ��ǰƫ��TNS7Header��С
	char* (*RemoveBody)(TNSS7Packet *cthis);
	//��ȡ����ʱ��,������Ϣ��ʱ�����������Ϣ�Ƿ�ʱ
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
