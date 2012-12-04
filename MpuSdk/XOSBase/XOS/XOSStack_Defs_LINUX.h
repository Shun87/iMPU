/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSStack_Defs_LINUX.h
 *  文件标识: 
 *  摘    要: linux平台下协议栈相关头文件和定义
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-16 18:27:12
 *  修正备注: 
 *  
 */

#ifndef __XOSSTACK_DEFS_LINUX_H__
#define __XOSSTACK_DEFS_LINUX_H__

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <pthread.h>
#include <stdarg.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdarg.h>

//socket句柄类型
#define XOS_SOCKET			int
//非法socket值
#define	XOS_INVALID_SOCKET	(-1)

//将socket设置成阻塞模式,s为socket句柄,以下同
#define xos_setsockblock(s) \
{ \
	int __opts; \
	__opts = fcntl((int)s, F_GETFL); \
	__opts = (__opts & ~O_NONBLOCK); \
	fcntl((int)s, F_SETFL, __opts); \
}

//将socket设置成非阻塞模式
#define xos_setsocknonblock(s) \
{	\
	int __opts; \
	__opts = fcntl((int)s, F_GETFL); \
	__opts = (__opts | O_NONBLOCK); \
	fcntl((int)s, F_SETFL, __opts); \
}

//将socket设置成端口可重用
#define xos_setsockreuseaddr(s) \
{ \
	int __bReuseAddr = 1; \
	setsockopt((int)s, SOL_SOCKET, SO_REUSEADDR, (char *)&__bReuseAddr, sizeof(__bReuseAddr)); \
}

//将socket设置成能够收发广播包
#define xos_setsockbroadcast(s) \
{ \
	int __optval = 1; \
	setsockopt((int)s, SOL_SOCKET, SO_BROADCAST, (char *)&__optval, sizeof(__optval)); \
}

//将socket加入指定的组播组,i为32位组播地址
#define xos_setsockaddmulticast(s, i) \
{ \
	struct ip_mreq __mreq; \
	__mreq.imr_interface.s_addr = INADDR_ANY; \
	__mreq.imr_multiaddr.s_addr = i; \
	setsockopt((int)s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&__mreq, sizeof(__mreq)); \
}

//将socket从指定的组播组中删除,i同上
#define xos_setsockdropmulticast(s, i) \
{ \
	struct ip_mreq __mreq; \
	__mreq.imr_interface.s_addr = INADDR_ANY; \
	__mreq.imr_multiaddr.s_addr = i; \
	setsockopt((int)s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&__mreq, sizeof(__mreq)); \
}

//设置socket接收缓冲大小,i为缓冲大小
#define xos_setsockrecvbuf(s, i) \
{ \
	int __i = i; \
	setsockopt((int)s, SOL_SOCKET, SO_RCVBUF, (char *)&__i, sizeof(__i)); \
}

//设置socket发送缓冲大小,i同上
#define xos_setsocksendbuf(s, i) \
{ \
	int __i = i; \
	setsockopt((int)s, SOL_SOCKET, SO_SNDBUF, (char *)&__i, sizeof(__i)); \
}

//设置socket等待时间,o表示是否使能等待,l表示等待时间,单位秒
//LINUX是否要设置还未知,好像不设置会出问题的,所以又设置了
//LINUX不能设置,如果设置了,关闭的时候就会等待设置的linger时间
#define xos_setsocklinger(s, o, l) \
{ \
	struct linger __Linger; \
	__Linger.l_onoff = o; \
	__Linger.l_linger = l; \
	setsockopt((int)s, SOL_SOCKET, SO_LINGER, (char*)&__Linger, sizeof(__Linger)); \
}

//设置socket为非延迟模式,
//具体含义不是太明白.根据平台不同而不同,一般不设这个
#define xos_setsocknodelay(s) \
/*{ \
	int __one = 1; \
	setsockopt((int)s, IPPROTO_TCP, TCP_NODELAY, (char*)&__one, sizeof(int)); 
}
*/

//设置socket为保持连接
//不是所有平台都支持
#define xos_setsockkeepalive(s) \
{ \
	int __one = 1; \
	setsockopt((int)s, SOL_SOCKET, SO_KEEPALIVE, (char*)&__one, sizeof(int)); \
}

#define setsockrecverr(s) /*\
{ \
	int __optval = 1; \
	setsockopt((int)s, SOL_IP, IP_RECVERR, (char *)&__optval, sizeof(__optval)); \
}*/

#endif //__XOSSTACK_DEFS_LINUX_H__
