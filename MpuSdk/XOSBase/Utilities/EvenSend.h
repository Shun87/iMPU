/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: EvenSend.h
 *  文件标识: 
 *  摘    要: 实现了数据的均匀发送	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-11-12 10:27:16
 *  修正备注: 
 *  
 */

#ifndef __EVENSEND_H__
#define __EVENSEND_H__

//返回错误码定义
#define EVENSEND_E_OK				0	//成功
#define EVENSEND_E_WOULDBLOCK		1	//暂时不可操作

typedef struct _TEvenSend
{
	int iTargetBitRate;			//目标发送码率,单位Kbps;
	char bTargetBitRateChanged;	//目标发送码率是否改变
	char bHungry;				//是否处于饥饿状态
	unsigned int uiBeginTIme;	//均匀发送开始时间
	int iSentBytes;				//从开始时间开始,已经发送的字节数
} TEvenSend;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 初始化
 *  参数说明:
 *      TEvenSend *pEvenSend [IN]:		对象数据结构指针
 *      int iBitRate []:
 *  返回值: void 
 *  备注:	
 */
void EvenSend_Init(TEvenSend *pEvenSend, int iTargetBitRate);

/*  功能描述: 探测是否可以发
 *  参数说明:
 *      TEvenSend *pEvenSend [IN]:		对象数据结构指针
 *      int iNewBytes [IN]:				打算要发送的,0表示只探测,不发送
 *  返回值: 错误码,取值见前面的宏定义,含义如下:
 *				EVENSEND_E_OK			成功,可以发送
 *				EVENSEND_E_WOULDBLOCK	暂时不可以发送
 */
int EvenSend_Probe(TEvenSend *pEvenSend, int iNewBytes);

/*  功能描述: 设置饥饿状态
 *  参数说明:
 *      TEvenSend *pEvenSend [IN]:		对象数据结构指针
 *  备注: 如果探测到可以发,而又没有数据可发就设置这个
 */
void EvenSend_SetHungry(TEvenSend *pEvenSend);

/*  功能描述: 设置目标发送码率
 *  参数说明:
 *      TEvenSend *pEvenSend [IN]:		对象数据结构指针
 *      int iTargetBitRate [IN]:		目标发送码率
 */
void EvenSend_SetTargetBitRate(TEvenSend *pEvenSend, int iTargetBitRate);

#ifdef __cplusplus
}
#endif

#endif //__EVENSEND_H__
