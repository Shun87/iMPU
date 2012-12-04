/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSStack.h
 *  文件标识: 
 *  摘    要: 协议栈相关定义和函数
 *				
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-14 15:35:24
 *  修正备注: 
 *  
 */

#ifndef __XOSSTACK_H__
#define __XOSSTACK_H__

//////////////////////////////////////////////////////////////////////////
//	!!!!!!所有用到的网络相关的类型,宏,函数都应该使用如下列举的,不允许例外
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	头文件和宏
//////////////////////////////////////////////////////////////////////////

//字节序转换
#define xos_ntohu16(u)	((((u) << 8) & 0xFF00) | ((u) >> 8))
#define xos_ntohu32(u)	(((u) << 24) | (((u) << 8) & 0x00FF0000) | \
						 (((u) >> 8) & 0x0000FF00)	| ((u) >> 24))
#define xos_htonu16(u)	xos_ntohu16(u)
#define xos_htonu32(u)	xos_ntohu32(u)

#define XOS_SOCKETERR_NOROUTE		-2		//没有路由
#define XOS_SOCKETERR_UNREACHABLE	-3		//连接地址不可达
#define XOS_SOCKETERR_NOSERVER		-4		//端口服务未开启

/*
	这些宏的定义在下面include的"XOSStack_Defs_XXX.h"的头文件中

	XOS_SOCKET					socket句柄类型
	XOS_INVALID_SOCKET			非法socket句柄

	xos_setsockblock(s)			将socket设置成阻塞模式,s为socket句柄,以下同
	xos_setsocknonblock(s)		将socket设置成非阻塞模式
	xos_setsockreuseaddr(s)		将socket设置成端口可重用
	xos_setsockbroadcast(s)		将socket设置成能够收发广播包
	xos_setsockaddmulticast(s, i) 将socket加入指定的组播组,i为32位组播地址
	xos_setsockdropmulticast(s, i) 将socket从指定的组播组中删除,i同上
	xos_setsockrecvbuf(s, i)	设置socket接收缓冲大小,i为缓冲大小
	xos_setsocksendbuf(s, i)	设置socket发送缓冲大小,i同上
	xos_setsocklinger(s, o, l)	设置socket等待时间,o表示是否使能等待,l表示等待时间,单位秒
								似乎NDK要设置,WIN32不要设置
	xos_setsocknodelay(s)		设置socket为非延迟模式,具体含义不是太明白
								根据平台不同而不同,一般不设这个
	xos_setsockkeepalive(s)		设置socket为保持连接,不是所有平台都支持,一般不设这个
*/

#ifdef WIN32
	#include "XOSStack_Defs_WIN32.h"
#elif _CREARO_NDK
	#include "XOSStack_Defs_CREARO_NDK.h"
#else //LINUX
	#include "XOSStack_Defs_LINUX.h"
#endif //WIN32

//////////////////////////////////////////////////////////////////////////
//	函数定义
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"  {
#endif

#define XOS_MAX_IPSTR_LEN	16	//IP字符串最大长度,包括结尾0
/*  功能描述: 整数IP地址转字符串 
 *  参数说明:
 *      XOS_U32 u32IP [IN]:	整数IP地址
 *  返回值: 字符串IP地址
 */
XOS_PSZ XOS_U32ToAddr(XOS_U32 u32IP, char szIP[XOS_MAX_IPSTR_LEN]);

/*  功能描述: 字符串IP地址转整数
 *  参数说明:
 *      XOS_CPSZ cpszAddr [IN]:	字符串IP地址
 *  返回值: 整数IP地址
 */
XOS_U32 XOS_AddrToU32(XOS_CPSZ cpszAddr);

/*  功能描述: 关闭socket
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *	备注:
 *		如下函数会产生一个socket
 *		XOS_TCPConnect
 *		XOS_TCPConnectNB
 *		XOS_TCPListen
 *		XOS_TCPAccept
 *		XOS_TCPAcceptNB
 *		XOS_UDPBind
 */
void XOS_CloseSocket(XOS_SOCKET sock);

//////////////////////////////////////////////////////////////////////////
//	TCP客户端
//////////////////////////////////////////////////////////////////////////

/*  功能描述: 创建TCP socket并以阻塞方式连接
 *  参数说明:
 *      XOS_U32 u32IP [IN]:		要连接的IP地址
 *      XOS_U16 u16Port [IN]:	要连接的端口
 *		int iTimeOut [IN]:		超时时间,单位秒,0表示永远等待
 *  返回值: 返回XOS_INVALID_SOCKET表示失败,其他表示成功,返回值为socket句柄
 *	备注: 返回的SOCKET缺省为阻塞模式
 */
XOS_SOCKET XOS_TCPConnect(XOS_U32 u32IP, XOS_U16 u16Port, int iTimeOut);

/*  功能描述: 创建TCP socket并以非阻塞方式连接
 *  参数说明:
 *      XOS_U32 u32IP [IN]:		要连接的IP地址
 *      XOS_U16 u16Port [IN]:	要连接的端口
 *  返回值: 返回XOS_INVALID_SOCKET表示失败,其他表示成功,返回值为socket句柄
 *	备注: 此函数返回成功之后,需要用XOS_TCP_ConnectProbe探测连接是否成功
 *		  返回的SOCKET缺省为非阻塞模式
 */
XOS_SOCKET XOS_TCPConnectNB(XOS_U32 u32IP, XOS_U16 u16Port);

/*  功能描述: 探测非阻塞TCP socket连接是否成功(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *  返回值: 返回-1表示出错,0表示成功,1表示还需继续探测
 */
int XOS_TCPConnectProbe(XOS_SOCKET sock);

/*  功能描述: 非阻塞方式发送TCP数据,发多少算多少(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:		socket句柄
 *      char *pSendBuf [IN]:		发送缓冲
 *      int iSendLen [IN]:			需发送数据的长度
 *  返回值: 返回-1表示出错,大于等于0表示发送的字节数
 */
int XOS_TCPSendNB(XOS_SOCKET sock, char *pSendBuf, int iSendLen);

/*  功能描述: 阻塞方式发送TCP数据,直至发送完指定长度的数据(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pSendBuf [IN]:	发送缓冲指针
 *      int iSendLen [IN]:		发送数据的长度
 *		int iTimeOut [IN]:		超时时间,单位秒,0表示永远等待
 */
XOS_BOOL XOS_TCPSendData(XOS_SOCKET sock, char *pSendBuf, int iSendLen, int iTimeOut);

/*  功能描述: 非阻塞方式发送TCP数据,直至发送完指定长度的数据(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:		socket句柄
 *      char *pSendBuf [IN]:		发送缓冲
 *      int *piSendLen [IN, OUT]:	已经发送的数据的长度
 *									上层赋*piSendLen初值为0,不要使用这个变量.
 *									用于此函数内部判断发送的位置
 *		int iDataLen [IN]:			指定要发送的数据的长度
 *  返回值: 返回-1表示出错,0表示发送成功,1表示啥都没干,还要继续发送,
 *			2表示发送了哪怕一个字节,但是还未发完,还需要继续发送
 */
int XOS_TCPSendDataNB(XOS_SOCKET sock, char *pSendBuf, int *piSendLen, int iDataLen);
int XOS_TCPConnectProbeWithErrNo(XOS_SOCKET sock);

/*  功能描述: 非阻塞方式发送单个字符(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char c [IN]:			要发送的字节
 *  返回值: 返回-1表示出错,0表示发送成功,1表示啥都没干,还需继续发送
 */
int XOS_TCPSendCharNB(XOS_SOCKET sock, char c);

/*  功能描述: 阻塞方式接收TCP数据,直到收到数据(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pRecvBuf [IN]:	接收缓冲地址	
 *      int iMaxRecvLen [IN]:	接收缓冲的最大长度
 *		int iTimeOut [IN]:		超时时间,单位秒,0表示永远等待
 *  返回值: -1表示出错,大于0表示接收到的字节数
 */
int XOS_TCPRecv(XOS_SOCKET sock, char *pRecvBuf, int iMaxRecvLen, int iTimeOut);

/*  功能描述: 非阻塞方式接收TCP数据(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pRecvBuf [IN]:	接收缓冲地址	
 *      int iMaxRecvLen [IN]:	接收缓冲的最大长度
 *  返回值: -1表示出错,0表示啥都没有收到,大于0表示接收到的字节数
 */
int XOS_TCPRecvNB(XOS_SOCKET sock, char *pRecvBuf, int iMaxRecvLen);

/*  功能描述: 阻塞方式接收数据,直到接收满指定字节数(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pRecvBuf [IN]:	接收缓冲地址	
 *      int iRecvLen [IN]:		接收数据的长度
 *		int iTimeOut [IN]:		超时时间,单位秒,0表示永远等待
 */
XOS_BOOL XOS_TCPRecvData(XOS_SOCKET sock, char *pRecvBuf, int iRecvLen, int iTimeOut);

/*  功能描述: 非阻塞方式接收数据,直到接收满指定字节数(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pRecvBuf [IN]:	接收缓冲地址
 *      int *piRecvLen [IN]:	已经接收的长度的地址,
 *								上层赋*piRecvLen初值为0,不要使用这个变量
 *								用于此函数内部判断接收的位置
 *		int iDataLen [IN]		指定的接收字节数
 *  返回值: 返回-1表示出错,0表示接收完毕,1表示啥都没干,还需继续接收
 *			2表示接收了哪怕一个字节,但是还是需要继续接收	
 */
int XOS_TCPRecvDataNB(XOS_SOCKET sock, char *pRecvBuf, int *piRecvLen, int iDataLen);

/*  功能描述: 阻塞方式接收单个字符(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pChar [OUT]:		*pChar为接收到的字符
 *		int iTimeOut [IN]:		超时时间,单位秒,0表示永远等待
 */
XOS_BOOL XOS_TCPRecvChar(XOS_SOCKET sock, char *pChar, int iTimeOut);

/*  功能描述: 非阻塞方式接收单个字符(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pChar [OUT]:		*pChar为接收到的字符
 *  返回值: 返回-1表示出错,0表示接收完毕,1表示啥都没干,还需继续接收
 */
int XOS_TCPRecvCharNB(XOS_SOCKET sock, char *pChar);

/*  功能描述: 阻塞方式接收数据头,直到收到指定串为止(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pBuf [IN]:		为接收缓冲指针,接收缓冲必须足够大,否则会溢出
 *      int iBufLen [IN]:		接收缓冲大小
 *      XOS_PSZ pszTail [IN]:	指定的结束串
 *		int iTimeOut [IN]:		超时时间,单位秒,0表示永远等待
 *  返回值: 返回0表示接收成功,-1表示出错,-2表示缓冲溢出
 */
int XOS_TCPRecvHead(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_PSZ pszTail, int iTimeOut);

/*  功能描述: 非阻塞方式接收数据头,直到收到指定串为止(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	socket句柄
 *      char *pBuf [IN]:		接收缓冲指针,接收缓冲必须足够大,否则会溢出
 *      int iBufLen [IN]:		接收缓冲长度
 *      int *piRecvLen [IN]:	已经接收的长度的地址,
 *								上层赋*piRecvLen初值为0,不要使用这个变量
 *								用于此函数内部判断接收的位置
 *      XOS_PSZ pszTail [IN]:	指定的结束串
 *  返回值: 返回0表示接收成功,-1表示出错,-2表示缓冲溢出,1表示啥都没干,需要继续接收,
 *			2表示接收了哪怕一个字节,但是还是需要继续接收
 */			
int XOS_TCPRecvHeadNB(XOS_SOCKET sock, char *pBuf, int iBufLen, int *piRecvLen, XOS_PSZ pszTail);

//////////////////////////////////////////////////////////////////////////
//	TCP服务器
//////////////////////////////////////////////////////////////////////////

/*  功能描述: 创建TCP服务器,并且帧听
 *  参数说明:
 *      XOS_U16 u16Port [IN]:	帧听端口
 *      int iListenNum [IN]:	最大可接受的连接数
 *  返回值: 返回XOS_INVALID_SOCKET表示失败,其他表示成功,返回值为socket句柄
 *	备注: 返回的SOCKET缺省为阻塞模式
 */
XOS_SOCKET XOS_TCPListen(XOS_U16 u16Port, int iListenNum);

/*  功能描述: 阻塞方式接受来自TCP客户端的连接(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	TCP服务器socket
 *  返回值: 返回XOS_INVALID_SOCKET表示失败,其他表示成功,返回值为socket句柄
 *	备注: 返回的SOCKET缺省为阻塞模式
 */
XOS_SOCKET XOS_TCPAccept(XOS_SOCKET sock);

/*  功能描述: 非阻塞方式接受来自TCP客户端的连接(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	TCP服务器socket
 *  返回值: 返回XOS_INVALID_SOCKET表示失败或者无连接接入,其他表示成功,返回值为socket句柄
 *	备注: 返回的SOCKET缺省为非阻塞模式
 */
XOS_SOCKET XOS_TCPAcceptNB(XOS_SOCKET sock);
    
/*  π¶ƒ‹√Ë ˆ: ªÒ»°∂‘∑Ω÷˜ª˙µƒµÿ÷∑.
 *  ≤Œ ˝Àµ√˜:
 *      XOS_SOCKET sock [IN]:	socketæ‰±˙
 *      XOS_U32 *pu32IP [OUT]:	∂‘∑ΩµƒIP,¥´»ÎNULL±Ì æ≤ª¥¯≥ˆ¥À≤Œ ˝.
 *      XOS_U16 *pu16Port [OUT]:	∂‘∑Ω¡¨Ω”µƒPort,¥´»ÎNULL±Ì æ≤ª¥¯≥ˆ¥À≤Œ ˝.
 */
XOS_BOOL XOS_TCPGetPeerAddr(XOS_SOCKET sock, XOS_U32 *pu32IP, XOS_U16 *pu16Port);

/*  π¶ƒ‹√Ë ˆ: ªÒ»°¡¨Ω”µƒ±æµÿµÿ÷∑(ƒø«∞ΩˆWIN32÷ß≥÷)
 *  ≤Œ ˝Àµ√˜:
 *      XOS_SOCKET sock [IN]:	socketæ‰±˙
 *      XOS_U32 *pu32IP [OUT]:	±æµÿµƒIP,¥´»ÎNULL±Ì æ≤ª¥¯≥ˆ¥À≤Œ ˝.
 *      XOS_U16 *pu16Port [OUT]:	±æµÿµƒPort,¥´»ÎNULL±Ì æ≤ª¥¯≥ˆ¥À≤Œ ˝.
 */
XOS_BOOL XOS_TCPGetLocalAddr(XOS_SOCKET sock, XOS_U32 *pu32IP, XOS_U16 *pu16Port);

    
    //////////////////////////////////////////////////////////////////////////
    //	UDP
    //////////////////////////////////////////////////////////////////////////
    
    /*  功能描述: 创建并绑定UDP socket
     *  参数说明:
     *		XOS_U32 u32IP [IN]:		绑定的IP,传入0表示由系统分配
     *      XOS_U16 u16Port [IN]:	绑定的端口,传入0表示由系统分配
     *  返回值: 返回XOS_INVALID_SOCKET表示失败,其他表示成功,返回值为socket句柄
     *	备注: 返回的SOCKET缺省为阻塞模式
     */
    XOS_SOCKET XOS_UDPBind(XOS_U32 u32IP, XOS_U16 u16Port);

/*  功能描述: 阻塞方式向指定IP和端口发送数据(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	UDP socket句柄
 *      char *pBuf [IN]:		数据缓冲地址
 *      int iBufLen [IN]:		数据上度
 *      XOS_U32 u32IP [IN]:		目标IP
 *      XOS_U16 u16Port [IN]:	目标Port
 */
XOS_BOOL XOS_UDPSendTo(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 u32IP, XOS_U16 u16Port);

/*  功能描述: 非阻塞方式向指定IP和端口发送数据(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	UDP socket句柄
 *      char *pBuf [IN]:		数据缓冲地址
 *      int iBufLen [IN]:		数据上度
 *      XOS_U32 u32IP [IN]:	目标IP
 *      XOS_U16 u16Port [IN]:	目标Port
 *  返回值: 返回-1表示出错,0表示发送成功,1表示没发成功,还需要继续尝试发送
 */
int XOS_UDPSendToNB(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 u32IP, XOS_U16 u16Port);

/*  功能描述: 阻塞方式接收UDP数据(SOCKET必须为阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	UDP socket句柄
 *      char *pBuf [IN]:		接收缓冲地址
 *      int iBufLen [IN]:		接收缓冲长度,一定要保证这个长度大于最大数据包长度
 *      XOS_U32 *pu32IP [OUT]:	来源IP,传入NULL表示不关心来源IP
 *      XOS_U16 *pu16Port [OUT]:来源Port,传入NULL表示不关心来源端口 
 *  返回值: 代表接收的数据长度(可能为0),如果返回-1代表接收失败
 */
int XOS_UDPRecvFrom(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 *pu32IP, XOS_U16 *pu16Port);

/*  功能描述: 非阻塞方式接收UDP数据(SOCKET必须为非阻塞模式)
 *  参数说明:
 *      XOS_SOCKET sock [IN]:	UDP socket句柄
 *      char *pBuf [IN]:		接收缓冲地址
 *      int iBufLen [IN]:		接收缓冲长度,一定要保证这个长度大于最大数据包长度
 *      XOS_U32 *pu32IP [OUT]:	来源IP,传入NULL表示不关心来源IP
 *      XOS_U16 *pu16Port [OUT]:	来源Port,传入NULL表示不关心来源端口
 *  返回值: 代表接收的数据长度(可能为0),如果返回-1代表接收失败
 */
int XOS_UDPRecvFromNB(XOS_SOCKET sock, char *pBuf, int iBufLen, XOS_U32 *pu32IP, XOS_U16 *pu16Port);

#ifdef __cplusplus
}
#endif

#endif //__XOSSTACK_H__
