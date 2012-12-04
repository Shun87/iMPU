/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: CycleStat.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-11-12 15:11:40
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../CycleStat.h"

void CycleStat_Init(TCycleStat *pCycleStat, int iStatCycle)
{
	int i;
	pCycleStat->iStatCycle = iStatCycle;
	pCycleStat->uiLastStatTime = XOS_GetTickCount();
	for (i=0; i<MAX_CYCLESTAT_COUNTER_NUM; i++)
	{
		pCycleStat->uiCounters[i] = 0;
		pCycleStat->uiLastCounters[i] = 0;
		pCycleStat->uiCounterNum[i] = 0;
		pCycleStat->uiLastCounterNum[i] = 0;
		pCycleStat->uiMaxCounters[i] = 0;
		pCycleStat->uiLastMaxCounters[i] = 0;
	}
	return ;
}

void CycleStat_Count(TCycleStat *pCycleStat, int iIdx, int iCounter)
{
	assert(iIdx < MAX_CYCLESTAT_COUNTER_NUM);
	pCycleStat->uiCounters[iIdx] += iCounter;
	pCycleStat->uiCounterNum[iIdx] ++;
	if (iCounter > (int)pCycleStat->uiMaxCounters[iIdx])
	{
		pCycleStat->uiMaxCounters[iIdx] = iCounter;
	}
	return ;
}

int CycleStat_Check(TCycleStat *pCycleStat)
{
	int i;
	unsigned int uiTickCount;
	uiTickCount = XOS_GetTickCount();
	if ((int)(uiTickCount - pCycleStat->uiLastStatTime) >= (pCycleStat->iStatCycle*1000))
	{
		pCycleStat->uiLastStatTime = uiTickCount;
		for (i=0; i<MAX_CYCLESTAT_COUNTER_NUM; i++)
		{
			pCycleStat->uiLastCounters[i] = pCycleStat->uiCounters[i];
			pCycleStat->uiCounters[i] = 0;
			pCycleStat->uiLastCounterNum[i] = pCycleStat->uiCounterNum[i];
			pCycleStat->uiCounterNum[i] = 0;
			pCycleStat->uiLastMaxCounters[i] = pCycleStat->uiMaxCounters[i];
			pCycleStat->uiMaxCounters[i] = 0;
		}
		return CYCLESTAT_E_OK;
	}
	else
	{
		return CYCLESTAT_E_WOULDBLOCK;
	}
}

int CycleStat_GetTotal(TCycleStat *pCycleStat, int iIdx)
{
	assert(iIdx < MAX_CYCLESTAT_COUNTER_NUM);
	return pCycleStat->uiLastCounters[iIdx];
}

int CycleStat_GetRate(TCycleStat *pCycleStat, int iIdx)
{
	assert(iIdx < MAX_CYCLESTAT_COUNTER_NUM);
	return pCycleStat->uiLastCounters[iIdx]/pCycleStat->iStatCycle;
}

int CycleStat_GetAverage(TCycleStat *pCycleStat, int iIdx)
{
	assert(iIdx < MAX_CYCLESTAT_COUNTER_NUM);
	if (pCycleStat->uiLastCounterNum[iIdx] == 0)
	{
		return -1;
	}
	return pCycleStat->uiLastCounters[iIdx]/pCycleStat->uiLastCounterNum[iIdx];
}

int CycleStat_GetMax(TCycleStat *pCycleStat, int iIdx)
{
	assert(iIdx < MAX_CYCLESTAT_COUNTER_NUM);
	return pCycleStat->uiLastMaxCounters[iIdx];
}
