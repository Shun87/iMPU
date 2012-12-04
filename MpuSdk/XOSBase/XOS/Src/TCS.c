/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: TCS.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2005-09-08 13:22:07
 *  修正备注: 
 *  
 */

#include "../XOS.h"

#ifdef _TDEBUG

typedef struct _TFunctionCall {
	char szFunctionName[64];
	char szFileName[128];
	int iLineNo;
	unsigned int uiCallTime;
} TFunctionCall;

typedef struct _TThread {
	int idx;
	unsigned int uiLoopNum;
	unsigned int uiCountTime;
	int iUserData;
	int iFunctionCallNum;
	XOS_U32 u32ThreadId;
	TFunctionCall FunctionCall[16];
}TThread;

#ifdef WIN32
static __declspec( thread ) void *tg_pThread = 0;
#elif _CREARO_NDK
#else //LINUX
static pthread_key_t s_key;
#endif

static TThread **s_pThreads = 0;
static int s_iMaxThreadNum = 0;
static int s_bInited = 0;
static void* s_cspThreads;

#define BLOCK_LOG_MAX_LEN		 1024
static char s_szBlockString[BLOCK_LOG_MAX_LEN+1];

//获取各个线程自己的pWD指针
static void *GetpThread()
{
	#ifdef WIN32
		return tg_pThread;
	#elif _CREARO_NDK
		TEnv *pEnv;
		pEnv = (TEnv *)TSK_getenv(TSK_self());
		if (pEnv == NULL)
		{
			return NULL;
		}
		else
		{
			return pEnv->pThread;
		}
	#else //LINUX
		return pthread_getspecific(s_key);
	#endif
}

static void SetpThread(void *pThread)
{
	#ifdef WIN32
		tg_pThread = pThread;
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
			pEnv->pThread = pThread;
		    TSK_setenv(TSK_self(), pEnv);
		}
		else
		{
			pEnv->pThread = pThread;		
		}
	#else //LINUX
		if (pthread_setspecific(s_key, pThread) != 0)
		{
			TThread *pTThread = (TThread *)pThread;
		    s_pThreads[pTThread->idx] = 0;
			free(pThread);
		    printf("pthread_setspecific failed\r\n");
		    return ;
		}
	#endif
	return ;
}

int TCS_Init(int iMaxThreadNum)
{
	s_iMaxThreadNum = iMaxThreadNum;

	s_cspThreads = XOSMutex_Create();

	s_pThreads = (TThread **)malloc(sizeof(TThread *)*s_iMaxThreadNum);
	if (s_pThreads == 0)
	{
		XOSMutex_Delete(s_cspThreads);
		return -1;
	}
	memset(s_pThreads, 0, sizeof(TThread *)*s_iMaxThreadNum);

	#ifndef WIN32
       #ifndef _CREARO_NDK
	     if (pthread_key_create(&s_key, NULL) != 0)
	    {
		   XOSMutex_Delete(s_cspThreads);
		   free (s_pThreads);
		   printf("TCS_Init\r\n");
		   printf("create pthread key failed\r\n");
		   return -1;
	     }
       #endif
    #endif
	s_bInited = 1;
	return 0;
}

void TCS_Join(int iUserData)
{
	int i;
    void *pThread;

	if (s_bInited == 0)
	{
		return ;
	}

    pThread = GetpThread();
	if (pThread != NULL)
	{
		//这个线程已经Join过了
		return ;
	}

	XOSMutex_Lock(s_cspThreads);
	for (i=0; i<s_iMaxThreadNum; i++)
	{
		if (s_pThreads[i] == NULL)
		{
			break;
		}
	}
	if (i == s_iMaxThreadNum)
	{
		//支持的线程已经满了
		XOSMutex_Unlock(s_cspThreads);
		return ;
	}

	s_pThreads[i] = (TThread*)malloc(sizeof(TThread));
	if (s_pThreads[i] == NULL)
	{
		XOSMutex_Unlock(s_cspThreads);
		return ;
	}
	memset(s_pThreads[i], 0, sizeof(TThread));
	XOSMutex_Unlock(s_cspThreads);

	s_pThreads[i]->iUserData = iUserData;
	s_pThreads[i]->idx = i;
	s_pThreads[i]->u32ThreadId = XOSThread_GetId();

	pThread = (void *)s_pThreads[i]; 
	SetpThread(pThread);
	
	return ;
}

void TCS_Leave()
{
	TThread *pThread;

	if (s_bInited == 0)
	{
		return ;
	}
	
	pThread = (TThread*)GetpThread();
	if (pThread == NULL)
	{
		//这个线程还没有Join
		return ;
	}

	//释放使用的对象
	XOSMutex_Lock(s_cspThreads);
	s_pThreads[pThread->idx] = 0;
	XOSMutex_Unlock(s_cspThreads);

	SetpThread(NULL);
	
	free(pThread);
}

void TCS_Push(const char *pszFunctionName, const char *pszFileName, int iLineNo)
{
	TThread *pThread;

	if (s_bInited == 0)
	{
		return ;
	}
   
    pThread = (TThread*)GetpThread();
	if (pThread == NULL)
	{
		TCS_Join(0);
		pThread = GetpThread();
		if (pThread == NULL)
		{
			return ;
		}
	}
	
	if (pThread->iFunctionCallNum >=  16)
	{
		return ;
	}
	memcpy(pThread->FunctionCall[pThread->iFunctionCallNum].szFunctionName, pszFunctionName, 63);
	memcpy(pThread->FunctionCall[pThread->iFunctionCallNum].szFileName, pszFileName, 127);
	pThread->FunctionCall[pThread->iFunctionCallNum].iLineNo = iLineNo;
	pThread->FunctionCall[pThread->iFunctionCallNum].uiCallTime = XOS_GetTickCount();
	pThread->iFunctionCallNum ++;
	return ;
}

void TCS_Pop()
{
	TThread *pThread;

	if (s_bInited == 0)
	{
		return ;
	}

	pThread = (TThread*)GetpThread();
	if (pThread == NULL)
	{
		TCS_Join(0);
		pThread = (TThread*)GetpThread();
		if (pThread == NULL)
		{
			return ;
		}
	}
	
	if (pThread->iFunctionCallNum <=  0)
	{
		return ;
	}

	pThread->iFunctionCallNum --;
	return ;
}

void TCS_Count()
{
	TThread *pThread;

	if (s_bInited == 0)
	{
		return ;
	}

	pThread = (TThread*)GetpThread();
	if (pThread == NULL)
	{
		TCS_Join(0);
		pThread = (TThread*)GetpThread();
		if (pThread == NULL)
		{
			return ;
		}
	}
	pThread->uiLoopNum ++;
	pThread->uiCountTime = XOS_GetTickCount();
	return ;
}

void TCS_Close()
{
	int i;

	if (s_bInited == 0)
	{
		return ;
	}
	
	s_bInited = 0;
	XOSMutex_Delete(s_cspThreads);
	for (i=0; i<s_iMaxThreadNum; i++)
	{
		if (s_pThreads[i] == NULL)
		{
			continue;
		}
		free(s_pThreads[i]);
	}

	#ifndef WIN32
         #ifndef _CREARO_NDK
	       pthread_key_delete(s_key);
         #endif
	#endif

	return ;
}

char *TCS_GetBlockString(int iThreadNum)
{
	int j;
	char *psz = NULL;
	memset(s_szBlockString, 0, sizeof(s_szBlockString));
	strcat(s_szBlockString, "程序阻塞重启:\r\n");
	psz = malloc(BLOCK_LOG_MAX_LEN+1);
	if (psz == NULL)
	{
		printf("malloc failed size:%d.\r\n", BLOCK_LOG_MAX_LEN+1);
		return s_szBlockString;
	}
	
	for (j=0; j<s_pThreads[iThreadNum]->iFunctionCallNum; j++)
	{
		memset(psz, 0, BLOCK_LOG_MAX_LEN+1);
		sprintf(psz, "0x%08X %6d %s\t%s\r\n", 
				s_pThreads[iThreadNum]->FunctionCall[j].uiCallTime,
				s_pThreads[iThreadNum]->FunctionCall[j].iLineNo,
				s_pThreads[iThreadNum]->FunctionCall[j].szFunctionName,
				s_pThreads[iThreadNum]->FunctionCall[j].szFileName);
		if (strlen(psz) < BLOCK_LOG_MAX_LEN)
		{
			if (strlen(s_szBlockString) + strlen(psz) < BLOCK_LOG_MAX_LEN)
			{
				strcat(s_szBlockString, psz);
			}
			else
			{
				if (strlen(s_szBlockString) < BLOCK_LOG_MAX_LEN)
				{
					//截段处理
					strncat(s_szBlockString, psz,  BLOCK_LOG_MAX_LEN - strlen(s_szBlockString));
				}
				break;
			}
		}
		else
		{
			if (strlen(s_szBlockString) < BLOCK_LOG_MAX_LEN)
			{
				strncat(s_szBlockString, psz,  BLOCK_LOG_MAX_LEN - strlen(s_szBlockString));
			}
			printf("TCS_GetBlockString psz too long, impossible!!\r\n");
			break;
		}
	}
	free(psz);
	psz = NULL;
	return s_szBlockString;
}

void TCS_Report(int (*pPrn)(const char *,...))
{
	int i, j;

	pPrn("====TCS Report Begin====\r\n");
	pPrn("bInited: %d\r\n", s_bInited);
	pPrn("hpThreadsMutex: 0x%08X\r\n", &s_cspThreads);
	pPrn("Now: 0x%08X", XOS_GetTickCount());
	if (s_bInited == 0)
	{
		return ;
	}
	TCS_PUSH("XOSMutex_Lock");
	XOSMutex_Lock(s_cspThreads);
	TCS_POP;
	
	for (i=0; i<s_iMaxThreadNum; i++)
	{
		if (s_pThreads[i] == 0)
		{
			continue;
		}
		pPrn("------------------------------------------\r\n", i);
		pPrn("ThreadNo: %d\r\n", i);
		pPrn("uiLoopNum: %d\r\n", s_pThreads[i]->uiLoopNum);
		pPrn("uiCountTime: 0x%08X\r\n", s_pThreads[i]->uiCountTime);
		pPrn("iUserData: %d\r\n", s_pThreads[i]->iUserData);
		pPrn("u32ThreadId: 0x%08X\r\n", s_pThreads[i]->u32ThreadId);
		for (j=0; j<s_pThreads[i]->iFunctionCallNum; j++)
		{
			pPrn("0x%08X %6d %s\t%s\r\n", s_pThreads[i]->FunctionCall[j].uiCallTime,
									s_pThreads[i]->FunctionCall[j].iLineNo, 
									s_pThreads[i]->FunctionCall[j].szFunctionName,
									s_pThreads[i]->FunctionCall[j].szFileName);
		}
	}
	XOSMutex_Unlock(s_cspThreads);
	pPrn("====TCS Report End====\r\n");
}

#endif //#ifdef _TDEBUG
