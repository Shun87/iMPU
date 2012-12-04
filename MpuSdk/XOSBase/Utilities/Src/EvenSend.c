/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: EvenSend.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-11-12 10:27:18
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../EvenSend.h"

void EvenSend_Init(TEvenSend *pEvenSend, int iTargetBitRate)
{
	pEvenSend->iTargetBitRate = iTargetBitRate;
	pEvenSend->bTargetBitRateChanged = 0;
	pEvenSend->bHungry = 1;		//一开始就处于饥饿状态
	pEvenSend->uiBeginTIme = 0;	//饥饿状态下探测时就会重新开始计算
	pEvenSend->iSentBytes = 0;
}

int EvenSend_Probe(TEvenSend *pEvenSend, int iNewBytes)
{
	int iCanSendBytes;			//能够发送的字节数
	char bEvenSendReset = 0;	//均匀发送是否重置

	//判断是否处于饥饿状态
	if (pEvenSend->bHungry)
	{
		pEvenSend->bHungry = 0;	//复位饥饿标记,下次如果没数据发,还会置成饥饿的
		bEvenSendReset = 1;			//置重置标记,待会不按时间计算能够发送的字节数
	}
	if (pEvenSend->bTargetBitRateChanged)
	{
		pEvenSend->bTargetBitRateChanged = 0;
		bEvenSendReset = 1;			//置重置标记,待会不按时间计算能够发送的字节数
	}

	//如果没有重置,则按照开始时间计算能够发送的字节数
	if (!bEvenSendReset)
	{
		double dDeltaTime = (XOS_GetTickCount() - pEvenSend->uiBeginTIme)/1000.0;
		int iTotalCanSentBytes = (int)(dDeltaTime*pEvenSend->iTargetBitRate*1024.0/8);
		iCanSendBytes = iTotalCanSentBytes - pEvenSend->iSentBytes;
	}
	else
	{
		pEvenSend->uiBeginTIme = XOS_GetTickCount();
		pEvenSend->iSentBytes = 0;	//已经发送字节数置0
		iCanSendBytes = 0;			//置成0,表示本次不发,下次再发,
									//考虑到Timer函数调用得非常频繁,这样不会有问题
	}

	if (iCanSendBytes <= 0)
	{
		return EVENSEND_E_WOULDBLOCK;
	}
	else
	{
		//就算做发送过数据了,如果上层没有发送,则会设置饥饿状态,
		//饥饿状态会重新计算,所有就算没发,这里增加了也无所谓
		//探测不发送的话,可以通过iNewBytes传入0处理
		pEvenSend->iSentBytes += iNewBytes;
		return EVENSEND_E_OK;
	}
}

void EvenSend_SetHungry(TEvenSend *pEvenSend)
{
	pEvenSend->bHungry = 1;
}

void EvenSend_SetTargetBitRate(TEvenSend *pEvenSend, int iTargetBitRate)
{
	pEvenSend->iTargetBitRate = iTargetBitRate;
	pEvenSend->bTargetBitRateChanged = 1;
}
