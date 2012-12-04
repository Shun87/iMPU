/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: MemBlock.h
 *  文件标识: 
 *  摘    要: 实现了一个伪C的内存块类	
 *  
 *  当前版本: 
 *  作    者: zhaoqq
 *  完成日期: 2007-01-05 15:11:59
 *  修正备注: 
 *  
 */

#ifndef __MEMBLOCK_H__
#define __MEMBLOCK_H__

typedef struct _TMemBlock TMemBlock;
struct _TMemBlock  
{
//public:
	void (*Construct)(TMemBlock *pMemBlock);
	void (*Destroy)(TMemBlock *pMemBlock);

	/*  功能描述: 创建内存块对象
	 *  参数说明:
	 *      int iSize [IN]:			内存块大小
	 *  返回值: 
	 *  备注:	
	 */
	XOS_BOOL (*Create)(TMemBlock *pMemBlock, int iSize);

	/*  功能描述: 获取内存块地址
	 *  返回值: 内存块地址
	 */
	char* (*GetBuffer)(TMemBlock *pMemBlock);

	/*  功能描述: 获取内存块长度
	 *  返回值: 内存块长度
	 */
    int (*GetSize)(TMemBlock *pMemBlock);

	/*  功能描述: 设置内存块实际大小
	 *  参数说明:
	 *      int iSize [IN]:		内存块实际大小
	 */
    XOS_BOOL (*SetSize)(TMemBlock *pMemBlock, int iSize);

//private:	
	char *m_pBuffer;	//内存缓冲地址
	int m_iSize;		//内存块的实际大小
	int m_iMaxSize;		//内存块的最大大小
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TMemBlock MEMBLOCK;

#ifdef __cplusplus
}
#endif

#endif //__MEMBLOCK_H__
