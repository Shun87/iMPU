/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: DNS.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: zhaoqq
 *  完成日期: 2006-9-28 8:54:19
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../DNS.h"

#define MAX_DNS_LEN			64
#define MAX_TIME_OUT		(60*1000)

typedef struct _TDNS {
	int bUsed;
	char szDNS[MAX_DNS_LEN];
	XOS_U32 uiIP;
	XOS_U32 uiLastActiveTime;
	XOS_U32 uiLastQueryTime;
} TDNS;

static int s_bInited = 0;
static int s_bRunning = 0;
static int s_MaxDNSNum = 0;
static TDNS *s_pDNS = 0;
static XOS_HANDLE s_hDNSMutex = 0;

static XOS_U32 QueryIP(XOS_PSZ lpsz)
{
	XOS_U32 uiIP;
	
	#ifdef _CREARO_NDK
		int rv;
		HOSTENT *ht =  NULL;
		static char s_hostent_buf[512] = "";
		rv = DNSGetHostByName((char*)lpsz,s_hostent_buf, 512); 
		if (NOERROR == rv)
		{
			ht = (HOSTENT *)s_hostent_buf;
		}
		if ((NULL != ht) && (ht->h_addr[0] != 0))
		{
			uiIP = *(XOS_U32 *)ht->h_addr[0];
	#else
		struct hostent *ht;
		//printf("before dns query:%s\r\n", lpsz);
		ht = gethostbyname(lpsz);
		if ((NULL != ht) && (ht->h_addr_list[0] != 0))
		{
			uiIP = *(XOS_U32 *)ht->h_addr_list[0];
	#endif

			if (uiIP == 0xFFFFFFFF)
			{
				uiIP = 0;
			}
			//printf("dns query ok:%08X\r\n", uiIP);
			return uiIP;
		}
	return 0;
}

//后台自动连接线程
static int DNSPool_Daemon(void *param)
{
	int i;
	XOS_U32 uiIP;
	char szDNS[MAX_DNS_LEN];

	#ifdef _CREARO_NDK
	fdOpenSession(TSK_self());
	#endif

	//因为CR300PT在网络不通的时候,gethostbyname函数的超时时间是150秒
	//所以这个现场狗的时间改成180秒,也就是3分钟.
	WD_JOIN("DNSPool_Daemon", 3*60);
	TCS_JOIN(0);
	TCS_PUSH("DNSPool_Daemon");
	
	while (s_bRunning) 
	{
		TCS_PUSH("WD_CLEAR");
		WD_CLEAR;
		TCS_POP;
		TCS_COUNT;
		
		for (i=0; i<s_MaxDNSNum; i++) 
		{
			WD_CLEAR;

			TCS_PUSH("XOSMutex_Lock");
			XOSMutex_Lock(s_hDNSMutex);
			TCS_POP;

			if (!s_pDNS[i].bUsed)
			{
				//没有使用
				XOSMutex_Unlock(s_hDNSMutex);
				continue;
			}
			if ((s_pDNS[i].uiIP != 0) && 
				(XOS_GetTickCount() - s_pDNS[i].uiLastQueryTime < MAX_TIME_OUT))
			{
				//已经使用,但是刚刚查询才不到1分钟
				XOSMutex_Unlock(s_hDNSMutex);
				continue;
			}

			szDNS[0] = 0;
			strcpy(szDNS, s_pDNS[i].szDNS);
			XOSMutex_Unlock(s_hDNSMutex);

			//DNS查找IP
			TCS_PUSH("QueryIP");
			uiIP = QueryIP(szDNS);
			TCS_POP;
			
			s_pDNS[i].uiLastQueryTime = XOS_GetTickCount();
			
			TCS_PUSH("XOSMutex_Lock");
			XOSMutex_Lock(s_hDNSMutex);
			TCS_POP;
			if (!s_pDNS[i].bUsed)
			{
				XOSMutex_Unlock(s_hDNSMutex);
				continue;
			}
			if (strcmp(szDNS, s_pDNS[i].szDNS) != 0)
			{
				XOSMutex_Unlock(s_hDNSMutex);
				continue;
			}
			s_pDNS[i].uiIP = uiIP;
			XOSMutex_Unlock(s_hDNSMutex);
		}

		TCS_PUSH("XOS_Sleep");
		XOS_Sleep(3000);
		TCS_POP;
	}
	
	WD_LEAVE;
	TCS_LEAVE;
	return 0;
}

static int DNSPool_Init( int MaxDNSNum)
{
	if (1 == s_bInited)
	{
		return -1;
	}

	s_MaxDNSNum = MaxDNSNum;

	s_hDNSMutex = XOSMutex_Create();
	if (NULL != s_hDNSMutex) 
	{
		s_pDNS = (TDNS *)malloc(sizeof(TDNS)*s_MaxDNSNum);
		if (0 != s_pDNS) 
		{
			memset(s_pDNS, 0, sizeof(TDNS)*s_MaxDNSNum);
			s_bRunning = 1;					
			if (XOSThread_Create(0x4000, &DNSPool_Daemon, 0) != NULL)
			{
				s_bInited = 1;
				return 0;
			}
			free(s_pDNS);
		}
		XOSMutex_Delete(s_hDNSMutex);
	}
	return -1;
}

/*
static int DNSPool_Close()
{
	if (0 == s_bInited)
	{
		return -1;
	}

	s_bInited = 0;
	s_bRunning = 0;
		
	free(pDNS);
	XOSMutex_Delete(s_hDNSMutex);

	return 0;	
}
*/

static int DNSPool_GetIPPortByDNS(char *szDNS, unsigned int *puiIP)
{
	int i;

	if (0 == s_bInited) 
	{
		return -1;
	}
	
	TCS_PUSH("XOSMutex_Lock");
	XOSMutex_Lock(s_hDNSMutex);
	TCS_POP;

	for (i=0; i<s_MaxDNSNum; i++) 
	{
		if (!s_pDNS[i].bUsed)
		{
			continue;
		}
		if (strcmp(s_pDNS[i].szDNS, szDNS) == 0)
		{
			break;
		}
	}
	if (i == s_MaxDNSNum) 
	{
		XOSMutex_Unlock(s_hDNSMutex);
		return -1;
	}

	*puiIP = s_pDNS[i].uiIP;
	s_pDNS[i].uiLastActiveTime = XOS_GetTickCount();
	XOSMutex_Unlock(s_hDNSMutex);
	return 0;
}

static int DNSPool_AddDNS(char *szDNS)
{
	int i;

	if (0 == s_bInited) 
	{
		return -1;
	}
	
	TCS_PUSH("XOSMutex_Lock");
	XOSMutex_Lock(s_hDNSMutex);
	TCS_POP;
	for (i=0; i<s_MaxDNSNum; i++) 
	{
		if (!s_pDNS[i].bUsed)
		{
			continue; 
		}
		if (strcmp(s_pDNS[i].szDNS, szDNS) == 0) 
		{
			//如果有,就没有必要添加了.
			XOSMutex_Unlock(s_hDNSMutex);
			return 0;
		}
	}

	//寻找未用的
	for (i=0; i<s_MaxDNSNum; i++) 
	{
		if (!s_pDNS[i].bUsed) 
		{
			break; 
		}
	}
	if (i == s_MaxDNSNum) 
	{
		int j = 0;
		unsigned int ui,uiMax = 0;

		//满了,找最老的用之
		ui = XOS_GetTickCount();
		for (i=0; i<s_MaxDNSNum; i++) 
		{
			//因为满了,所以每个都是有效的.
			if ((ui - s_pDNS[i].uiLastActiveTime) > uiMax)
			{
				uiMax = ui - s_pDNS[i].uiLastActiveTime;
				j = i;
			}
		}
		i = j;
	}

	memcpy(s_pDNS[i].szDNS, szDNS, MAX_DNS_LEN-1);
	s_pDNS[i].uiLastActiveTime = XOS_GetTickCount();
	s_pDNS[i].uiIP = 0;
	s_pDNS[i].bUsed = 1;
	XOSMutex_Unlock(s_hDNSMutex);
	
	return 0;
}

void DNSPool_Report(int (*pPrn)(const char *,...) )
{
	int i;
	struct in_addr in;

	if (pPrn == 0) 
	{
		return ;
	}

	pPrn("====DNSPool Report Begin====\r\n");
	pPrn("bInited:%d\r\n", s_bInited);
	pPrn("bRunning:%d\r\n", s_bRunning);
	pPrn("Num:%d,Mutex:%08X\r\n", s_MaxDNSNum, s_hDNSMutex);
	pPrn("No\tbUsed\tszDNS\tuiIP\ttLastActiveTime\r\n");
	for (i=0; i<s_MaxDNSNum; i++) 
	{
		if (!s_pDNS[i].bUsed) 
		{
			continue;
		}
		in.s_addr = s_pDNS[i].uiIP;
		pPrn("%d\t%d\t%s\t%s\t%08X\r\n", i, s_pDNS[i].bUsed, s_pDNS[i].szDNS, inet_ntoa(in), s_pDNS[i].uiLastActiveTime);
	}
	pPrn("====DNSPool Report End====\r\n");

	return ;	
}

void DNS2IPReport(int (*pPrn)(const char *,...))
{
	DNSPool_Report(pPrn);
}

int DNS2IPInit()
{
	if (DNSPool_Init(128) != 0)
	{
		return -1;
	}
	s_bInited = 1;
	return 0;
}

XOS_U32 DNS2IP(XOS_CPSZ lpsz)
{
	XOS_U32 uiIP;

	if (s_bInited == 0)
	{
		return 0;
	}

	uiIP = inet_addr(lpsz);
	if ((uiIP == 0) || (uiIP == 0xFFFFFFFF))
	{
		TCS_PUSH("DNSPool_GetIPPortByDNS");
		if (DNSPool_GetIPPortByDNS((char *)lpsz, &uiIP) != 0)
		{
			TCS_POP;
			//没有找到,把此DNS放到pool中
			TCS_PUSH("DNSPool_AddDNS");
			DNSPool_AddDNS((char *)lpsz);
			TCS_POP;
			return 0;
		}
		TCS_POP;
	}
	return uiIP;
}

XOS_BOOL Addr2IPAndPort(XOS_CPSZ lpsz, XOS_U32 *puiIP, XOS_U16 *pusPort, XOS_BOOL bDefPort)
{
	char *pszIP; 
	char *pszPort;
	pszIP = (char *)lpsz;
	pszPort = strstr(lpsz, ":");
	if (pszPort == 0) 
	{
		//没有冒号
		if (!bDefPort)
		{
			//并且无缺省端口,URL不合法
			return XOS_FALSE;
		}

		*puiIP = inet_addr(pszIP);
		if ((*puiIP == 0) || (*puiIP == 0xFFFFFFFF))
		{
			TCS_PUSH("DNS2IP");
			*puiIP = DNS2IP(pszIP);
			TCS_POP;
		}
		if ((*puiIP == 0) || (*puiIP == 0xFFFFFFFF))
		{
			return XOS_FALSE;
		}

		return XOS_TRUE;
	}
	else
	{
		pszPort[0] = 0;
		pszPort ++;
		*puiIP = inet_addr(pszIP);
		if ((*puiIP == 0) || (*puiIP == 0xFFFFFFFF))
		{
			TCS_PUSH("DNS2IP");
			*puiIP = DNS2IP(pszIP);
			TCS_POP;
		}
		if ((*puiIP == 0) || (*puiIP == 0xFFFFFFFF))
		{
			return XOS_FALSE;
		}

		*pusPort = atoi(pszPort);
		if (*pusPort == 0) 
		{
			return XOS_FALSE;
		}

		return XOS_TRUE;
	}
}

/*  功能描述: 使用IP和端口合成URL
 */
XOS_PSZ IPAndPort2Addr(XOS_U32 uiIP, XOS_U16 usPort)
{
	static char s_szURL[64];
	struct in_addr in;
	s_szURL[0] = 0;
	in.s_addr = uiIP;
	sprintf(s_szURL, "%s:%d", inet_ntoa(in), usPort);
	return s_szURL;
}
