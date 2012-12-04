/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSThread_Funs_LINUX.c
 *  文件标识: 
 *  摘    要: linux平台下多线程相关函数的实现	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-16 18:40:25
 *  修正备注: 
 *  
 */
 
#include "../XOS.h"

#include <pthread.h>
#include <semaphore.h>

static XOS_HANDLE s_hGetThread = NULL;
static unsigned long s_ul_jiffies = 0;
static int s_fd_uptime = -1;

static unsigned long long GetTickCount64()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (unsigned long long)(tv.tv_sec*1000+tv.tv_usec/1000);
}

static void getjiffies()
{
	int rv;
	unsigned long ul_jiffies;
	unsigned long sec, usec;
	char buf[64];
	if (s_fd_uptime == -1)
	{
		s_fd_uptime = open("/proc/uptime", O_RDONLY);
		if (s_fd_uptime == -1)
		{
			printf("getjiffies open failed.\r\n");
			return ;
		}
	}
	memset(buf,0,64);
	lseek(s_fd_uptime, SEEK_SET, 0);
	rv = read(s_fd_uptime, buf, 63);
	if (rv <= 0)
	{
		printf("getjiffies read failed.\r\n");
		return ;
	}

	rv = sscanf(buf,"%u.%u", &sec, &usec);
	if (rv != 2)
	{
		printf("getjiffies sscanf failed.\r\n");
		return ;
	}

	ul_jiffies = (sec * 1000 + usec*10);

	/*
	//先不多此一举了
	if ((long)(ul_jiffies - s_ul_jiffies)) < 0)
	{
		printf("getjiffies invalid.\r\n");
		return ;
	}
	*/

	s_ul_jiffies = ul_jiffies;

	return ;
}

//CPU使用率参数
typedef struct _t_cpu_rate 
{
	XOS_U32 user;		//用户态的CPU时间
    XOS_U32 nice;		//低优先级的用户模式
	XOS_U32 system;		//内核时间
	XOS_U32 idle;		//空闲的处理器时间(除硬盘IO等待时间以外其它等待时间)
	XOS_U32 iowait;		//硬盘IO等待时间
	XOS_U32 irq;		//硬中断时间
	XOS_U32 softirq;	//软中断时间
}t_cpu_rate;
#define PROC_STAT_MAX_LEN	256
static t_cpu_rate s_t_cpu_rate_minus;
static t_cpu_rate s_t_cpu_rate_org;
static unsigned long s_UpdateTimeCPURateStat;
static void get_cpu_rate_para(t_cpu_rate *pt_cpu_rate)
{
	FILE *fp = NULL;
	char cpu[21];
	char *pszText = NULL;
	fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
		printf("get_cpu_rate_para fopen:%s failed.\r\n", "/proc/stat");
		return;
	}
	
	pszText = malloc(PROC_STAT_MAX_LEN+1);
	memset(pszText, 0, PROC_STAT_MAX_LEN+1);
	while (fgets(pszText, PROC_STAT_MAX_LEN, fp))
	{
		if (strstr(pszText, "cpu"))
		{
			//cpu是总的cpu统计,cpu0 cpu1...
			sscanf(pszText, "%s %d %d %d %d %d %d %d", cpu, 
					&pt_cpu_rate->user, &pt_cpu_rate->nice, &pt_cpu_rate->system,
					&pt_cpu_rate->idle, &pt_cpu_rate->iowait, &pt_cpu_rate->irq, 
					&pt_cpu_rate->softirq);
			break;
		}
	}
	fclose(fp);
}

static void GetThread()
{
	while (1)
	{
		getjiffies();
		XOS_Sleep(5);

		if (XOS_GetTickCount() - s_UpdateTimeCPURateStat > 2000)
		{
			t_cpu_rate t_cpu_rate_now;
			s_UpdateTimeCPURateStat = XOS_GetTickCount();
			get_cpu_rate_para(&t_cpu_rate_now);
			s_t_cpu_rate_minus.user    = t_cpu_rate_now.user -    s_t_cpu_rate_org.user;
			s_t_cpu_rate_minus.nice    = t_cpu_rate_now.nice -    s_t_cpu_rate_org.nice;
			s_t_cpu_rate_minus.system  = t_cpu_rate_now.system -  s_t_cpu_rate_org.system;
			s_t_cpu_rate_minus.idle    = t_cpu_rate_now.idle -	  s_t_cpu_rate_org.idle;
			s_t_cpu_rate_minus.iowait  = t_cpu_rate_now.iowait -  s_t_cpu_rate_org.iowait;
			s_t_cpu_rate_minus.irq     = t_cpu_rate_now.irq -     s_t_cpu_rate_org.irq;
			s_t_cpu_rate_minus.softirq = t_cpu_rate_now.softirq - s_t_cpu_rate_org.softirq;
			memcpy(&s_t_cpu_rate_org, &t_cpu_rate_now, sizeof(t_cpu_rate));
		}
	}
}

XOS_U32 XOS_GetTickCount()
{
	/*
	if (s_hGetThread == NULL)
	{
		memset(&s_t_cpu_rate_org, 0, sizeof(t_cpu_rate));
		getjiffies();
		s_hGetThread = XOSThread_Create(0, &GetThread, NULL);
	}
	//return 10*(XOS_U32)times(NULL);
	return s_ul_jiffies;
	*/
	unsigned long long ullTick = GetTickCount64();
	return (unsigned long)(ullTick & 0xFFFFFFFF);
}

void XOS_Sleep(XOS_U32 u32)
{
	XOS_U32 u32Sec = 0;
	XOS_U32 u32MSec = 0;
	if (u32 == XOS_INFINITE)
	{
		u32Sec = XOS_INFINITE;
	}
	else
	{
		u32Sec = u32/1000;
		u32MSec = u32%1000;
	}
	if (u32Sec > 0)
	{
		sleep(u32Sec);
	}
	if (u32MSec > 0)
	{
		usleep((u32MSec)*1000);
	}
}

void XOS_GetCPURate(char CPURate[7])
{
	int total = 0;
    total = (s_t_cpu_rate_minus.user + s_t_cpu_rate_minus.nice + s_t_cpu_rate_minus.system + 
			 s_t_cpu_rate_minus.idle + s_t_cpu_rate_minus.iowait + s_t_cpu_rate_minus.irq + 
			 s_t_cpu_rate_minus.softirq);
    CPURate[0] = (s_t_cpu_rate_minus.user*100) / total;
    CPURate[1] = (s_t_cpu_rate_minus.nice*100) / total;
    CPURate[2] = (s_t_cpu_rate_minus.system*100) / total;
	CPURate[3] = (s_t_cpu_rate_minus.idle*100) / total;
	CPURate[4] = (s_t_cpu_rate_minus.iowait*100) / total;
	CPURate[5] = (s_t_cpu_rate_minus.irq*100) / total;
	CPURate[6] = (s_t_cpu_rate_minus.softirq*100) / total;		
}

/************************************************************************/
/* thread                                                               */
/************************************************************************/

XOS_HANDLE XOSThread_Create(int stacksize, void *pfun, void *pParam)
{	
	pthread_t   thr_id; 
	if (pthread_create(&thr_id, NULL, (void *(*)(void *))pfun, pParam) != 0)
	{
		printf("XOSThread_Create failed.\r\n");
		return NULL;
	}
	return (XOS_HANDLE)1;
}
void XOSThread_Destroy(XOS_HANDLE hThread)
{
	return;
}
int XOSThread_Setpri(XOS_HANDLE hThread, int pri)
{
	return 0;
}
int XOSThread_Getpri(XOS_HANDLE hThread)
{
	return 0;
}
XOS_U32 XOSThread_GetId()
{
	return (XOS_U32)pthread_self();
}

/************************************************************************/
/* mutex                                                                */
/************************************************************************/

XOS_HANDLE XOSMutex_Create()
{
	pthread_mutex_t *hMutex;
	hMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (hMutex == NULL)
	{
		printf("XOSMutex_Create malloc failed.\r\n");
		return NULL;
	}
	pthread_mutex_init(hMutex, NULL);
	return hMutex;
}
void XOSMutex_Delete(XOS_HANDLE hMutex)
{
	pthread_mutex_destroy((pthread_mutex_t *)hMutex);
	free(hMutex);
}
void XOSMutex_Lock(XOS_HANDLE hMutex)
{
	pthread_mutex_lock((pthread_mutex_t *)hMutex);
}
void XOSMutex_Unlock(XOS_HANDLE hMutex)
{
	pthread_mutex_unlock((pthread_mutex_t *)hMutex);
}

/************************************************************************/
/* sem                                                                  */
/************************************************************************/

XOS_HANDLE XOSSem_Create(int iInitCount)
{
	sem_t *hSem;
	hSem = (sem_t *)malloc(sizeof(sem_t));
	if (hSem == NULL)
	{
		printf("XOSSem_Create malloc failed.\r\n");
		return NULL;
	}
	sem_init(hSem, 0, iInitCount);
	return hSem;
}
void XOSSem_Delete(XOS_HANDLE hSem)
{
	sem_destroy((sem_t *)hSem);
	free(hSem);
}
void XOSSem_Post(XOS_HANDLE hSem)
{
	sem_post((sem_t *)hSem);
}
void XOSSem_iPost(XOS_HANDLE hSem)
{
	sem_post((sem_t *)hSem);
}
void XOSSem_Pend(XOS_HANDLE hSem)
{
	sem_wait((sem_t *)hSem);
}
XOS_BOOL XOSSem_Pendt(XOS_HANDLE hSem, int iTimeOut)
{
	int trycnt=0;
	while (1)
	{
		usleep(10*1000);
		if(sem_trywait((sem_t *)hSem) == 0)
		{
			return XOS_TRUE;
		}
		else
		{
			trycnt++;
			if (trycnt>(iTimeOut/20))
			{
				return XOS_FALSE;
			}
		}
	}
}
