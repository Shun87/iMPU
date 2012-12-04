/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: CycleStat.h
 *  文件标识: 
 *  摘    要: 周期统计器,可同时支持多个统计量的统计,但是统计周期需要相同
 *			  最大统计计数为4G,能够计算如下统计量
 *				1.统计周期内统计量的总数
 *				2.统计周期内统计量的速率,单位:每秒
 *				3.统计周期内统计量的均值
 *				4.统计周期内统计量的最大值
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-11-12 14:46:20
 *  修正备注: 
 *  
 */

#ifndef __CYCLESTAT_H__
#define __CYCLESTAT_H__

//返回错误码定义
#define CYCLESTAT_E_OK				0	//成功
#define CYCLESTAT_E_WOULDBLOCK		1	//暂时不可操作

#define MAX_CYCLESTAT_COUNTER_NUM	4
typedef struct _TCycleStat
{
	int iStatCycle;												//统计周期,单位秒
	unsigned int uiLastStatTime;								//上次统计时间
	unsigned int uiCounters[MAX_CYCLESTAT_COUNTER_NUM];			//计数值数组
	unsigned int uiLastCounters[MAX_CYCLESTAT_COUNTER_NUM];		//上次的计数值数组
	unsigned int uiCounterNum[MAX_CYCLESTAT_COUNTER_NUM];		//计数数目
	unsigned int uiLastCounterNum[MAX_CYCLESTAT_COUNTER_NUM];	//上次的计数数目
	int uiMaxCounters[MAX_CYCLESTAT_COUNTER_NUM];				//最大计数值
	int uiLastMaxCounters[MAX_CYCLESTAT_COUNTER_NUM];			//上次的最大计数值
} TCycleStat;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 初始化对象
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *      int iStatCycle [IN]:			统计周期,单位秒
 */
void CycleStat_Init(TCycleStat *pCycleStat, int iStatCycle);

/*  功能描述: 增加计数
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *      int iIdx [IN]:					计数器索引
 *      int iCounter [IN]:				增加的计数
 */
void CycleStat_Count(TCycleStat *pCycleStat, int iIdx, int iCounter);

/*  功能描述: 检查时间是否到了
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *  返回值: 错误码,取值见前面的宏定义,含义如下:
 *				CYCLESTAT_E_OK			成功,到时间了
 *				CYCLESTAT_E_WOULDBLOCK	还没有到时间
 *  备注: 上层需要不停的调用此函数检查,否则错过统计周期可能造成统计结果不准
 */
int CycleStat_Check(TCycleStat *pCycleStat);

/*  功能描述: 获取总数
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *      int iIdx [IN]:					获取的统计计数器的索引
 *  返回值: 获取到的总数
 *  备注: Check函数返回成功之后,调用此函数即可获得上一统计周期内的统计值
 */
int CycleStat_GetTotal(TCycleStat *pCycleStat, int iIdx);

/*  功能描述: 获取速率
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *      int iIdx [IN]:					获取的统计计数器的索引
 *  返回值: 获取到的速率
 *  备注: Check函数返回成功之后,调用此函数即可获得上一统计周期内的速率,单位:每秒
 */
int CycleStat_GetRate(TCycleStat *pCycleStat, int iIdx);

/*  功能描述: 获取均值
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *      int iIdx [IN]:					获取的统计计数器的索引
 *  返回值: 获取到的均值,-1表示没有统计样本
 *  备注: Check函数返回成功之后,调用此函数即可获得上一统计周期内的均值
 */
int CycleStat_GetAverage(TCycleStat *pCycleStat, int iIdx);

/*  功能描述: 获取最大值
 *  参数说明:
 *      TCycleStat *pCycleStat [IN]:	对象数据结构指针
 *      int iIdx [IN]:					获取的统计计数器的索引
 *  返回值: 获取到的最大值 
 *  备注: Check函数返回成功之后,调用此函数即可获得上一统计周期内的最大值
 */
int CycleStat_GetMax(TCycleStat *pCycleStat, int iIdx);

#ifdef __cplusplus
}
#endif

#endif //__CYCLESTAT_H__
