/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: Proxy.h
 *  文件标识: 
 *  摘    要: 完成了代理的连接,支持HTTP,SOCKET4,SOCKET5代理
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-02-28 17:11:41
 *  修正备注: 
 *  
 */

#ifndef __PROXY_H__
#define __PROXY_H__

//代理连接超时时间
#define MAX_PROXY_TIMEOUT			(60*1000)

//代理操作结果
#define PROXY_PROBE_OK				0
#define PROXY_PROBE_WOULDBLOCK		1
#define PROXY_PROBE_TIMEOUT			2
#define PROXY_PROBE_ERROR			-1

//代理类型
#define PROXY_TYPE_NULL				0
#define PROXY_TYPE_HTTP				1
#define PROXY_TYPE_SOCKET4			2
#define PROXY_TYPE_SOCKET5			3

//代理地址等信息最大长度
#define MAX_PROXY_DESTADDR_LEN		256
#define MAX_PROXY_ADDR_LEN			256
#define MAX_PROXY_USERNAME_LEN		64
#define MAX_PROXY_PASSWORD_LEN		64

//代理内部状态
#define PROXY_STATUS_NULL						0
#define PROXY_STATUS_CONNECTING					1
#define PROXY_STATUS_NEGOTIATION_REQUEST		2
#define PROXY_STATUS_NEGOTIATION_RESPONSE		3
#define PROXY_STATUS_SUBNEGOTIATION_REQUEST		4
#define PROXY_STATUS_SUBNEGOTIATION_RESPONSE	5
#define PROXY_STATUS_CONNECTION_REQUEST			6
#define PROXY_STATUS_CONNECTION_RESPONSE		7
#define PROXY_STATUS_CONNECTION_FINISHED		8
#define PROXY_STATUS_FAILED						9

typedef struct _TProxy TProxy;
struct _TProxy  
{
//public:
	void (*Construct)(TProxy *pProxy);
	void (*Destroy)(TProxy *pProxy);

	/*  功能描述: 创建代理对象
	 *  参数说明:
	 *      XOS_PSZ pszProxyType [IN]:	代理类型,取值可以是:"HTTP","SOCKET4","SOCKET5"
	 *      XOS_PSZ pszDestAddr [IN]:	目标地址,格式为:ip:port或者domainname:port
	 *      XOS_PSZ pszProxyAddr [IN]:	代理地址,格式同上
	 *      XOS_PSZ pszUserName [IN]:	连接代理的用户名和密码
	 *      XOS_PSZ pszPassword [IN]:	连接代理的密码
	 */
	XOS_BOOL (*Create)(TProxy *pProxy, XOS_PSZ pszProxyType, XOS_PSZ pszDestAddr, XOS_PSZ pszProxyAddr, XOS_PSZ pszUserName, XOS_PSZ pszPassword);

	/*  功能描述: 探测代理是否连接成果
	 *  返回值: 返回值见前面的"代理操作结果"宏定义
	 */
	int (*Probe)(TProxy *pProxy);

	/*  功能描述: 获取代理连接成功的socket句柄
	 *  返回值: 连接成功的socket句柄
	 *  备注: 在Probe成功之后才调用这个函数,就会返回一个合法值,否则返回XOS_INVALID_SOCKET
	 */
	XOS_SOCKET (*GetSocket)(TProxy *pProxy);

//private:
	int (*ProbeHttp)(TProxy *pProxy);
	int (*ProbeSocket4)(TProxy *pProxy);
	int (*ProbeSocket5)(TProxy *pProxy);

	int m_status;
	XOS_SOCKET m_sock;
	XOS_SOCKET m_sock_bak;
	int m_iProxyType;
	char m_szDestAddr[MAX_PROXY_DESTADDR_LEN];
	char m_szProxyAddr[MAX_PROXY_ADDR_LEN];
	char m_szUserName[MAX_PROXY_USERNAME_LEN];
	char m_szPassword[MAX_PROXY_PASSWORD_LEN];
	char m_tmpbuf[256];
	char m_sendbuf[1024];
	char m_recvbuf[1024];
	int	 m_isendbufLen;
	int  m_ihaveSendLen;
	int  m_irecvLen;
	XOS_U32 m_uiOperationBeginTime;
	XOS_U32 m_uiDestIP;
	XOS_U16 m_usDestPort;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TProxy PROXY;

#ifdef __cplusplus
}
#endif

#endif //__PROXY_H__
