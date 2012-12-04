/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: DCM7_Defs.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: xiangmm
 *  �������: 2010-3-23 20:09:23
 *  ������ע: 
 *  
 */

#include "../../XOSBase/XOS/XOS.h"
#include "../../XOSBase/Utilities/MiniList.h"
#include "../../XOSBase/Utilities/Proxy.h"
#include "../../XOSBase/Utilities/DNS.h"
#include "../DC7.h"

#define DC7_TOKEN_LEN			64		//Token ��󳤶�

#define DC7_PACKET_MAXLEN		1400	//���İ�����
#define DC7_SENDPACKET_MAXLEN	1024	//���Ͱ�����󳤶�,
										//��������ڴ󲿷ֵ������д���Ч�ʸ�

//DC7�ڲ�ʹ�ô�����
#define DC7_E_NODATA			2		//û������

//Э��汾
#define DC7_PACKET_VER			0x03	//���汾

//������
#define	DC7_PACKET_DATA			0x00	//���ݰ�
#define	DC7_PACKET_KEEP_ALIVE	0x01	//�����
#define	DC7_PACKET_NOTIFY		0x02	//֪ͨ��

//����ͨ����ǰ������״̬
#define	DC7_PS_CONNECTING		0x01    //������
#define	DC7_PS_CONNECTPROBE		0x02	//̽����
#define	DC7_PS_SEND_POST		0x03	//����POST
#define	DC7_PS_RECV_STATUS		0x04	//����״̬
#define	DC7_PS_OK				0x05	//��֤�ɹ�

//��������ͨ��ʱ,���ַ����񷵻ص�״̬��
#define	DC7_STATUS_OK				0	//����ͨ�������ɹ�
#define	DC7_STATUS_ALREADY_EXIST	1	//����ͨ���ѽ���
#define	DC7_STATUS_TOKEN_INVALID	2	//������Ч
#define	DC7_STATUS_TYPE_ERROR		3	//���ʹ���

//��ͷ��ʽ
typedef struct _TDC7PacketHead
{
	XOS_U8 u8Ver;			//Э��汾��(�̶�Ϊ3)
	XOS_U8 u8Type;			//������(��ͨ���ݰ�:0,�������ݰ�:1)
	XOS_U16 u16Len;			//�����ݵĳ���(��������ͷ)
	XOS_U8 u8BeginFlag;		//�ð��Ƿ���һ֡�Ŀ�ʼ(��:1,��:0)
	XOS_U8 u8EndFlag;		//�ð��Ƿ���һ֡�Ľ���(��:1,��:0)
	XOS_U8 u8IFrameFlag;	//�Ƿ��ǹؼ�֡(��:1,��:0)
	XOS_U8 u8Rev;			//������(�̶�Ϊ0)
} TDC7PacketHead;

//֡ͷ
typedef struct _TDC7FrameHead
{
	XOS_U8 u8DitherSetsIdx;
	XOS_U8 u8DitherIdx;
	XOS_U16 u16FrameNO;
	XOS_U8 u8FrameType;
	XOS_U8 u8Rsv1[3];
	XOS_U32 u32UTCTime;		//UTCʱ��,��λ��
	XOS_U32 u32Timestamp;	//��һ֡��ʱ���,��λms,һ��ȡ�豸������������ڵ�ʱ��
	XOS_U8 u8IFrameFlag;
	XOS_U8 u8Rsv2[3];	//����
} TDC7FrameHead;

typedef struct _TDC7NotifyNode
{
	struct _TDC7NotifyNode *pNext;
	XOS_U32 u32Len;
	char *pbinBuffer;
} TDC7NotifyNode;

typedef struct _TDC7
{
	//�������
	char szAddr[DC7_MAX_ADDR_LEN+1];
	XOS_U16 u16Port;
	char szToken[DC7_TOKEN_LEN];

	XOS_BOOL bByProxy;
	char szProxyType[64];
	char szProxyAddr[DC7_MAX_PROXY_ADDR_LEN+1];
	unsigned short usProxyPort;
	char szProxyUserName[DC7_MAX_PROXY_USERNAME_LEN+1];
	char szProxyPassword[DC7_MAX_PROXY_PASSWORD_LEN+1];
	
	//�ڲ�ʹ�õĲ���
	XOS_SOCKET sock;
	char *pRecvFrameBuf;
	XOS_U32 u32MaxRecvFrameLen;   //��ǰ���ջ���������󳤶�
	char *pSendFrameBuf;
	XOS_U32 u32MaxSendFrameLen;   //��ǰ���ͻ���������󳤶�
	char binRecvPacket[DC7_PACKET_MAXLEN];
	char binSendPacket[DC7_PACKET_MAXLEN+1024];	//����1024,����Ϊ����Post��ʱ��,���ݳ�
	TProxy *pProxy;				//����ģ��ָ��

	TDC7PacketHead RecvHead;
	XOS_U32 u32RecvLen;			//�������ݵ��ܳ���
	XOS_U32 u32HaveRecvLen;		//�Ѿ����յ����ݳ���
	XOS_U32 u32RecvFrameOffset; //����֡��λ����

	XOS_U32 u32SendLen;			//�������ݵ��ܳ���
	XOS_U32 u32HaveSendLen;		//�Ѿ����͵����ݳ���
	XOS_U32 u32SendFrameOffset; //����֡��λ����

	XOS_BOOL bRecvHead;			//�Ƿ���հ�ͷ
	int iProbeStatus;

	XOS_U32 u32LastTryRecvTime;
	XOS_U32 u32LastRecvOKTime;
	XOS_U32 u32LastSendOKTime;
	XOS_U32 u32LastSendKATime;	//�ϴη��ͱ������ʱ��
	XOS_BOOL bFillSendPacket;   //�Ƿ��Ѿ����ķ��Ͱ�
	XOS_U32 u32SendFrameLen;	//���ϲ��÷�������֡�ĳ���
	XOS_BOOL bSendFrameValid;   //��ʾ���ͻ����֡�����Ƿ���Ч

	TMiniList lstNotify;
	TDC7FrameInfo RecvFrameInfo;
	TDC7FrameInfo SendFrameInfo;

	XOS_BOOL bRecvFrameOk;
	XOS_BOOL bSendEmptyPacket;
	XOS_BOOL bRecvEmptyPacketOk;

	unsigned int uiBirthTime;
	unsigned int uiTotalMBytes;	//MB��
	unsigned int uiTotalKBytes;	//KB��
	unsigned int uiTotalBytes;	//B��
	
	XOS_HANDLE hMutex;			//������
} TDC7;
