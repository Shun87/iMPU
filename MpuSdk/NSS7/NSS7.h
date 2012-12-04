/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: NSS7.h
 *  文件标识: 
 *  摘    要: NSS7 = NRCAP Server Session 7.0
 *  
 *  当前版本: 
 *  作    者: 项敏敏
 *  完成日期: 2010-4-8 17:05:51
 *  修正备注: 
 *  
 */

#ifndef __NSS7_H__
#define __NSS7_H__

//库的版本
#define NCS7_VERSION		"7.2011.11.14"

//返回的错误码,小于NSS7_E_OK的要关闭通道
#define NSS7_E_WOULDBLOCK		 1		//操作正在进行中
#define NSS7_E_OK				 0		//成功

//未知错误码
#define NSS7_E_UNKNOW			-1		//未知错误
#define NSS7_E_PARAM			-2		//接口函数中传入的参数无效
#define NSS7_E_PARAMLEN			-3		//在向平台注册的过程中,接收到的通用参数的长度错误
#define NSS7_E_REDIRECT			-4		//平台返回的重定向失败,属性ErrorCode的值不为0或者标签的值不为"OK"
#define NSS7_E_POSTLEN			-5		//在注册的过程中,接收到POST的长度溢出, 或者接收POST缓冲溢出(接收缓冲长度为1024字节)
#define NSS7_E_POST				-6		//接收到的POST错误,不符合协议的要求,可能是缺胳膊少腿的
#define NSS7_E_MALLOC			-7		//内存开辟错误
#define NSS7_E_TCPRECV			-8		//TCP接收出错
#define NSS7_E_TCPSEND			-9		//TCP发送出错
#define NSS7_E_TIMEOUT			-10		//接收数据超时
//收发命令时关键错误
#define NSS7_E_SYNCFLAG			-51		//接收命令时起始字符
#define NSS7_E_VERSION			-52		//接收命令时版本错误
#define NSS7_E_CTRLFLAG			-53		//接收命令时控制字段错误
#define NSS7_E_MSGTYPE			-54		//接收命令时消息类型错误
#define NSS7_E_PACKETLEN		-55		//接收命令时包长度错误,长度溢出或需要加密的没有8字节对齐
#define NSS7_E_PACKETTYPE		-56		//接收命令时包类型错误
#define NSS7_E_REGISTERVER		-57		//注册协议版本错误
//平台地址不可达错误码
#define NSS7_E_NETBASE			-100	//网络出错错误码起始值
#define NSS7_E_NOROUTE			-101	//没路由
#define NSS7_E_NOSERVER			-102	//平台服务为开启(端口出错)
#define NSS7_E_UNREACHABLE		-103	//平台地址不可达
#define NSS7_E_PROXYCONNECT		-104	//代理连接失败
#define NSS7_E_NETBASEEND		-149	//网络出错错误码结束值
//平台返回的错误码
#define NSS7_E_PLATFORMBASE		-150	//平台返回基值开始(即: 此值 - 下面的错误码 = 平台返回的错误码)
#define NSS7_E_PASSWORD			-151	//平台返回密码错误
#define NSS7_E_PUIDNOTEXIST		-152	//平台返回PUID不存在
#define NSS7_E_PUIDNOTDISABLE	-153	//平台返回PUID被禁用
#define NSS7_E_INVALIDPOST		-154	//平台返回POST报文格式出错
#define NSS7_E_PUFULL			-155	//平台返回PU接入数已满
#define NSS7_E_PRODUCERID		-156	//平台返回非法的厂商ID
#define NSS7_E_ALREADYONLINE	-157	//平台返回PUID已被使用
#define NSS7_E_DEVIDONLINE		-158	//平台返回DevID已在线
#define NSS7_E_PRODUCERPSW		-159	//平台返回厂商密码错误
#define NSS7_E_PLATFORMBASEEND	-199	//平台返回基值结束(返回的错误码)

//最大时间长度定义
#define NSS7_KEEPALIVEINTERVAL	15		//保活包时间间隔,单位秒
#define NSS7_CONNECTTIMEOUT		120		//连接超时时间长度,单位秒
//链表最大长度定义
#define NSS7_MAXLEN_PACKETLIST	(16)	//发送链表中命令包的个数
										//大于此值时,发送函数会返回NSS7_E_WOULDBLOCK
//传入参数长度说明
#define NSS7_MAXLEN_HASHPSW			16
#define NSS7_MAXLEN_PUIADDR			64	//PUI地址
#define NSS7_MAXLEN_PROXYTYPE		16
#define NSS7_MAXLEN_PROXYADDR		64
#define NSS7_MAXLEN_PROXYUNAME		64
#define NSS7_MAXLEN_PROXYPSW		64
#define NSS7_MAXLEN_CONTENT			2048

//用户信息
#define NSS7_MAXLEN_USERID			64
#define NSS7_MAXLEN_CLIENTTYPE		32

//消息类型定义
#define NSS7_MSGTYPE_REQUEST					1	//请求事件
#define NSS7_MSGTYPE_RESPONSE					2	//响应事件
#define NSS7_MSGTYPE_NOTIFY						3	//通知事件
#define NSS7_MSGTYPE_REPORT						4	//配置上报事件
#define NSS7_MSGTYPE_INTELL						5	//智能分析事件
#define NSS7_MSGTYPE_EXTENDAPPNOTIFY			6	//扩展应用通知事件

//设备与平台通信句柄,设备与客户端的通信在完成协商后,也会转到此句柄中来
typedef void* NSS7_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 初始化XOS模块.
 *  参数说明:
 *  返回值: 0 表示成功, -1 表示失败
 *  备注: 用到的多个模块中都有XXX_XOSStartup函数的时候,只调用一个就可以了
 */
int NSS7_XOSStartup();

/*  功能描述: 初始化DNS查询模块
 *  参数说明:
 *  返回值: 0 表示成功, -1 表示失败
 *  备注: 用到的多个模块中都有XXX_DNS2IPInit函数的时候,只调用一个就可以了
 */
int NSS7_DNS2IPInit();

/*  功能描述: 创建NSS7句柄,用来与平台通信
 *  参数说明:
 *      char *pszPUIAddr [IN]:				PUI地址,IP或域名.
 *      unsigned short usPUIPort [IN]:		PUI端口
 *      unsigned char binPswHash[16] [IN]:	登录密码的哈希值
 *      char *pszPUID [IN]:					设备的PUID也就是服务号,由运营商分配
 *      unsigned int uiIP [IN]:				设备网口的IP,可填0
 *      unsigned int uiSubnetMask [IN]:		子网掩码,设备网口的子网掩码,可填0
 *      unsigned int uiDialIP [IN]:			拨号IP,设备拨号获取的IP,比如3G无线拨号或者PPPoE拨号,可填0
 *      char *pszProducerID [IN]:			设备生产厂商代码,由创世分配.
 *		unsigned char binProducerPsw[64] [IN]:	厂商接入密码,由创世分配.
 *      int iKeyResNum[IN]:					关键资源数目,比如视频服务器设备一般填视频数,具体是什么资源的数量
 *											由创世在分配厂商接入ID的时候预定好,平台将通过这个数值统计这一
 *											厂商使用的资源数,如果小于后面上报的实际资源数,那么在平台中能够看
 *											到的实际资源数将以这个为准.
 *      char *pszDevID [IN]:				设备唯一标识,在平台中必须唯一,建议使用厂商代码加上MAC地址构成,
 *											例如:"00001-112233445566".
 *      char *pszDevType [IN]:				设备类型,取值和含义如下:
 *												ENC		有线编码器
 *												DEC		有线解码器
 *												WENC	无线编码器
 *												WDEC	无线解码器
 *												ALARM	报警主机
 *												GPS		GPS设备
 *      char *pszDevModel [IN]:				设备型号,用以区分相同厂商的不同型号的设备.
 *      char *pszHardwareVer [IN]:			硬件版本号,用以区分相同型号设备的不同硬件版本.
 *      char *pszSoftwareVer [IN]:			软件版本号,用以区分相同型号设备的不同软件版本
 *      char *pszExtendPost [IN]:			向平台注册时的扩者Post报文,格式为:Item=value&..., 空填:"".
 *      int bProxy [IN]:					是否通过代理登录平台, 1:是, 0:否
 *      char *pszProxyType [IN]:			代理类型, 空填:""..
 *      char *pszProxyAddr [IN]:			代理服务器地址, IP或域名, 空填:"".
 *      unsigned short usProxyPort [IN]:	代理服务器端口, 空填:0.
 *      char *pszProxyUserName [IN]:		代理用户名, 空填:"".
 *      char *pszProxyPassword [IN]:		代理用密码, 空填:"".
 *      int bFixPUIAddr [IN]:				是否固定PUI的地址1:是, 0:否. (平台做了端口映射且支持重定向时需要)
 *      int bDebug [IN]:					是否是调试版本, 1是, 0:否
 *  返回值: 成功返回NSS7句柄,否则NULL.
 */
NSS7_HANDLE NSS7_Create(char *pszPUIAddr, unsigned short usPUIPort, unsigned char binPswHash[16],
						char *pszPUID, unsigned int uiIP, unsigned int uiSubnetMask, unsigned int uiDialIP,
						char *pszProducerID, unsigned char binProducerPsw[64], int iKeyResNum,
						char *pszDevID, char *pszDevType, char *pszDevModel,
						char *pszHardwareVer, char *pszSoftwareVer, char *pszExtendPost,
						int bProxy, char *pszProxyType, char *pszProxyAddr, unsigned short usProxyPort,
						char *pszProxyUserName, char *pszProxyPassword,
						int bFixPUIAddr, int bDebug);

/*  功能描述: 获取连接是否成功
 *  参数说明:
 *      int sock [IN]:					NSS7模块句柄
 *      unsigned char ucCtlFlag [IN]:	加密算法标识,1表示DES加密,0表示不加密
 *      char binDesKey[8] [IN]:			DES加密的密钥
 *      unsigned int uiKeepAliveInterval [IN]:	发送保活包的间隔,单位毫秒
 *      unsigned int uiSessionTimeOut [IN]:		连接超时时间,单位毫秒
 *      char *pszUserID [IN]:	连接进来的用户ID.
 *      char *pszClientType [IN]:	连接进来的客户类型.
 *  返回值: 成功返回NSS7句柄,否则NULL.
 */
NSS7_HANDLE NSS7_CreateWithoutChat(int sock, unsigned char ucCtlFlag, char binDesKey[8],
								   unsigned int uiKeepAliveInterval, unsigned int uiSessionTimeOut,
								   char *pszUserID, char *pszClientType);

/*  功能描述: 获取连接是否成功
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			连接成功
 *				NSS7_E_WOULDBLOCK	需要继续探测
 *				小于NSS7_E_OK			连接出错的错误码
 */
int NSS7_GetConnectStatus(NSS7_HANDLE hNSS7);

/*  功能描述: 第一个发送报文是否发送完毕
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *  返回值: 1表示发送成功,0表示没有发送成功
 *  备注: 调用这个函数的时候,不要忘记调用NSS7_Run,内部没线程调度
 */
int NSS7_IsFirstPacketSendOK(NSS7_HANDLE hNSS7);

/*  功能描述: 内部调度
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			调度成功
 *				小于NSS7_E_OK		连接出错的错误码
 *  备注: 要定时调用此接口,才能实现数据的发送接收.
 */
int NSS7_Run(NSS7_HANDLE hNSS7);

/*  功能描述: 接收请求命令
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *      char **ppMsgBody [OUT]:			存放请求消息体地址
 *      int *piMsgBodyLen [OUT]:		请求消息体的长度
 *      unsigned int *puiTransID [OUT]:	请求的事务号
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			接收成功
 *				NSS7_E_WOULDBLOCK	暂时没有请求
 *				小于NSS7_E_OK		连接出错的错误码
 *  备注: 当成功获得请求信息后要调用NSS7_Free释放内存,不然会有内存泄露
 */
int NSS7_RecvRequest(NSS7_HANDLE hNSS7, char **ppMsgBody, int *piMsgBodyLen, unsigned int *puiTransID);

/*  功能描述: 发送命令响应
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *      char *pMsgBody [IN]:			响应消息体缓冲
 *      int iMsgBodyLen [IN]:			响应消息体的长度
 *      unsigned int uiTransID [IN]:	响应的事物号
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			发送成功
 *				NSS7_E_WOULDBLOCK	缓冲已满,暂时不能发送
 *				小于NSS7_E_OK		连接出错的错误码
 */
int NSS7_SendResponse(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen, unsigned int uiTransID);

/*  功能描述: 发送通知
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *      char *pMsgBody [IN]:			通知消息体缓冲
 *      int iMsgBodyLen [IN]:			通知消息体的长度
 *      int iType [IN]:					消息类型, 见上面"消息类型定义"
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			发送成功
 *				NSS7_E_WOULDBLOCK	缓冲已满,暂时不能发送
 *				小于NSS7_E_OK		连接出错的错误码
 */
int NSS7_SendNotify(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen);
int NSS7_SendNotifyEx(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen, int iType);

/*  功能描述: 发送配置上报
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *      char *pMsgBody [IN]:			上报消息体缓冲
 *      int iMsgBodyLen [IN]:			上报消息体的长度
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			发送成功
 *				NSS7_E_WOULDBLOCK	缓冲已满,暂时不能发送
 *				小于NSS7_E_OK		连接出错的错误码
 *  备注: 只有与平台的连接才可以发送上报,对于客户端的连接此接口无效
 */
int NSS7_SendReport(NSS7_HANDLE hNSS7, char *pMsgBody, int iMsgBodyLen);

/*  功能描述: 释放消息缓冲
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *      void *pMsgBody [IN]:			要是否的消息体缓冲
 */
void NSS7_Free(NSS7_HANDLE hNSS7, void *pMsgBody);

/*  功能描述: 释放NSS7模块
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *  备注: 当NSS7句柄出错或不需要的时候需要调用此接口关闭句柄
 */
void NSS7_Close(NSS7_HANDLE hNSS7);

/*  功能描述: 获取连接信息
 *  参数说明:
 *      NSS7_HANDLE hNSS7 [IN]:			NSS7模块句柄
 *      unsigned int *puiLoginTime [OUT]:	连接建立时间,UTC时间
 *      unsigned int *puiPeerIP [OUT]:		对方IP地址
 *		unsigned short *pusPeerPort [OUT]:	对方端口
 *		char szUerID[NSS7_MAXLEN_USERID] [OUT]:		连接用户名
 *		char szClientType[NSS7_MAXLEN_CLIENTTYPE] [OUT]:	连接密码
 *  返回值: 错误码,取值如下
 *				NSS7_E_OK			发送成功
 *				小于NSS7_E_OK		连接出错的错误码
 */
int NSS7_GetSessionInfo(NSS7_HANDLE hNSS7, unsigned int *puiLoginTime,
						unsigned int *puiPeerIP, unsigned short *pusPeerPort,
						char szUerID[NSS7_MAXLEN_USERID], char szClientType[NSS7_MAXLEN_CLIENTTYPE]);

#ifdef __cplusplus
}
#endif

#endif //__NSS7_H__
