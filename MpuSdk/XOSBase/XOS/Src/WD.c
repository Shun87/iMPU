/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: WD.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-4-6 9:55:53
 *  修正备注: 
 *  
 */

#include "../XOS.h"

#ifdef _WATCH_DOG

typedef struct _TWD {
	char bUsed;
	char ThreadName[64];
	int WDCnt;
	int MaxWDCnt;
	int iTimeOut;
	int idx;
} TWD;

#ifdef WIN32
static __declspec( thread ) HANDLE tg_pWD = NULL;
#elif _CREARO_NDK
#else //LINUX
static pthread_key_t s_key;
#endif

static TWD *s_pWDs = NULL; 
static int s_iMaxThreadNum = 0;
static void *s_csWDs = NULL;
static int s_bInited = 0;
static void (*s_clear_dog)() = NULL;

void WD_Deamon(void)
{
	int i;
	int bClearDog;

	WD_JOIN("WD_Deamon", 60);
	TCS_JOIN(0);
	TCS_PUSH("WD_Deamon");

	while(s_bInited)
	{
		TCS_PUSH("WD_CLEAR");
		WD_CLEAR;
		TCS_POP;
		TCS_COUNT;
		
		TCS_PUSH("Sleep");
		XOS_Sleep(10*1000);
		TCS_POP;

		bClearDog = 1;
		TCS_PUSH("XOSMutex_Lock");
		XOSMutex_Lock(s_csWDs);
		TCS_POP;
		for (i=0; i<s_iMaxThreadNum; i++)
		{
			if (s_pWDs[i].bUsed == 0)
			{
				continue;
			}

			//增加计数
			s_pWDs[i].WDCnt += 10;
			if (s_pWDs[i].WDCnt > s_pWDs[i].MaxWDCnt)
			{
				s_pWDs[i].MaxWDCnt = s_pWDs[i].WDCnt;
			}

			//判断是否超时
			if (s_pWDs[i].WDCnt > s_pWDs[i].iTimeOut)
			{
				char *psz;
				psz = TCS_GetBlockString(i);
				printf("WD_Deamon iThreadNum: %d TimeOut!\r\n%s\r\n", i, psz);
				bClearDog = 0;
				XOSLog_Record(psz, USER_LOG_TYPE_REBOOT);
			}
		}
		XOSMutex_Unlock(s_csWDs);

		if ((bClearDog == 1) && (s_clear_dog != NULL))
		{
			s_clear_dog();
		}
	}

	WD_LEAVE;
	TCS_LEAVE;
	return ;
}

//获取各个线程自己的pWD指针
static void *GetpWD()
{
	#ifdef WIN32
		return tg_pWD;
	#elif _CREARO_NDK
		TEnv *pEnv;
		pEnv = (TEnv *)TSK_getenv(TSK_self());
		if (pEnv == NULL)
		{
			return NULL;
		}
		else
		{
			return pEnv->pWD;
		}
	#else //LINUX
		return pthread_getspecific(s_key);
	#endif
}

static void SetpWD(void *pWD)
{
	#ifdef WIN32
		tg_pWD = pWD;
	#elif _CREARO_NDK
		TEnv *pEnv;
		pEnv = (TEnv *)TSK_getenv(TSK_self());
		if (pEnv == NULL)
		{
			pEnv = (TEnv *)malloc(sizeof(TEnv));
			if (pEnv == NULL)
			{
				return ;
			}
			memset(pEnv, 0, sizeof(TEnv));
			pEnv->pWD = pWD;
		    TSK_setenv(TSK_self(), pEnv);
		}
		else
		{
			pEnv->pWD = pWD;	   
		}
	#else //LINUX
		if (pthread_setspecific(s_key, pWD) != 0)
		{
			TWD *pTWD = (TWD *)pWD;
			s_pWDs[pTWD->idx].bUsed = 0;
			printf("pthread_setspecific failed\r\n");
		}
	#endif
	return ;
}

int WD_Init(int iMaxThreadNum, void (*clear_dog)())
{
	s_pWDs = (TWD *)malloc(sizeof(TWD)*iMaxThreadNum);
	if (s_pWDs == NULL)
	{
		return -1;
	}
	memset(s_pWDs, 0, sizeof(TWD)*iMaxThreadNum);
	s_csWDs = XOSMutex_Create();
	if (s_csWDs == NULL)
	{
		free(s_pWDs);
		return -1;
	}

	#ifdef WIN32
	#elif _CREARO_NDK
	#else //LINUX
	if (pthread_key_create(&s_key, NULL) != 0)
	{
		XOSMutex_Delete(s_csWDs);
		free (s_pWDs);
		printf("create pthread key failed\r\n");
		return -1;
	}
	#endif

	s_iMaxThreadNum = iMaxThreadNum;
	s_clear_dog = clear_dog;
	s_bInited = 1;

	if (XOSThread_Create(0x400, &WD_Deamon, 0) == NULL)
	{
		XOSMutex_Delete(s_csWDs);
		free(s_pWDs);
		return -1;
	}

	return 0;
}

void WD_Join(const char *pszName, int iTimeOut)
{
	int i;
	void *pWD = NULL;
	
	if (s_bInited == 0) 
	{
		return ;
	}

	pWD = GetpWD();
	if (pWD != NULL)
	{
		//已经Join过了,返回
		return ;
	}
	
	XOSMutex_Lock(s_csWDs);
	for (i=0; i<s_iMaxThreadNum; i++)
	{
		if (s_pWDs[i].bUsed == 0)
		{
			break;
		}
	}
	if (i == s_iMaxThreadNum)
	{
		//用光光了		
		XOSMutex_Unlock(s_csWDs);
		return ;
	}
	memcpy(s_pWDs[i].ThreadName, pszName, 63);
	s_pWDs[i].WDCnt = 0;
	s_pWDs[i].MaxWDCnt = 0;
	s_pWDs[i].iTimeOut = iTimeOut;
	s_pWDs[i].idx = i;
	s_pWDs[i].bUsed = 1;
	XOSMutex_Unlock(s_csWDs);

	pWD = &s_pWDs[i];
	SetpWD(pWD);

	return ;
}

void WD_SetTimeOut(int iTimeOut)
{
	TWD *pWD;

	if (s_bInited == 0) 
	{
		return ;
	}

	pWD = (TWD *)GetpWD();
	if (pWD == NULL)
	{
		//这个线程还没有Join
		return ;
	}

	pWD->iTimeOut = iTimeOut;
	
	return ;
}

void WD_Leave()
{
	TWD *pWD;
	
	if (s_bInited == 0) 
	{
		return ;
	}
	
    pWD = (TWD *)GetpWD();
	if (pWD == NULL)
	{
		//这个线程还没有Join
		return ;
	}

	pWD->bUsed = 0;
	
	SetpWD(NULL);

	return ;
}

void WD_Clear()
{
	TWD *pWD;
	
	if (s_bInited == 0) 
	{
		return ;
	}
    
    pWD = (TWD *)GetpWD();
	if (pWD == NULL)
	{
		//这个线程还没有Join
		return ;
	}

	pWD->WDCnt = 0;
	
	return ;
}

void WD_Report(int (*pPrn)(const char *,...))
{
	int i;

	pPrn("====WD Report Begin====\r\n");
	pPrn("s_bInited:%d\r\n", s_bInited);
	pPrn("s_csWDs:%08X\r\n", s_csWDs);
	pPrn("s_clear_dog:%08X\r\n", s_clear_dog);
	pPrn("s_pWDs:%08X,%d\r\n", s_pWDs, s_iMaxThreadNum);

	pPrn("\tNo\tbUsed\tThreadName      \tWDCnt\tMaxWDCnt\tiTimeOut(s)\r\n");
	for (i=0; i<s_iMaxThreadNum; i++)
	{
		if (s_pWDs[i].bUsed == 0)
		{
			continue;
		}
		pPrn("\t%d\t%d\t%-16s\t%d\t%d\t\t%d\r\n",
			i,
			s_pWDs[i].bUsed,
			s_pWDs[i].ThreadName,
			s_pWDs[i].WDCnt,
			s_pWDs[i].MaxWDCnt,
			s_pWDs[i].iTimeOut
			);

	}
	pPrn("====WD Report End====\r\n");
	return ;
}

#endif //#ifdef _WATCH_DOG
