/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: LoopBuffer.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-11-10 14:04:35
 *  修正备注: 
 *  
 */

#ifndef __LOOPBUFFER_H__
#define __LOOPBUFFER_H__

typedef struct _TLoopBuffer TLoopBuffer;
struct _TLoopBuffer
{
//public:
	void (*Construct)(TLoopBuffer *pLoopBuffer);
	void (*Destroy)(TLoopBuffer *pLoopBuffer);
	XOS_BOOL (*Create)(TLoopBuffer *pLoopBuffer, XOS_U32 uiBufferLen);

	/*  功能描述: 向环形缓冲写入数据
	 *  参数说明:
	 *      char *pData [IN]:		数据指针
	 *      XOS_U32 uiDataLen [IN]:	数据长度
	 *      XOS_BOOL bCover [IN]:		是否覆盖,如果不是,则满了之后,返回值返回实际写入的字节数
	 *  返回值: 添加成功的字节数,如果选择覆盖,则这个值等于uiDataLen;
	 */
	XOS_U32 (*WriteData)(TLoopBuffer *pLoopBuffer, char *pData, XOS_U32 uiDataLen, XOS_BOOL bCover);

	/*  功能描述: 从环形缓冲读取数据
	 *  参数说明:
	 *      char *pData [IN]:		数据指针
	 *      XOS_U32 *puiDataLen [IN, OUT]:	进去的时候是pData的大小,出来的时候是实际填充的字节数.
	 */
	XOS_BOOL (*ReadData)(TLoopBuffer *pLoopBuffer, char *pData, XOS_U32 *puiDataLen);

//private:
	char* (*pAdd)(TLoopBuffer *pLoopBuffer, char *p);

	char *m_pBuf;
	XOS_U32 m_uiBufferLen;
	char *m_pRead;
	char *m_pWrite;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TLoopBuffer LOOPBUFFER;

#ifdef __cplusplus
}
#endif

#endif //__LOOPBUFFER_H__
