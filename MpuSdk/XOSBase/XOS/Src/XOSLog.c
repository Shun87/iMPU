/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSLog.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2009-3-2 20:35:19
 *  修正备注: 
 *  
 */

#include "../XOS.h"

#define MAX_CUSTOM_LEVEL_NAME_LEN	64
typedef struct _TCustomLevelEnable
{
	XOS_BOOL bUsed;
	char szName[MAX_CUSTOM_LEVEL_NAME_LEN];
} TCustomLevelEnable;

#define MAX_CUSTOM_LEVEL_ENABLE_NUM			16
static TCustomLevelEnable s_customLevelEnables[MAX_CUSTOM_LEVEL_ENABLE_NUM] =
{
	{XOS_FALSE, ""},	//0
	{XOS_FALSE, ""},	//1
	{XOS_FALSE, ""},	//2
	{XOS_FALSE, ""},	//3
	{XOS_FALSE, ""},	//4
	{XOS_FALSE, ""},	//5
	{XOS_FALSE, ""},	//6
	{XOS_FALSE, ""},	//7
	{XOS_FALSE, ""},	//8
	{XOS_FALSE, ""},	//9
	{XOS_FALSE, ""},	//10
	{XOS_FALSE, ""},	//11
	{XOS_FALSE, ""},	//12
	{XOS_FALSE, ""},	//13
	{XOS_FALSE, ""},	//14
	{XOS_FALSE, ""},	//15
};

static XOSLOG_GET_CHAR s_get_char = NULL;
static XOSLOG_PUT_CHAR s_put_char = (XOSLOG_PUT_CHAR)&putchar;
static XOSLOG_ON_COMMAND_IN s_on_command_in = NULL;
static XOSLOG_ON_HELP_IN s_on_help_in = NULL;

static XOS_SOCKET s_sockListen = XOS_INVALID_SOCKET;
static XOS_SOCKET s_sockAccept = XOS_INVALID_SOCKET;
static XOS_HANDLE s_hThreadFun = NULL;
static XOS_HANDLE s_hMutex = NULL;
static XOS_BOOL s_bRunning = XOS_FALSE;

static char s_binCommandBuffer[1024];
static char s_binOutputBuffer[64*1024];
static int s_iCommandRecvLen = 0; //命令接收用
static XOS_U32 s_u32LastTCPRecvTime;
static XOS_U32 s_u32XOSLogOpenTime;

static XOS_BOOL s_bEnableXX = XOS_TRUE;
static XOS_BOOL s_bEnableIM = XOS_TRUE;
static XOS_BOOL s_bEnableEX = XOS_TRUE;
static XOS_BOOL s_bEnableCR = XOS_TRUE;
static XOS_BOOL s_bEnableUC = XOS_TRUE;

static void XOSLog_PutChar(char c);

//////////////////////////////////////////////////////////////////////////
//

static char XOSLog_Char2Hex(char c)
{
	if ((c >= 'a') && (c <= 'f'))
	{
		return c - 'a' + 10;
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		return c - 'A' + 10;
	}
	else if ((c >= '0') && (c <= '9'))
	{
		return c - '0';
	}
	else
	{
		return -1;
	}
}

static XOS_BOOL XOSLog_Str2Hex(char *pStr, char *pHex)
{
	char c0,c1;
	char *hp,*sp,*sep;

	if ((strlen(pStr) % 2) != 0)
	{
		return XOS_FALSE;
	}

	sep = pStr + strlen(pStr);
	hp = pHex;
	sp = pStr;
	while (sp < sep)
	{
		c0 = XOSLog_Char2Hex(sp[0]);
		c1 = XOSLog_Char2Hex(sp[1]);
		if ((c0 == (char)-1) || (c1 == (char)-1))
		{
			return XOS_FALSE;
		}
		hp[0] = (c0 << 4) + c1;
		sp += 2;
		hp ++;
	}

	return XOS_TRUE;
}

//////////////////////////////////////////////////////////////////////////
//命令处理
 
static void XOSLog_PrintPrompt()
{
	nprintf("XOS:>");
}
static void XOSLog_PrintWelcome()
{
	nprintf("Welcome to XOS log system v1.0 sock:%d.\r\n", s_sockAccept);
}
static void XOSLog_PrintNow()
{
	struct tm st;
	XOS_U32 u32TotalTime;
	u32TotalTime = XOS_GetTickCount() - s_u32XOSLogOpenTime;
	XOS_GetLocalTime(&st);
	nprintf("now: %02d-%02d-%02d %02d:%02d:%02d tick:0x%08X.\r\n", 
				st.tm_year+1900-2000, st.tm_mon+1, st.tm_mday,	
				st.tm_hour, st.tm_min, st.tm_sec, XOS_GetTickCount());
	nprintf("system has running for: %02d %02d:%02d:%02d.\r\n", 
					u32TotalTime/1000/60/60/24,
					(u32TotalTime/1000/60/60)%24,
					(u32TotalTime/1000/60)%60,
					(u32TotalTime/1000)%60);
}

void XOSLog_PrintHelpSetLevel()
{
	nprintf("[syntax]:   setlevel(sl) [level] [on/off]\r\n");
	nprintf("[function]: set log output level.\r\n");
	nprintf("[param]:\r\n");
	nprintf("    [level]: log level. 'null' to switch off all log(include custom log), [on/off] must be null.\r\n");
	nprintf("           now supportted level is following:\r\n");
	nprintf("               df    set all level to default, [on/off] no care.\r\n");
	nprintf("               xx    normal log. default on.\r\n");
	nprintf("               im    impossible log. default on.\r\n");
	nprintf("               ex    except log. default on.\r\n");
	nprintf("               cr    critical flow log. default on.\r\n");
	nprintf("               uc    user operate log, include net command. default on.\r\n");
	nprintf("           we cann't list other custom levels here, default off.\r\n");
	nprintf("               ...\r\n");
	nprintf("    [on/off]: log switch. 1 to switch on log of [level], else switch off.\r\n");
	nprintf("[example]:\r\n");
	nprintf("    sl        ->switch off all log.\r\n");
	nprintf("    sl df     ->set all level to default.\r\n");
	nprintf("    sl im 1   ->switch im log on.\r\n");
	nprintf("    sl cr 0   ->switch cr log off.\r\n");
}

void XOSLog_PrintHelpMem()
{
	nprintf("[syntax]:   mem(m) <addr> <len>\r\n");
	nprintf("[function]: print memory.\r\n");
	nprintf("[param]:\r\n");
	nprintf("    <addr>: memory base address, prefix by '0x', in hex.\r\n");
	nprintf("    <len>: length of bytes to print.\r\n");
	nprintf("[example]:\r\n");
	nprintf("    m 0x00123456 128        ->print 128 bytes at address 0x00123456.\r\n");
}

void XOSLog_OnHelpIn(XOS_PSZ pszParam)
{
	XOSLog_PrintWelcome();

	if (pszParam != NULL)
	{
		if ((strcmp(pszParam, "setlevel") == 0) || (strcmp(pszParam, "sl") == 0))
		{
			XOSLog_PrintHelpSetLevel();
			return ;
		}
		if ((strcmp(pszParam, "mem") == 0) || (strcmp(pszParam, "m") == 0))
		{
			XOSLog_PrintHelpMem();
			return ;
		}
		else
		{
			if (s_on_help_in != NULL)
			{
				if (!s_on_help_in(pszParam))
				{
					nprintf("unkown help target: %s.\r\n", pszParam);
				}
				else
				{
					return ;
				}
			}
			else
			{
				return ;
			}
		}
	}

	nprintf("supportted command:\r\n");
	nprintf("    help(h):         print this message.\r\n");
	nprintf("    exit(e):         exit log system.\r\n");
	nprintf("    setlevel(sl):    set log level, enter 'help setlevel(sl) for detail.\r\n");
	nprintf("    now(n):          print now and system running time.\r\n");
	nprintf("    tcs(t):          print thread call stack.\r\n");
	nprintf("    mem(m):          print memory, enter 'help mem(m) for detail.\r\n");
	nprintf("    memrate(mr):     print used memory rate.\r\n");
	nprintf("    cpurate(ur):     print used cpu rate.\r\n");

	//输出第一层帮助
	if (s_on_help_in != NULL)
	{
		s_on_help_in(NULL);
	}
}

void XOSLog_OnSetLevelIn(XOS_PSZ pszParam)
{
	int i;
	char *pszLevel;
	char *pszOnOff;
	pszLevel = strtok(pszParam, " ");
	if (pszLevel == NULL)
	{
		//缺省全关
		pszLevel = "";
	}
	pszOnOff = strtok(NULL, " ");
	if (pszOnOff == NULL)
	{
		//缺省为0
		pszOnOff = "0";
	}
	if (strcmp(pszLevel, "xx") == 0)
	{
		if (strcmp(pszOnOff, "1") == 0)
		{
			s_bEnableXX = XOS_TRUE;
		}
		else
		{
			s_bEnableXX = XOS_FALSE;
		}
	}
	else if (strcmp(pszLevel, "im") == 0)
	{
		if (strcmp(pszOnOff, "1") == 0)
		{
			s_bEnableIM = XOS_TRUE;
		}
		else
		{
			s_bEnableIM = XOS_FALSE;
		}
	}
	else if (strcmp(pszLevel, "ex") == 0)
	{
		if (strcmp(pszOnOff, "1") == 0)
		{
			s_bEnableEX = XOS_TRUE;
		}
		else
		{
			s_bEnableEX = XOS_FALSE;
		}
	}
	else if (strcmp(pszLevel, "cr") == 0)
	{
		if (strcmp(pszOnOff, "1") == 0)
		{
			s_bEnableCR = XOS_TRUE;
		}
		else
		{
			s_bEnableCR = XOS_FALSE;
		}
	}
	else if (strcmp(pszLevel, "uc") == 0)
	{
		if (strcmp(pszOnOff, "1") == 0)
		{
			s_bEnableUC = XOS_TRUE;
		}
		else
		{
			s_bEnableUC = XOS_FALSE;
		}
	}
	else if (strcmp(pszLevel, "df") == 0)
	{
		s_bEnableXX = XOS_TRUE;
		s_bEnableIM = XOS_TRUE;
		s_bEnableEX = XOS_TRUE;
		s_bEnableCR = XOS_TRUE;
		s_bEnableUC = XOS_TRUE;
		memset(s_customLevelEnables, 0, sizeof(TCustomLevelEnable)*MAX_CUSTOM_LEVEL_ENABLE_NUM);
		nprintf("setlevel default ok.\r\n", pszLevel, pszOnOff);
	}
	else
	{
		//遍历寻找,如果开的话,已经开了,就不开了,如果是关的话,找到就关了
		for (i=0; i<MAX_CUSTOM_LEVEL_ENABLE_NUM; i++)
		{
			if (!s_customLevelEnables[i].bUsed)
			{
				continue;
			}
			if (strcmp(pszLevel, s_customLevelEnables[i].szName) == 0)
			{
				if (strcmp(pszOnOff, "1") == 0)
				{
					break;
				}
				else
				{
					s_customLevelEnables[i].bUsed = XOS_FALSE;
					memset(s_customLevelEnables[i].szName, 0, MAX_CUSTOM_LEVEL_NAME_LEN);
					break;
				}
			}
		}
		if (i == MAX_CUSTOM_LEVEL_ENABLE_NUM)
		{
			//如果是开的话,还得找个没用的给开了
			if (strcmp(pszOnOff, "1") == 0)
			{
				for (i=0; i<MAX_CUSTOM_LEVEL_ENABLE_NUM; i++)
				{
					if (!s_customLevelEnables[i].bUsed)
					{
						s_customLevelEnables[i].bUsed = XOS_TRUE;
						memset(s_customLevelEnables[i].szName, 0, MAX_CUSTOM_LEVEL_NAME_LEN);
						strncpy(s_customLevelEnables[i].szName, pszLevel, MAX_CUSTOM_LEVEL_NAME_LEN-1);
						break;
					}
				}
				if (i == MAX_CUSTOM_LEVEL_ENABLE_NUM)
				{
					nprintf("setlevel %s %s overflow.\r\n", pszLevel, pszOnOff);
					return ;
				}
			}
			else
			{
				s_bEnableXX = XOS_FALSE;
				s_bEnableIM = XOS_FALSE;
				s_bEnableEX = XOS_FALSE;
				s_bEnableCR = XOS_FALSE;
				s_bEnableUC = XOS_FALSE;
				memset(s_customLevelEnables, 0, sizeof(TCustomLevelEnable)*MAX_CUSTOM_LEVEL_ENABLE_NUM);
				nprintf("setlevel all off ok.\r\n", pszLevel, pszOnOff);
			}
		}
	}
	nprintf("setlevel %s %s ok.\r\n", pszLevel, pszOnOff);
	return ;
}

void XOSLog_OnMemIn(XOS_PSZ pszParam)
{
	char *p;
	if ((p=strtok(pszParam, " ")) != NULL)
	{
		unsigned char *pMem;
		if (XOSLog_Str2Hex(p+2, p+2))
		{
			memcpy(&pMem, (char*)(p+2), 4);
			pMem = (char *)xos_ntohu32((unsigned long)pMem);
			if ((p=strtok(NULL, " ")) != NULL)
			{
				int iLen;
				iLen = atoi(p);
				XOSLog_PrintMem(pMem, iLen);
			}
		}
	}
}

void XOSLog_OnCommandIn(XOS_PSZ pszCommand)
{
	XOS_PSZ pszParam;
	pszCommand = strtok(pszCommand, " ");
	pszParam = strtok(NULL, "\0");
	if (pszCommand == NULL)
	{
		return ;
	}
	if ((strcmp(pszCommand, "help") == 0) || (strcmp(pszCommand, "h") == 0))
	{
		XOSLog_OnHelpIn(pszParam);
	}
	else if ((strcmp(pszCommand, "exit") == 0) || (strcmp(pszCommand, "e") == 0))
	{
		if (s_sockAccept == XOS_INVALID_SOCKET)
		{
			nprintf("local log, DO NOT support this command.\r\n");
		}
		else
		{
			nprintf("thanks, bye-bye.\r\n");
			XOS_CloseSocket(s_sockAccept);
			s_sockAccept = XOS_INVALID_SOCKET;
		}
	}
	else if ((strcmp(pszCommand, "setlevel") == 0) || (strcmp(pszCommand, "sl") == 0))
	{
		XOSLog_OnSetLevelIn(pszParam);
	}
	else if ((strcmp(pszCommand, "now") == 0) || (strcmp(pszCommand, "n") == 0))
	{
		XOSLog_PrintNow();
	}
	else if ((strcmp(pszCommand, "tcs") == 0) || (strcmp(pszCommand, "t") == 0))
	{
		TCS_REPORT(nprintf);
	}
	else if ((strcmp(pszCommand, "wd") == 0) || (strcmp(pszCommand, "w") == 0))
	{
		WD_REPORT(nprintf);
	}
	else if ((strcmp(pszCommand, "mem") == 0) || (strcmp(pszCommand, "m") == 0))
	{
		XOSLog_OnMemIn(pszParam);
	}			
	else if ((strcmp(pszCommand, "memrate") == 0) || (strcmp(pszCommand, "mr") == 0))
	{
		nprintf("mem rate: %d%%.\r\n", XOS_GetMemRate());
	}
	else if ((strcmp(pszCommand, "cpurate") == 0) || (strcmp(pszCommand, "ur") == 0))
	{
		char CPURate[7];
		memset(CPURate, 0, sizeof(CPURate));
		XOS_GetCPURate(CPURate);
		nprintf("cpu:%d%% user:%d%% nice:%d%% system:%d%% idle:%d%% iowait:%d%% irq:%d%% softirq:%d%%\r\n", 
				 100 - CPURate[3],
				 CPURate[0], CPURate[1], CPURate[2], CPURate[3], CPURate[4], CPURate[5], CPURate[6]);
	}
	else
	{
		if (s_on_command_in != NULL)
		{
			if (!s_on_command_in(pszCommand, pszParam))
			{
				//外面没有处理,调用系统命令
				FILE *fp;

				//修好空格
				if (pszParam != NULL)
				{
					pszCommand[strlen(pszCommand)] = ' ';
				}

				#ifdef WIN32
					#ifndef WINCE
							fp = _popen(pszCommand, "r");
					#else
							fp = fopen(pszCommand, "r");
					#endif
				#elif _CREARO_NKD
				return ;
				#else //LINUX
				fp = popen(pszCommand, "r");
				#endif
				nprintf("unkown command: %s, call system instead [%08X].\r\n", pszCommand, fp);
				XOSMutex_Lock(s_hMutex);
				while (fp != NULL)
				{
					int c;
					c = fgetc(fp);
					if (c != EOF)
					{
						XOSLog_PutChar((char)c);
					}
					else
					{
						#ifdef WIN32
							#ifndef WINCE
								_pclose(fp);
							#else
								fclose(fp);
							#endif
						#else
						pclose(fp);
						#endif
						break;
					}
				}
				XOSMutex_Unlock(s_hMutex);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//命令接收分析

#ifdef WIN32
	#define lastSockErrno WSAGetLastError()
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#define ENOBUFS WSAENOBUFS
#elif _CREARO_NDK
	#define lastSockErrno fdError()
	#define SOCKET_ERROR	-1
#else //LINUX
	#define lastSockErrno	errno
	#define SOCKET_ERROR	-1
	#define INVALID_SOCKET	-1
#endif //WIN32

static int XOSLog_TCPSendChar(char c)
{
	int rv;
	rv = send(s_sockAccept, &c, 1, 0);
	if (rv == SOCKET_ERROR)
	{
		rv = lastSockErrno;
		if ((rv == EWOULDBLOCK) || (rv == ENOBUFS))
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}
static void CloseSocket(XOS_SOCKET sock)
{
#ifdef WIN32
	closesocket(sock);
#elif _CREARO_NDK
	fdClose(sock);
#else //LINUX
	close(sock);
#endif //WIN32
}
static void XOSLog_PutChar(char c)
{
	int rv;
	if (s_sockAccept != XOS_INVALID_SOCKET)
	{
		//循环发送数据,如果1分钟都没有发出去就关闭连接
		//1分钟都没有发出一个字节,这日志也没法看了.
		XOS_U32 u32SendBeginTime;
		u32SendBeginTime = XOS_GetTickCount();
		while (s_bRunning)
		{
			rv = XOSLog_TCPSendChar(c);
			if (rv == 0)
			{
				//发送成功
				return ;
			}
			else if (rv == 1)
			{
				//没有发送成功,则判断时间
				if ((XOS_GetTickCount()-u32SendBeginTime) > 60*1000)
				{
					CloseSocket(s_sockAccept);
					s_sockAccept = XOS_INVALID_SOCKET;
					return ;
				}
			}
			else //-1
			{
				CloseSocket(s_sockAccept);
				s_sockAccept = XOS_INVALID_SOCKET;
				return ;
			}
			//等待一会再发
			XOS_Sleep(2);
		}
	}
	else
	{
		if (s_put_char != NULL)
		{
			s_put_char(c);
		}
	}
}

static int XOSLog_RecvCommand(XOS_SOCKET sock, char *pBuf, int iBufLen, 
							  int *piRecvLen, XOS_PSZ pszTail)
{
	int rv;

	while (s_bRunning)
	{
		if ((*piRecvLen) >= (iBufLen-1))
		{
			printf("xos log recv command overflow.\r\n");
			if (s_sockAccept != XOS_INVALID_SOCKET)
			{
				XOS_CloseSocket(s_sockAccept);
				s_sockAccept = XOS_INVALID_SOCKET;
			}
			//清空缓冲区
			*piRecvLen = 0;
			return -2;
		}

		if (s_sockAccept != XOS_INVALID_SOCKET)
		{
			//有网络,判断网络超时
			if ((XOS_GetTickCount() - s_u32LastTCPRecvTime) > (10*60*1000))
			{
				//十分钟未活动,则关闭连接
				printf("xos log tcp recv time out.\r\n");
				XOS_CloseSocket(s_sockAccept);
				s_sockAccept = XOS_INVALID_SOCKET;
				return -1;
			}

			//从网络接收
			rv = XOS_TCPRecvNB(sock, pBuf+(*piRecvLen), 1);
			if (rv == -1)
			{
				//接收出错
				printf("xos log tcp recv single char nonblock failed.\r\n");
				XOS_CloseSocket(s_sockAccept);
				s_sockAccept = XOS_INVALID_SOCKET;
				return -1;
			}
			else if (rv == 1)
			{
				//更新活动时间
				s_u32LastTCPRecvTime = XOS_GetTickCount();
			}
			else //0
			{
				//没有收到
				XOS_Sleep(2);
				continue;
			}
		}
		else
		{
			if (s_get_char != NULL)
			{
				pBuf[*piRecvLen] = s_get_char();
				if (pBuf[*piRecvLen] == (char)-1)
				{
					//没接收到字符,返回
					return -1;
				}
			}
			else
			{
				XOS_Sleep(2);
				continue;
			}
		}

		if (pBuf[*piRecvLen] == '\b')
		{
			//特殊处理退格键
			if ((*piRecvLen) > 0)
			{
				XOSLog_PutChar('\b');
				(*piRecvLen) --;
			}
			XOSLog_PutChar(' ');
			XOSLog_PutChar('\b');
		}
		else
		{
			//回显字符
			XOSLog_PutChar(pBuf[*piRecvLen]);

			//全部转成小写
			/*
			if ((pBuf[*piRecvLen] >= 'A') && (pBuf[*piRecvLen] <= 'Z'))
			{
				pBuf[*piRecvLen] += ('a' - 'A');
			}
			*/

			//准备接收下一个字符
			(*piRecvLen) ++;
			pBuf[*piRecvLen] = 0;
			
			if (strstr(pBuf, pszTail) != NULL)
			{
				//接收完了,把这个置成0
				(*piRecvLen) = 0;

				return 0;
			}
		}
	}
	return -1;
}

static void XOSLog_ThreadFun(void *pParam)
{
	int rv;

	while (s_bRunning)
	{
		if ((s_sockAccept == XOS_INVALID_SOCKET) && 
			(s_sockListen != XOS_INVALID_SOCKET))
		{
			//没有连接,尝试接收客户连接
			s_sockAccept = XOS_TCPAcceptNB(s_sockListen);
			if (s_sockAccept != XOS_INVALID_SOCKET)
			{
				xos_setsocknonblock(s_sockAccept);
				s_iCommandRecvLen = 0;
				s_u32LastTCPRecvTime = XOS_GetTickCount();

				//打印欢迎语和提示符
				XOSLog_PrintWelcome();
				XOSLog_PrintNow();
				XOSLog_OnCommandIn("ver");
				XOSLog_PrintPrompt();
			}
		}

		//要处理网络输入和控制台输入两种情况
		//如果是网络的是阻塞的,非要接收完了命令才返回,如果不是网络的,则不阻塞
		rv = XOSLog_RecvCommand(s_sockAccept, s_binCommandBuffer, 
								sizeof(s_binCommandBuffer), &s_iCommandRecvLen, "\r\n");
		if (rv == 0)
		{
			//成功收到命令
			//截掉回车换行
			s_binCommandBuffer[strlen(s_binCommandBuffer)-2] = 0;
			
			//分析处理命令
			XOSLog_OnCommandIn(s_binCommandBuffer);

			//打印提示符
			XOSLog_PrintPrompt();
		}

		XOS_Sleep(2);
	}
}

//////////////////////////////////////////////////////////////////////////
//日志输出

static void xlvprintf(const char *f, va_list va, char *pszLevel)
{
	int i;
	int rv;
	int len;
	struct tm st;

	if (s_hMutex == NULL)
	{
		s_hMutex = XOSMutex_Create();
		if (s_hMutex == NULL)
		{
			return ;
		}
	}

	XOSMutex_Lock(s_hMutex);
	
	//输出时间类别
	XOS_GetLocalTime(&st);
	memset(s_binOutputBuffer, 0, sizeof(s_binOutputBuffer));
	if (pszLevel != NULL)
	{
		rv = sprintf(s_binOutputBuffer, "== %02d-%02d %02d:%02d:%02d [%08X][%s]: ",
				st.tm_mon+1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec, 
				XOSThread_GetId(), pszLevel);
	}
	else
	{
		rv = sprintf(s_binOutputBuffer, "== %02d-%02d %02d:%02d:%02d [%08X]",
				st.tm_mon+1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec,
				XOSThread_GetId());
	}
	//assert(rv > 0);
	len = strlen(s_binOutputBuffer);
	assert(len < sizeof(s_binOutputBuffer));
	for (i=0; i<len; i++)
	{
		XOSLog_PutChar(s_binOutputBuffer[i]);
	}

	//输出日志内容
	memset(s_binOutputBuffer, 0, sizeof(s_binOutputBuffer));
	rv = vsprintf(s_binOutputBuffer, f, va);
	if (rv < 0)
	{
		memset(s_binOutputBuffer, 0, sizeof(s_binOutputBuffer));
		sprintf(s_binOutputBuffer, "xlvprintf failed: %s.\r\n", f);
	}
	len = strlen(s_binOutputBuffer);
	assert(len < sizeof(s_binOutputBuffer));
	for (i=0; i<len; i++)
	{
		XOSLog_PutChar(s_binOutputBuffer[i]);
	}

	XOSMutex_Unlock(s_hMutex);

	return ;
}

//////////////////////////////////////////////////////////////////////////
//接口函数

int nprintf(const char *f, ...)
{
	int i;
	int rv;
	int len;
	va_list va;

	if (s_hMutex == NULL)
	{
		s_hMutex = XOSMutex_Create();
		if (s_hMutex == NULL)
		{
			return 0;
		}
	}

	va_start(va, f);
	XOSMutex_Lock(s_hMutex);

	memset(s_binOutputBuffer, 0, sizeof(s_binOutputBuffer));
	rv = vsprintf(s_binOutputBuffer, f, va);
	if (rv < 0)
	{
		memset(s_binOutputBuffer, 0, sizeof(s_binOutputBuffer));
		sprintf(s_binOutputBuffer, "nprintf failed: %s.\r\n", f);
	}
	len = strlen(s_binOutputBuffer);
	assert(len < sizeof(s_binOutputBuffer));
	for (i=0; i<len; i++)
	{
		XOSLog_PutChar(s_binOutputBuffer[i]);
	}

	XOSMutex_Unlock(s_hMutex);
	va_end(va);

	return 0;
}

int xprintf(const char *f, ...)
{
	va_list va;

	if (!s_bEnableXX)
	{
		return 0;
	}

	va_start(va, f);
	xlvprintf(f, va, XLL_XX);
	va_end(va);

	return 0;
}

#ifndef xlprintf
int xlprintf(const char *f, ...)
{
	int i;
	va_list va;
	char * pszLevel;
	XOS_BOOL bEnable;

	va_start(va, f);
	pszLevel = va_arg(va, char *);
	if (strcmp(pszLevel, XLL_XX) == 0)
	{
		bEnable = s_bEnableXX;
	}
	else if (strcmp(pszLevel, XLL_IM) == 0)
	{
		bEnable = s_bEnableIM;
	}
	else if (strcmp(pszLevel, XLL_EX) == 0)
	{
		bEnable = s_bEnableEX;
	}
	else if (strcmp(pszLevel, XLL_CR) == 0)
	{
		bEnable = s_bEnableCR;
	}
	else if (strcmp(pszLevel, XLL_UC) == 0)
	{
		bEnable = s_bEnableUC;
	}
	else
	{
		for (i=0; i<MAX_CUSTOM_LEVEL_ENABLE_NUM; i++)
		{
			if (!s_customLevelEnables[i].bUsed)
			{
				continue;
			}
			if (strcmp(pszLevel, s_customLevelEnables[i].szName) == 0)
			{
				bEnable = XOS_TRUE;
				break;
			}
		}
		if (i == MAX_CUSTOM_LEVEL_ENABLE_NUM)
		{
			bEnable = XOS_FALSE;
		}
	}
	if (!bEnable)
	{
		return 0;
	}

	va_start(va, f);
	xlvprintf(f, va, NULL);
	va_end(va);

	return 0;
}
#endif

void XOSLog_PrintMem(unsigned char *pMem, int iLen)
{
	nprintf("print memery at 0x%08X(%d):\r\n", pMem, iLen);
	if (iLen > 0)
	{
		int i, j;
		for (i=0; i<=(int)(iLen/16); i++)
		{
			int linelen;
			if ((iLen-(i*16)) > 16)
			{
				linelen = 16;	
			}
			else
			{
				linelen = iLen-(i*16);
			}

			for (j=0; j<linelen; j++)
			{
				nprintf("%02X ", pMem[i*16+j]);
			}
			nprintf("  ");
			for (j=0; j<linelen; j++)
			{
				nprintf("%c", pMem[i*16+j]);
			}
			nprintf("\r\n");
		}
	}
}

XOS_BOOL XOSLog_Init()
{
	return XOS_TRUE;
}

XOS_BOOL XOSLog_Open(XOS_U16 u16Port, XOSLOG_GET_CHAR get_char, 
					 XOSLOG_PUT_CHAR put_char, XOSLOG_ON_COMMAND_IN on_command_in,
					 XOSLOG_ON_HELP_IN on_help_in)
{
	s_get_char = get_char;
	s_put_char = put_char;
	s_on_command_in = on_command_in;
	s_on_help_in = on_help_in;

	if (s_hMutex == NULL)
	{
		s_hMutex = XOSMutex_Create();
		if (s_hMutex == NULL)
		{
			printf("xos log open create mutex failed.\r\n");
			return XOS_FALSE;
		}
	}

	if (u16Port != 0)
	{
		s_sockListen = XOS_TCPListen(u16Port, 1);
		if (s_sockListen == XOS_INVALID_SOCKET)
		{
			printf("xos log open tcp listen failed.\r\n");
			XOSMutex_Delete(s_hMutex);
			s_hMutex = NULL;
			return XOS_FALSE;
		}
		xos_setsocknonblock(s_sockListen);
	}

	s_bRunning = XOS_TRUE;
	s_hThreadFun = XOSThread_Create(0x1000, &XOSLog_ThreadFun, NULL);
	if (s_hThreadFun == NULL)
	{
		printf("xos log open create thread failed.\r\n");
		XOS_CloseSocket(s_sockListen);
		s_sockListen = XOS_INVALID_SOCKET;
		XOSMutex_Delete(s_hMutex);
		s_hMutex = NULL;
		return XOS_FALSE;
	}

	s_u32XOSLogOpenTime = XOS_GetTickCount();

	return XOS_TRUE;
}

XOS_BOOL XOSLog_EnableCustomLevel(XOS_CPSZ cpszName)
{
	int i;
	for (i=0; i<MAX_CUSTOM_LEVEL_ENABLE_NUM; i++)
	{
		if (!s_customLevelEnables[i].bUsed)
		{
			s_customLevelEnables[i].bUsed = XOS_TRUE;
			memset(s_customLevelEnables[i].szName, 0, MAX_CUSTOM_LEVEL_NAME_LEN);
			strncpy(s_customLevelEnables[i].szName, cpszName, MAX_CUSTOM_LEVEL_NAME_LEN-1);
			break;
		}
	}
	if (i == MAX_CUSTOM_LEVEL_ENABLE_NUM)
	{
		nprintf("XOSLog_EnableCustomLevel overflow.\r\n");
		return XOS_FALSE;
	}
	return XOS_TRUE;
}

//录像重启日志函数
static RECORD_LOG s_recordLog = NULL;
void XOSLog_RegisterRecord(RECORD_LOG recordLog)
{
	s_recordLog = recordLog;
}

void XOSLog_Record(XOS_CPSZ cpszAction, char cLogType)
{	
	if (s_recordLog != NULL)
	{
		printf("XOSLog_Record start:\r\n");
		s_recordLog(cpszAction, cLogType);
	}
}

void XOSLog_Close()
{
	s_bRunning = XOS_FALSE;
	XOSThread_Destroy(s_hThreadFun);
	s_hThreadFun = NULL;
	if (s_sockAccept != XOS_INVALID_SOCKET)
	{
		XOS_CloseSocket(s_sockAccept);
		s_sockAccept = XOS_INVALID_SOCKET;
	}
	XOS_CloseSocket(s_sockListen);
	s_sockListen = XOS_INVALID_SOCKET;
	XOSMutex_Delete(s_hMutex);
	s_hMutex = NULL;
	return ;
}

