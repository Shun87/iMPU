/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOSStack_Defs_LINUX.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: linuxƽ̨��Э��ջ���ͷ�ļ��Ͷ���
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-10-16 18:27:12
 *  ������ע: 
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

//socket�������
#define XOS_SOCKET			int
//�Ƿ�socketֵ
#define	XOS_INVALID_SOCKET	(-1)

//��socket���ó�����ģʽ,sΪsocket���,����ͬ
#define xos_setsockblock(s) \
{ \
	int __opts; \
	__opts = fcntl((int)s, F_GETFL); \
	__opts = (__opts & ~O_NONBLOCK); \
	fcntl((int)s, F_SETFL, __opts); \
}

//��socket���óɷ�����ģʽ
#define xos_setsocknonblock(s) \
{	\
	int __opts; \
	__opts = fcntl((int)s, F_GETFL); \
	__opts = (__opts | O_NONBLOCK); \
	fcntl((int)s, F_SETFL, __opts); \
}

//��socket���óɶ˿ڿ�����
#define xos_setsockreuseaddr(s) \
{ \
	int __bReuseAddr = 1; \
	setsockopt((int)s, SOL_SOCKET, SO_REUSEADDR, (char *)&__bReuseAddr, sizeof(__bReuseAddr)); \
}

//��socket���ó��ܹ��շ��㲥��
#define xos_setsockbroadcast(s) \
{ \
	int __optval = 1; \
	setsockopt((int)s, SOL_SOCKET, SO_BROADCAST, (char *)&__optval, sizeof(__optval)); \
}

//��socket����ָ�����鲥��,iΪ32λ�鲥��ַ
#define xos_setsockaddmulticast(s, i) \
{ \
	struct ip_mreq __mreq; \
	__mreq.imr_interface.s_addr = INADDR_ANY; \
	__mreq.imr_multiaddr.s_addr = i; \
	setsockopt((int)s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&__mreq, sizeof(__mreq)); \
}

//��socket��ָ�����鲥����ɾ��,iͬ��
#define xos_setsockdropmulticast(s, i) \
{ \
	struct ip_mreq __mreq; \
	__mreq.imr_interface.s_addr = INADDR_ANY; \
	__mreq.imr_multiaddr.s_addr = i; \
	setsockopt((int)s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&__mreq, sizeof(__mreq)); \
}

//����socket���ջ����С,iΪ�����С
#define xos_setsockrecvbuf(s, i) \
{ \
	int __i = i; \
	setsockopt((int)s, SOL_SOCKET, SO_RCVBUF, (char *)&__i, sizeof(__i)); \
}

//����socket���ͻ����С,iͬ��
#define xos_setsocksendbuf(s, i) \
{ \
	int __i = i; \
	setsockopt((int)s, SOL_SOCKET, SO_SNDBUF, (char *)&__i, sizeof(__i)); \
}

//����socket�ȴ�ʱ��,o��ʾ�Ƿ�ʹ�ܵȴ�,l��ʾ�ȴ�ʱ��,��λ��
//LINUX�Ƿ�Ҫ���û�δ֪,�������û�������,������������
//LINUX��������,���������,�رյ�ʱ��ͻ�ȴ����õ�lingerʱ��
#define xos_setsocklinger(s, o, l) \
{ \
	struct linger __Linger; \
	__Linger.l_onoff = o; \
	__Linger.l_linger = l; \
	setsockopt((int)s, SOL_SOCKET, SO_LINGER, (char*)&__Linger, sizeof(__Linger)); \
}

//����socketΪ���ӳ�ģʽ,
//���庬�岻��̫����.����ƽ̨��ͬ����ͬ,һ�㲻�����
#define xos_setsocknodelay(s) \
/*{ \
	int __one = 1; \
	setsockopt((int)s, IPPROTO_TCP, TCP_NODELAY, (char*)&__one, sizeof(int)); 
}
*/

//����socketΪ��������
//��������ƽ̨��֧��
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
