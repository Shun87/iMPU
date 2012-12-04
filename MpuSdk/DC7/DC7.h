/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: DC7.h
 *  文件标识: 数据通道模块(Data Channel)
 *  摘    要: 本模块完成了7.0协议中数据通道的封装.
 *			  关于帧信息中的一些参数和宏定义的更加详细说明
 *			  请参见设备接入协议文档
 *			  支持多线程调用,内部有互斥.
 *  当前版本: 
 *  作    者: xiangmm
 *  完成日期: 2010-3-23 20:05:22
 *  修正备注: 
 *  
 */

#ifndef __DC7_H__
#define __DC7_H__

#ifdef WIN32
	#define DC7_STDCALL	__stdcall
	#ifdef _DC7LIB	//如果使用Lib版,需要预定义这个宏
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

//内部实现相关宏定义
#define DC7_VERSION				"v7.1.1.0527"	//库的版本
#define DC7_FRAME_MAXLEN		(1024*1024)		//最大视频帧长度,考虑高清的情况
												//不考虑高清的时候,150k就可以了
												//这个仅仅是参考长度,DC7库内部不做任何限定
#define DC7_CONNECT_MAXTIME		(15*1000)		//连接超时时间
#define DC7_KEEPALIVE_INTERVAL	(5*1000)		//发送保活包的间隔
#define DC7_MAXNOTIFYLIST_LEN	256				//通知链表的最大长度

/*
 *	错误码定义
 */
	
//非关键错误,不需要释放通道,重新连接,关键错误需要释放通道,重新连接.
//非关键错误的值都大于0,成功等于0,关键错误的都小于0
#define DC7_E_OK				0   //成功标志
#define DC7_E_WOULDBLOCK		1   //操作暂时无法完成
#define DC7_E_FRAMEBUF_LEN		2	//帧长度错误,
									//这个表示接收到的帧长度大于DC7_RecvFrame传入的最大长度
									//上层应该扩大缓冲，或进行相应的处理
#define DC7_E_END				3	//接收到结束包,比如下载完成等

//一般关键错误
#define DC7_E_UNKNOW			-1  //未定义错误,一般不会返回这个
									//DEBUG版的库会出现assert错误.
#define DC7_E_PARAM				-2	//参数错误
#define DC7_E_IVALHANDLE		-3  //无效DC7句柄
#define DC7_E_MALLOC			-4  //内存错误
#define DC7_E_TIMEOUT			-5  //超时
#define DC7_E_CONNECT			-6  //连接错误
#define DC7_E_TCPSEND			-7  //TCP发送出错
#define DC7_E_TCPRECV			-8  //TCP接收出错

//建立连接是服务器返回的错误
#define DC7_E_INVTOKEN			-101 //无效令牌 
#define DC7_E_CHTYPEERR			-102 //通道类型错误
#define DC7_E_CHEXIST			-103 //通道已经存在

//数据错误
#define DC7_E_PACKET_VER		-201 //包版本错误
#define DC7_E_PACKET_TYPE		-202 //包类型错误
#define DC7_E_PACKET_LEN		-203 //包长度错误
#define DC7_E_PACKET_BFLAG		-204 //帧开始标志错误
#define DC7_E_PACKET_EFLAG		-205 //帧结束标志错误
#define DC7_E_FRAME_TYPE		-211 //帧类型错误
#define DC7_E_FRAME_LEN			-212 //数据帧长度错误

//最大长度定义
#define DC7_MAX_ADDR_LEN				(255-8)	//代理目标地址的最大长度,不包括结尾0
												//减去8是为了合成的时候,放Port用
#define DC7_MAX_PROXY_ADDR_LEN			(255-8)	//代理地址的最大长度,不包括结尾0
												//减去8是为了合成的时候,放Port用
#define DC7_MAX_PROXY_USERNAME_LEN		63	//代理用户名的最大长度,不包括结尾0
#define DC7_MAX_PROXY_PASSWORD_LEN		63	//代理密码的最大长度,不包括结尾0

//视频算法
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

//音频算法
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

//图像格式
#define	DC7_PICALGID_DAMAGEJPEG		0  //有损JPEG
#define	DC7_PICALGID_NODAMAGEJPEG	1  //无损JPEG
#define	DC7_PICALGID_JPEG2000		2
#define	DC7_PICALGID_BMP			3
#define	DC7_PICALGID_PNG			4

//帧类型
#define DC7_FRAME_VIDEO			1	//视频帧
#define	DC7_FRAME_AUDIO			2	//音频帧
#define	DC7_FRAME_DATA			3	//数据帧
#define	DC7_FRAME_GPS			4	//GPS帧
#define	DC7_FRAME_SP			5	//串口帧
#define	DC7_FRAME_PIC			6	//图像帧

//视频帧信息
typedef struct _TDC7VideoFrameInfo
{
	unsigned short u16Width;		//视频帧分辨率(宽度): 288
	unsigned short u16Height;		//视频帧分辨率(高度): 352
	unsigned short u16ProducerID;	//厂商ID, 如: 创世的厂商ID是1
	unsigned char u8FrmRate;		//帧率
	unsigned char u8Reserved1;		//保留位
	unsigned int u32Reserved2;		//保留位
	unsigned int u32Reserved3;		//保留位
	//下面是创世私有格式中的
	unsigned char u8AlgID;			//算法标志, 具体见上面的宏定义
	unsigned char u8Rsv[3];			//保留位
	unsigned char u8PrivateData[8]; //私有数据, 没有的时候就全填0
} TDC7VideoFrameInfo;

//音频帧信息
typedef struct _TDC7AudioFrameInfo
{
	unsigned short u16BlockAlign;	//每个算法帧的包含的字节数
	unsigned char u8Channels;		//通道个数, 一般是1或2
	unsigned char u8BitsPerSample;	//PCM格式时的采样精度, 一般是8bit或16bit
	unsigned short u16SamplesPerSec;//PCM格式时的采样率除以100. 例如: 8K采样填80
	unsigned short u16AlgFrmNum;	//后面算法帧的个数, 这个值应该保持不变
	unsigned short u16ProducerID;	//厂商ID, 如:创世的厂商ID是1
	unsigned short u16PCMLen;		//一个算法帧解码后的PCM数据长度
	unsigned int u32Reserved;		//保留位
} TDC7AudioFrameInfo;

//GPS帧信息
typedef struct _TDC7GPSFrameInfo
{
	unsigned short u16FrmCnt;		//这一帧包含的GPS采样点的个数
	unsigned char u8Reserved[2];    //保留
} TDC7GPSFrameInfo;

//串口数据
typedef struct _TDC7SPFrameInfo
{
	unsigned short u16FrmCnt;		//这一帧包含的串口帧的个数
	unsigned char u8OfflineFlg;		//1为设备下线时存的数据, 0为实时数据
	unsigned char u8Reserved;		//保留
} TDC7SPFrameInfo;

//图像数据
typedef struct _TDC7PICFrameInfo
{
	unsigned short u16Width;		//图像宽度, 单位像素
	unsigned short u16Height;		//图像高度, 单位像素
	unsigned char u8StandardAlg;	//标准算法ID. 必须是标准压缩算法.
									//取值见前面DC7_PICALGID_DAMAGEJPEG等宏定义
	unsigned char u8Rsv[3];
} TDC7PICFrameInfo;

//帧信息
typedef struct _TDC7FrameInfo
{
	unsigned short u16FrameNO;		//帧序号
	unsigned char u8FrameType;		//帧类型, 见上面定义
	unsigned char u8Rsv;			//保留, 填0
	unsigned int u32UTCTime;		//UTC时间, 单位秒
	unsigned int u32Timestamp;		//时间戳, 单位毫秒
	unsigned char bIFrame;			//1表示关键帧, 0表示非关键帧
	unsigned char u8Rsv2[3];		//保留, 填0
	union
	{
		TDC7VideoFrameInfo video;	//视频帧信息
		TDC7AudioFrameInfo audio;	//音频帧信息
		TDC7GPSFrameInfo GPS;		//GPS帧信息
		TDC7SPFrameInfo SP;			//串口帧信息
		TDC7PICFrameInfo PIC;		//图片帧信息
	}Res;
} TDC7FrameInfo;

typedef void* DC7_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 初始化DNS查询模块
 *  参数说明:
 *  返回值: 0 表示成功, -1 表示失败
 *  备注: 如果要使用DNS的话, 必须调用这个函数.
 */
int DC7DLL_API DC7_DNS2IPInit();

/*  功能描述: 建立数据通道(包括协商过程)
 *			  通过代理建立数据通道(包括协商过程)
 *			  建立数据通道(包括协商过程)	
 *  参数说明:
 *		DC7_HANDLE *phDC7 [OUT]:		阻塞接口中,通过这个返回建立的DC7句柄
 *      const char *cpszAddr [IN]:		分发服务的地址,IP或者域名字符串,最大长度见前面的宏定义
 *										如果使用域名的话,必须调用DNS2IPInit函数,以下同
 *      unsigned short u16Port [IN]:	分发服务的端口
 *      const char *cpszToken [IN]:		连接分发服务的Token
 *      const char *cpszChID [IN]:		ChID,没有的时候填NULL
 *      const char *cpszChData [IN]:	ChData,没有的时候填NULL
 *		const char *cpszHost [IN]:		用户指定的Host域信息,格式ip:port或者dns:port,
 *										传入NULL或者空字符串表示使用连接的目标地址作为Host信息域.
 *      const char *pszProxyType [IN]:	代理类型,取值和含义如下:
 *											"HTTP"		基本认证的HTTP代理
 *											"SOCKET4"	SOCKS4代理
 *											"SOCKET5"	SOCKS5代理
 *											"NTLM"		NTLM认证的HTTP代理
 *      const char *pszProxyAddr [IN]:	代理地址,IP或者域名字符串,最大长度见前面的宏定义
 *		unsigned short usProxyPort [IN]:代理的端口
 *      const char *pszProxyUsername [IN]:	连接代理的用户名,最大长度见前面的宏定义
 *      const char *pszProxyPassword [IN]:	连接代理的密码,最大长度见前面的宏定义
 *  返回值: 非阻塞成功返回DC7_HANDLE句柄, 失败返回NULL
 *			阻塞接口返回DC7_E_OK,表示建立成功,其他表示错误码
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

/*  功能描述: 探测数据通道是否连接成功
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *  返回值:		DC7_E_OK			协商成功返回
 *				DC7_E_WOULDBLOCK	表示还要继续探测
 *				其他错误码均为小于0的, 需要关闭通道.
 */
int DC7DLL_API DC7_GetConnectStatus(DC7_HANDLE hDC7);

/*  功能描述: 建立数据通道(不需要协商,用于前端设备的处理直连的数据通道)
 *  参数说明:
 *      int sock [IN]:					协商过后的sock.
 *  返回值: 成功返回DC7_HANDLE句柄, 失败返回NULL.
 *  备注: 这个函数会把sock设置成非阻塞的,
 *		  调用过这个之后,不需要调用DC7_GetConnectStatus获取连接状态.
 */
DC7_HANDLE DC7DLL_API DC7_CreateWithoutChat(int sock);

/*  功能描述: 设置发送缓冲的大小
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      int iSize [IN]:					发送缓冲大小,单位字节
 *  备注: 必须在连接成功之后才能够调用这个函数
 */
void DC7DLL_API DC7_SetSendBufferSize(DC7_HANDLE hDC7, int iSize);

/*  功能描述: 设置接收缓冲的大小
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      int iSize [IN]:					接收缓冲大小,单位字节
 *  备注: 必须在连接成功之后才能够调用这个函数
 */
void DC7DLL_API DC7_SetRecvBufferSize(DC7_HANDLE hDC7, int iSize);

/*  功能描述: DC7调度模块.
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *  返回值:		DC7_E_OK			调度成功
 *				其他的为小于0的错误码都需要要关闭通道.
 */
int DC7DLL_API DC7_Run(DC7_HANDLE hDC7);

/*  功能描述: 从数据通道中读取一帧数据.
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      char *binFrameBuf [OUT]:		数据帧的存放地址, 需要调用者开辟.
 *      unsigned int *pu32BufLen [IN, OUT]: 传入时为缓冲的最大长度,最大长度可为DC7_FRAME_MAXLEN,
											传出时为数据帧的实际长度.
 *      TDC7FrameInfo *pFrameInfo [OUT]: 数据帧的相关信息.
 *  返回值:		DC7_E_OK			接收数据帧成功
 *				DC7_E_WOULDBLOCK	读取数据帧没成功, 需要继续接收 
 *				DC7_E_FRAMEBUF_LEN	表示传入缓冲长度不够,此时pu32BufLen为接收到的帧长度,上层应该修改缓冲的长度
 *				DC7_E_END			表示完成接收,表示接收到了一个空包.(根据约定, 可以断开或继续通道)
 *				其他的小于0的错误码要关闭通道.
 *  备注: 1、当接收到的是音频帧时, binFrameBuf中的数据组成如下:
 *			 TDC7FrameInfo.aduio.u16AlgFrmNum个TDC7FrameInfo.aduio.u16BlockAlign长度的算法帧
 *		     每个算法帧的格式具体参考<NRCAP2.0数据传输的格式_创世数据格式.doc>文档
 *		  2、支持空帧的接收, 接收成功返回DC7_E_OK, 数据长度为0, 帧信息有效.
 */
int DC7DLL_API DC7_RecvFrame(DC7_HANDLE hDC7, char *binFrameBuf, unsigned int *pu32BufLen, TDC7FrameInfo *pFrameInfo);

/*  功能描述: 从数据通道中读取一帧裸数据帧(相对上一个函数,这个函数不提取帧信息)
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      char *binFrameBuf [OUT]:		数据帧的存放地址, 需要调用者开辟.
 *      unsigned int *pu32BufLen [IN, OUT]: 传入时为缓冲的最大长度,最大长度可为DC7_FRAME_MAXLEN+sizeof(TDC7FrameInfo)
											传出时为数据帧的实际长度.
 *      int *piFrameType [OUT]:			帧类型,取值见前面宏定义	
 *      int *pbKeyFrame [OUT]:			是否是关键帧,1表示是,0表示不是
 *  返回值:	同DC7_RecvFrame函数.
 *	备注:	支持接收空帧, 此时只传出帧信息和帧信息长度.
 */
int DC7DLL_API DC7_RecvRawFrame(DC7_HANDLE hDC7, char *binFrameBuf, unsigned int *pu32BufLen, int *piFrameType, int *pbKeyFrame);

/*  功能描述: 将一帧数据写到数据通道中.
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      char *binSendFrame [IN]:		需要发送的数据帧的缓冲.
 *      unsigned int u32FrameLen [IN]:	需要发送数据帧的长度,不能够大于DC7_FRAME_MAXLEN
 *      TDC7FrameInfo *pFrameinfo [IN]:	需要发送数据帧的帧信息.
 *  返回值:		DC7_E_OK			发送数据帧成功, 
 *				DC7_E_WOULDBLOCK	发送数据帧没成功, 
 *				其他的小于0的错误码要关闭通道.
 *  备注: 1、当发送的是音频帧时,binFrameBuf中的数据组成同DC7_RecvFrame函数.
 *		  2、支持空帧的发送, 此时需要传入的数据长度为0和有效帧信息.
 */
int DC7DLL_API DC7_SendFrame(DC7_HANDLE hDC7, char *binSendFrame, unsigned int u32FrameLen, TDC7FrameInfo *pFrameinfo);

/*  功能描述: 发送一个空的数据包
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]: 有效句柄
 *  返回值: int 同DC7_SendFrame;
 */
int DC7DLL_API DC7_SendEmptyPacket(DC7_HANDLE hDC7);

/*  功能描述: 将一帧裸数据写到数据通道中(相对上一个函数,这个函数穿入的不是帧信息).
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      char *binSendFrame [IN]:		需要发送的数据帧的缓冲.
 *      unsigned int u32FrameLen [IN]:	需要发送数据帧的长度,不能够大于DC7_FRAME_MAXLEN
 *      int iFrameType [IN]:			帧类型,取值见前面的宏定义.
 *		unsigned short u16FrameNO [IN]:	帧序号,用来判断转发过程中是否丢帧.
 *  返回值:		DC7_E_OK			发送数据帧成功, 
 *				DC7_E_WOULDBLOCK	发送数据帧没成功, 
 *				其他的小于0的错误码要关闭通道.
 */
int DC7DLL_API DC7_SendRawFrame(DC7_HANDLE hDC7, char *binSendFrame, unsigned int u32FrameLen, int iFrameType, unsigned short u16FrameNO);

/*  功能描述: 从通知链表中获得一个通知.
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *      char *pbinNotifyBuf [OUT]:		通知的存放地址, 需要调用者开辟.
 *      unsigned int *pu32BufLen [IN, OUT]: 传入时为缓冲的最大长度, 传出时为通知数据的实际长度.
 *  返回值:		DC7_E_OK			表示接收通知成功
 *				DC7_E_WOULDBLOCK	表示没有接收到通知
 * 				DC7_E_FRAMEBUF_LEN	表示传入缓冲长度不够,此时pu32BufLen为接收到的帧长度,上层应该修改缓冲的长度
 *				其他的小于0的错误码要关闭通道.
 */
//int DC7DLL_API DC7_RecvNotify(DC7_HANDLE hDC7, char *binNotifyBuf, unsigned int *pu32BufLen);

/*  功能描述: 关闭DC7模块
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:			有效DC7_HANDLE句柄.
 *  备注: 所有Create成功的数据通道必须调用此接口关闭, 否则会内存泄露	
 */
void DC7DLL_API DC7_Close(DC7_HANDLE hDC7);

/*  功能描述: 获取数据通道信息.
 *  参数说明:
 *      DC7_HANDLE hDC7 [IN]:		有效DC7_HANDLE句柄.
 *      unsigned int *puiBirthTime [OUT]:	通道创建UTC时间.
 *      unsigned int *puiPeerIP [OUT]:	对方的IP
 *      unsigned short *puiPeerPort [OUT]:	对方的端口
 *      unsigned int *puiTotalMBytes [OUT]: 收发字节数的MB级部分.
 *      unsigned int *puiTotalKBytes [OUT]: 收发字节数的KB级部分.
 *  返回值: DC7DLL_API int 操作成功后返回DC7_E_OK, 否则返回其他.
 */
int DC7DLL_API DC7_GetChnInfo(DC7_HANDLE hDC7, unsigned int *puiBirthTime, 
							  unsigned int *puiPeerIP, unsigned short *puiPeerPort, 
							  unsigned int *puiTotalMBytes, unsigned int *puiTotalKBytes);

#ifdef __cplusplus
}
#endif

#endif //__DC7_H__
