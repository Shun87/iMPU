/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: DC7.h
 *  �ļ���ʶ: ����ͨ��ģ��(Data Channel)
 *  ժ    Ҫ: ��ģ�������7.0Э��������ͨ���ķ�װ.
 *			  ����֡��Ϣ�е�һЩ�����ͺ궨��ĸ�����ϸ˵��
 *			  ��μ��豸����Э���ĵ�
 *			  ֧�ֶ��̵߳���,�ڲ��л���.
 *  ��ǰ�汾: 
 *  ��    ��: xiangmm
 *  �������: 2010-3-23 20:05:22
 *  ������ע: 
 *  
 */

#ifndef __DC7_H__
#define __DC7_H__

#ifdef WIN32
	#define DC7_STDCALL	__stdcall
	#ifdef _DC7LIB	//���ʹ��Lib��,��ҪԤ���������
		#define DC7DLL_API DC7_STDCALL
	#else
		#ifdef DC7DLL_EXPORTS
		#define DC7DLL_API __declspec(dllexport) DC7_STDCALL
		#else
		#define DC7DLL_API __declspec(dllimport) DC7_STDCALL
		#endif
	#endif
#else
	#define DC7_STDCALL
	#define DC7DLL_API 
#endif

//�ڲ�ʵ����غ궨��
#define DC7_VERSION				"v7.1.1.0527"	//��İ汾
#define DC7_FRAME_MAXLEN		(1024*1024)		//�����Ƶ֡����,���Ǹ�������
												//�����Ǹ����ʱ��,150k�Ϳ�����
												//��������ǲο�����,DC7���ڲ������κ��޶�
#define DC7_CONNECT_MAXTIME		(15*1000)		//���ӳ�ʱʱ��
#define DC7_KEEPALIVE_INTERVAL	(5*1000)		//���ͱ�����ļ��
#define DC7_MAXNOTIFYLIST_LEN	256				//֪ͨ�������󳤶�

/*
 *	�����붨��
 */
	
//�ǹؼ�����,����Ҫ�ͷ�ͨ��,��������,�ؼ�������Ҫ�ͷ�ͨ��,��������.
//�ǹؼ������ֵ������0,�ɹ�����0,�ؼ�����Ķ�С��0
#define DC7_E_OK				0   //�ɹ���־
#define DC7_E_WOULDBLOCK		1   //������ʱ�޷����
#define DC7_E_FRAMEBUF_LEN		2	//֡���ȴ���,
									//�����ʾ���յ���֡���ȴ���DC7_RecvFrame�������󳤶�
									//�ϲ�Ӧ�����󻺳壬�������Ӧ�Ĵ���
#define DC7_E_END				3	//���յ�������,����������ɵ�

//һ��ؼ�����
#define DC7_E_UNKNOW			-1  //δ�������,һ�㲻�᷵�����
									//DEBUG��Ŀ�����assert����.
#define DC7_E_PARAM				-2	//��������
#define DC7_E_IVALHANDLE		-3  //��ЧDC7���
#define DC7_E_MALLOC			-4  //�ڴ����
#define DC7_E_TIMEOUT			-5  //��ʱ
#define DC7_E_CONNECT			-6  //���Ӵ���
#define DC7_E_TCPSEND			-7  //TCP���ͳ���
#define DC7_E_TCPRECV			-8  //TCP���ճ���

//���������Ƿ��������صĴ���
#define DC7_E_INVTOKEN			-101 //��Ч���� 
#define DC7_E_CHTYPEERR			-102 //ͨ�����ʹ���
#define DC7_E_CHEXIST			-103 //ͨ���Ѿ�����

//���ݴ���
#define DC7_E_PACKET_VER		-201 //���汾����
#define DC7_E_PACKET_TYPE		-202 //�����ʹ���
#define DC7_E_PACKET_LEN		-203 //�����ȴ���
#define DC7_E_PACKET_BFLAG		-204 //֡��ʼ��־����
#define DC7_E_PACKET_EFLAG		-205 //֡������־����
#define DC7_E_FRAME_TYPE		-211 //֡���ʹ���
#define DC7_E_FRAME_LEN			-212 //����֡���ȴ���

//��󳤶ȶ���
#define DC7_MAX_ADDR_LEN				(255-8)	//����Ŀ���ַ����󳤶�,��������β0
												//��ȥ8��Ϊ�˺ϳɵ�ʱ��,��Port��
#define DC7_MAX_PROXY_ADDR_LEN			(255-8)	//�����ַ����󳤶�,��������β0
												//��ȥ8��Ϊ�˺ϳɵ�ʱ��,��Port��
#define DC7_MAX_PROXY_USERNAME_LEN		63	//�����û�������󳤶�,��������β0
#define DC7_MAX_PROXY_PASSWORD_LEN		63	//�����������󳤶�,��������β0

//��Ƶ�㷨
#define	DC7_VIDEOALG_JPEG		0
#define	DC7_VIDEOALG_MPEG1		1
#define	DC7_VIDEOALG_MPEG2		2
#define	DC7_VIDEOALG_MJPEG		3
#define	DC7_VIDEOALG_MPEG4		4
#define	DC7_VIDEOALG_H263		5
#define	DC7_VIDEOALG_H264		6
#define	DC7_VIDEOALG_AVS		7
#define	DC7_VIDEOALG_S264		8
#define	DC7_VIDEOALG_Q264		9

//��Ƶ�㷨
#define	DC7_AUDIOALG_G7231		0
#define	DC7_AUDIOALG_G729AB		1
#define	DC7_AUDIOALG_G726		2
#define	DC7_AUDIOALG_G722		3
#define	DC7_AUDIOALG_MP3		4
#define	DC7_AUDIOALG_ADPCM		5
#define	DC7_AUDIOALG_PCM		6
#define	DC7_AUDIOALG_G711A		7
#define	DC7_AUDIOALG_G711U		8
#define DC7_AUDIOALG_AMR        9

//ͼ���ʽ
#define	DC7_PICALGID_DAMAGEJPEG		0  //����JPEG
#define	DC7_PICALGID_NODAMAGEJPEG	1  //����JPEG
#define	DC7_PICALGID_JPEG2000		2
#define	DC7_PICALGID_BMP			3
#define	DC7_PICALGID_PNG			4

//֡����
#define DC7_FRAME_VIDEO			1	//��Ƶ֡
#define	DC7_FRAME_AUDIO			2	//��Ƶ֡
#define	DC7_FRAME_DATA			3	//����֡
#define	DC7_FRAME_GPS			4	//GPS֡
#define	DC7_FRAME_SP			5	//����֡
#define	DC7_FRAME_PIC			6	//ͼ��֡

//��Ƶ֡��Ϣ
typedef struct _TDC7VideoFrameInfo
{
	unsigned short u16Width;		//��Ƶ֡�ֱ���(���): 288
	unsigned short u16Height;		//��Ƶ֡�ֱ���(�߶�): 352
	unsigned short u16ProducerID;	//����ID, ��: �����ĳ���ID��1
	unsigned char u8FrmRate;		//֡��
	unsigned char u8Reserved1;		//����λ
	unsigned int u32Reserved2;		//����λ
	unsigned int u32Reserved3;		//����λ
	//�����Ǵ���˽�и�ʽ�е�
	unsigned char u8AlgID;			//�㷨��־, ���������ĺ궨��
	unsigned char u8Rsv[3];			//����λ
	unsigned char u8PrivateData[8]; //˽������, û�е�ʱ���ȫ��0
} TDC7VideoFrameInfo;

//��Ƶ֡��Ϣ
typedef struct _TDC7AudioFrameInfo
{
	unsigned short u16BlockAlign;	//ÿ���㷨֡�İ������ֽ���
	unsigned char u8Channels;		//ͨ������, һ����1��2
	unsigned char u8BitsPerSample;	//PCM��ʽʱ�Ĳ�������, һ����8bit��16bit
	unsigned short u16SamplesPerSec;//PCM��ʽʱ�Ĳ����ʳ���100. ����: 8K������80
	unsigned short u16AlgFrmNum;	//�����㷨֡�ĸ���, ���ֵӦ�ñ��ֲ���
	unsigned short u16ProducerID;	//����ID, ��:�����ĳ���ID��1
	unsigned short u16PCMLen;		//һ���㷨֡������PCM���ݳ���
	unsigned int u32Reserved;		//����λ
} TDC7AudioFrameInfo;

//GPS֡��Ϣ
typedef struct _TDC7GPSFrameInfo
{
	unsigned short u16FrmCnt;		//��һ֡������GPS������ĸ���
	unsigned char u8Reserved[2];    //����
} TDC7GPSFrameInfo;

//��������
typedef struct _TDC7SPFrameInfo
{
	unsigned short u16FrmCnt;		//��һ֡�����Ĵ���֡�ĸ���
	unsigned char u8OfflineFlg;		//1Ϊ�豸����ʱ�������, 0Ϊʵʱ����
	unsigned char u8Reserved;		//����
} TDC7SPFrameInfo;

//ͼ������
typedef struct _TDC7PICFrameInfo
{
	unsigned short u16Width;		//ͼ����, ��λ����
	unsigned short u16Height;		//ͼ��߶�, ��λ����
	unsigned char u8StandardAlg;	//��׼�㷨ID. �����Ǳ�׼ѹ���㷨.
									//ȡֵ��ǰ��DC7_PICALGID_DAMAGEJPEG�Ⱥ궨��
	unsigned char u8Rsv[3];
} TDC7PICFrameInfo;

//֡��Ϣ
typedef struct _TDC7FrameInfo
{
	unsigned short u16FrameNO;		//֡���
	unsigned char u8FrameType;		//֡����, �����涨��
	unsigned char u8Rsv;			//����, ��0
	unsigned int u32UTCTime;		//UTCʱ��, ��λ��
	unsigned int u32Timestamp;		//ʱ���, ��λ����
	unsigned char bIFrame;			//1��ʾ�ؼ�֡, 0��ʾ�ǹؼ�֡
	unsigned char u8Rsv2[3];		//����, ��0
	union
	{
		TDC7VideoFrameInfo video;	//��Ƶ֡��Ϣ
		TDC7AudioFrameInfo audio;	//��Ƶ֡��Ϣ
		TDC7GPSFrameInfo GPS;		//GPS֡��Ϣ
		TDC7SPFrameInfo SP;			//����֡��Ϣ
		TDC7PICFrameInfo PIC;		//ͼƬ֡��Ϣ
	}Res;
} TDC7FrameInfo;

typedef void* DC7_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ��ʼ��DNS��ѯģ��
 *  ����˵��:
 *  ����ֵ: 0 ��ʾ�ɹ�, -1 ��ʾʧ��
 *  ��ע: ���Ҫʹ��DNS�Ļ�, ��������������.
 */
int DC7DLL_API DC7_DNS2IPInit();

/*  ��������: ��������ͨ��(����Э�̹���)
 *			  ͨ������������ͨ��(����Э�̹���)
 *			  ��������ͨ��(����Э�̹���)	
 *  ����˵��:
 *		DC7_HANDLE *phDC7 [OUT]:		�����ӿ���,ͨ��������ؽ�����DC7���
 *      const char *cpszAddr [IN]:		�ַ�����ĵ�ַ,IP���������ַ���,��󳤶ȼ�ǰ��ĺ궨��
 *										���ʹ�������Ļ�,�������DNS2IPInit����,����ͬ
 *      unsigned short u16Port [IN]:	�ַ�����Ķ˿�
 *      const char *cpszToken [IN]:		���ӷַ������Token
 *      const char *cpszChID [IN]:		ChID,û�е�ʱ����NULL
 *      const char *cpszChData [IN]:	ChData,û�е�ʱ����NULL
 *		const char *cpszHost [IN]:		�û�ָ����Host����Ϣ,��ʽip:port����dns:port,
 *										����NULL���߿��ַ�����ʾʹ�����ӵ�Ŀ���ַ��ΪHost��Ϣ��.
 *      const char *pszProxyType [IN]:	��������,ȡֵ�ͺ�������:
 *											"HTTP"		������֤��HTTP����
 *											"SOCKET4"	SOCKS4����
 *											"SOCKET5"	SOCKS5����
 *											"NTLM"		NTLM��֤��HTTP����
 *      const char *pszProxyAddr [IN]:	�����ַ,IP���������ַ���,��󳤶ȼ�ǰ��ĺ궨��
 *		unsigned short usProxyPort [IN]:����Ķ˿�
 *      const char *pszProxyUsername [IN]:	���Ӵ�����û���,��󳤶ȼ�ǰ��ĺ궨��
 *      const char *pszProxyPassword [IN]:	���Ӵ��������,��󳤶ȼ�ǰ��ĺ궨��
 *  ����ֵ: �������ɹ�����DC7_HANDLE���, ʧ�ܷ���NULL
 *			�����ӿڷ���DC7_E_OK,��ʾ�����ɹ�,������ʾ������
 */
DC7_HANDLE DC7DLL_API DC7_Create(const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost);
DC7_HANDLE DC7DLL_API DC7_CreateByProxy(const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost,
				 	char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
					char *pszProxyUsername, char *pszProxyPassword);
int DC7DLL_API DC7_CreateBlock(DC7_HANDLE *phDC7, const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost);
int DC7DLL_API DC7_CreateByProxyBlock(DC7_HANDLE *phDC7, const char *cpszAddr, unsigned short u16Port,
					const char *cpszToken, const char *cpszChID, const char *cpszChData,
					const char *cpszHost,
					char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
					char *pszProxyUsername, char *pszProxyPassword);

/*  ��������: ̽������ͨ���Ƿ����ӳɹ�
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *  ����ֵ:		DC7_E_OK			Э�̳ɹ�����
 *				DC7_E_WOULDBLOCK	��ʾ��Ҫ����̽��
 *				�����������ΪС��0��, ��Ҫ�ر�ͨ��.
 */
int DC7DLL_API DC7_GetConnectStatus(DC7_HANDLE hDC7);

/*  ��������: ��������ͨ��(����ҪЭ��,����ǰ���豸�Ĵ���ֱ��������ͨ��)
 *  ����˵��:
 *      int sock [IN]:					Э�̹����sock.
 *  ����ֵ: �ɹ�����DC7_HANDLE���, ʧ�ܷ���NULL.
 *  ��ע: ����������sock���óɷ�������,
 *		  ���ù����֮��,����Ҫ����DC7_GetConnectStatus��ȡ����״̬.
 */
DC7_HANDLE DC7DLL_API DC7_CreateWithoutChat(int sock);

/*  ��������: ���÷��ͻ���Ĵ�С
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      int iSize [IN]:					���ͻ����С,��λ�ֽ�
 *  ��ע: ���������ӳɹ�֮����ܹ������������
 */
void DC7DLL_API DC7_SetSendBufferSize(DC7_HANDLE hDC7, int iSize);

/*  ��������: ���ý��ջ���Ĵ�С
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      int iSize [IN]:					���ջ����С,��λ�ֽ�
 *  ��ע: ���������ӳɹ�֮����ܹ������������
 */
void DC7DLL_API DC7_SetRecvBufferSize(DC7_HANDLE hDC7, int iSize);

/*  ��������: DC7����ģ��.
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *  ����ֵ:		DC7_E_OK			���ȳɹ�
 *				������ΪС��0�Ĵ����붼��ҪҪ�ر�ͨ��.
 */
int DC7DLL_API DC7_Run(DC7_HANDLE hDC7);

/*  ��������: ������ͨ���ж�ȡһ֡����.
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      char *binFrameBuf [OUT]:		����֡�Ĵ�ŵ�ַ, ��Ҫ�����߿���.
 *      unsigned int *pu32BufLen [IN, OUT]: ����ʱΪ�������󳤶�,��󳤶ȿ�ΪDC7_FRAME_MAXLEN,
											����ʱΪ����֡��ʵ�ʳ���.
 *      TDC7FrameInfo *pFrameInfo [OUT]: ����֡�������Ϣ.
 *  ����ֵ:		DC7_E_OK			��������֡�ɹ�
 *				DC7_E_WOULDBLOCK	��ȡ����֡û�ɹ�, ��Ҫ�������� 
 *				DC7_E_FRAMEBUF_LEN	��ʾ���뻺�峤�Ȳ���,��ʱpu32BufLenΪ���յ���֡����,�ϲ�Ӧ���޸Ļ���ĳ���
 *				DC7_E_END			��ʾ��ɽ���,��ʾ���յ���һ���հ�.(����Լ��, ���ԶϿ������ͨ��)
 *				������С��0�Ĵ�����Ҫ�ر�ͨ��.
 *  ��ע: 1�������յ�������Ƶ֡ʱ, binFrameBuf�е������������:
 *			 TDC7FrameInfo.aduio.u16AlgFrmNum��TDC7FrameInfo.aduio.u16BlockAlign���ȵ��㷨֡
 *		     ÿ���㷨֡�ĸ�ʽ����ο�<NRCAP2.0���ݴ���ĸ�ʽ_�������ݸ�ʽ.doc>�ĵ�
 *		  2��֧�ֿ�֡�Ľ���, ���ճɹ�����DC7_E_OK, ���ݳ���Ϊ0, ֡��Ϣ��Ч.
 */
int DC7DLL_API DC7_RecvFrame(DC7_HANDLE hDC7, char *binFrameBuf, unsigned int *pu32BufLen, TDC7FrameInfo *pFrameInfo);

/*  ��������: ������ͨ���ж�ȡһ֡������֡(�����һ������,�����������ȡ֡��Ϣ)
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      char *binFrameBuf [OUT]:		����֡�Ĵ�ŵ�ַ, ��Ҫ�����߿���.
 *      unsigned int *pu32BufLen [IN, OUT]: ����ʱΪ�������󳤶�,��󳤶ȿ�ΪDC7_FRAME_MAXLEN+sizeof(TDC7FrameInfo)
											����ʱΪ����֡��ʵ�ʳ���.
 *      int *piFrameType [OUT]:			֡����,ȡֵ��ǰ��궨��	
 *      int *pbKeyFrame [OUT]:			�Ƿ��ǹؼ�֡,1��ʾ��,0��ʾ����
 *  ����ֵ:	ͬDC7_RecvFrame����.
 *	��ע:	֧�ֽ��տ�֡, ��ʱֻ����֡��Ϣ��֡��Ϣ����.
 */
int DC7DLL_API DC7_RecvRawFrame(DC7_HANDLE hDC7, char *binFrameBuf, unsigned int *pu32BufLen, int *piFrameType, int *pbKeyFrame);

/*  ��������: ��һ֡����д������ͨ����.
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      char *binSendFrame [IN]:		��Ҫ���͵�����֡�Ļ���.
 *      unsigned int u32FrameLen [IN]:	��Ҫ��������֡�ĳ���,���ܹ�����DC7_FRAME_MAXLEN
 *      TDC7FrameInfo *pFrameinfo [IN]:	��Ҫ��������֡��֡��Ϣ.
 *  ����ֵ:		DC7_E_OK			��������֡�ɹ�, 
 *				DC7_E_WOULDBLOCK	��������֡û�ɹ�, 
 *				������С��0�Ĵ�����Ҫ�ر�ͨ��.
 *  ��ע: 1�������͵�����Ƶ֡ʱ,binFrameBuf�е��������ͬDC7_RecvFrame����.
 *		  2��֧�ֿ�֡�ķ���, ��ʱ��Ҫ��������ݳ���Ϊ0����Ч֡��Ϣ.
 */
int DC7DLL_API DC7_SendFrame(DC7_HANDLE hDC7, char *binSendFrame, unsigned int u32FrameLen, TDC7FrameInfo *pFrameinfo);

/*  ��������: ����һ���յ����ݰ�
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]: ��Ч���
 *  ����ֵ: int ͬDC7_SendFrame;
 */
int DC7DLL_API DC7_SendEmptyPacket(DC7_HANDLE hDC7);

/*  ��������: ��һ֡������д������ͨ����(�����һ������,�����������Ĳ���֡��Ϣ).
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      char *binSendFrame [IN]:		��Ҫ���͵�����֡�Ļ���.
 *      unsigned int u32FrameLen [IN]:	��Ҫ��������֡�ĳ���,���ܹ�����DC7_FRAME_MAXLEN
 *      int iFrameType [IN]:			֡����,ȡֵ��ǰ��ĺ궨��.
 *		unsigned short u16FrameNO [IN]:	֡���,�����ж�ת���������Ƿ�֡.
 *  ����ֵ:		DC7_E_OK			��������֡�ɹ�, 
 *				DC7_E_WOULDBLOCK	��������֡û�ɹ�, 
 *				������С��0�Ĵ�����Ҫ�ر�ͨ��.
 */
int DC7DLL_API DC7_SendRawFrame(DC7_HANDLE hDC7, char *binSendFrame, unsigned int u32FrameLen, int iFrameType, unsigned short u16FrameNO);

/*  ��������: ��֪ͨ�����л��һ��֪ͨ.
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *      char *pbinNotifyBuf [OUT]:		֪ͨ�Ĵ�ŵ�ַ, ��Ҫ�����߿���.
 *      unsigned int *pu32BufLen [IN, OUT]: ����ʱΪ�������󳤶�, ����ʱΪ֪ͨ���ݵ�ʵ�ʳ���.
 *  ����ֵ:		DC7_E_OK			��ʾ����֪ͨ�ɹ�
 *				DC7_E_WOULDBLOCK	��ʾû�н��յ�֪ͨ
 * 				DC7_E_FRAMEBUF_LEN	��ʾ���뻺�峤�Ȳ���,��ʱpu32BufLenΪ���յ���֡����,�ϲ�Ӧ���޸Ļ���ĳ���
 *				������С��0�Ĵ�����Ҫ�ر�ͨ��.
 */
//int DC7DLL_API DC7_RecvNotify(DC7_HANDLE hDC7, char *binNotifyBuf, unsigned int *pu32BufLen);

/*  ��������: �ر�DC7ģ��
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:			��ЧDC7_HANDLE���.
 *  ��ע: ����Create�ɹ�������ͨ��������ô˽ӿڹر�, ������ڴ�й¶	
 */
void DC7DLL_API DC7_Close(DC7_HANDLE hDC7);

/*  ��������: ��ȡ����ͨ����Ϣ.
 *  ����˵��:
 *      DC7_HANDLE hDC7 [IN]:		��ЧDC7_HANDLE���.
 *      unsigned int *puiBirthTime [OUT]:	ͨ������UTCʱ��.
 *      unsigned int *puiPeerIP [OUT]:	�Է���IP
 *      unsigned short *puiPeerPort [OUT]:	�Է��Ķ˿�
 *      unsigned int *puiTotalMBytes [OUT]: �շ��ֽ�����MB������.
 *      unsigned int *puiTotalKBytes [OUT]: �շ��ֽ�����KB������.
 *  ����ֵ: DC7DLL_API int �����ɹ��󷵻�DC7_E_OK, ���򷵻�����.
 */
int DC7DLL_API DC7_GetChnInfo(DC7_HANDLE hDC7, unsigned int *puiBirthTime, 
							  unsigned int *puiPeerIP, unsigned short *puiPeerPort, 
							  unsigned int *puiTotalMBytes, unsigned int *puiTotalKBytes);

#ifdef __cplusplus
}
#endif

#endif //__DC7_H__
