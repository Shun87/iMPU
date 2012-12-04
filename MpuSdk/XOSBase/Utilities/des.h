/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: des.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2008-10-20 15:30:56
 *  修正备注: 
 *  
 */

#ifndef __DES_H__
#define __DES_H__

//////////////////////////////////////////////////////////////////////////
//	DES内部数据结构
//////////////////////////////////////////////////////////////////////////

#define DES_BLOCK_LEN	8
typedef struct _TDes {
	int sp_[8][64]; 	// Combined S and P boxes 
	char iperm[16][16][8];	// Initial and final permutations 
	char fperm[16][16][8];
	// 8 6-bit subkeys for each of 16 rounds, initialized by setkey() 
	unsigned char kn[16][8];
	int desmode;
} TDes,*DES_HANDLE;

//////////////////////////////////////////////////////////////////////////
//	DES函数接口
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/*
	使用方法:
		1.先调用Des_Init
		2.然后调用Des_SetKey
		3.然后进行加密(Des_Enc)或者解密(Des_Dec)
 */


/*  功能描述: 初始化DES模块
 *  参数说明:
 *      DES_HANDLE Handle [IN]:	DES模块句柄,即TDes结构指针
 *      int Mode [IN]:			DES模式,目前取0,表示标准加密算法
 *										取1和2的含义见c文件中的说明.
 *  返回值: 0表示成功,其他表示失败,目前不会返回失败.
 */
int  Des_Init(DES_HANDLE Handle, int Mode);

/*  功能描述: 
 *  参数说明:
 *      DES_HANDLE Handle [IN]:	DES模块句柄,即TDes结构指针
 *      char *Key [IN]:			DES加密密码,长度固定为8字节
 *  返回值: void 
 *  备注:	
 */
void Des_SetKey(DES_HANDLE Handle, char *Key);

/*  功能描述: 
 *  参数说明:
 *      DES_HANDLE Handle [IN]:	DES模块句柄,即TDes结构指针
 *      char *Block [IN, OUT]:	进去为加密前的数据,出来为加密后的数据,长度为8字节
 *  备注:	
 */
void Des_Enc(DES_HANDLE Handle, char *Block);

/*  功能描述: 
 *  参数说明:
 *      DES_HANDLE Handle [IN]:	DES模块句柄,即TDes结构指针
 *      char *Block [IN, OUT]:	进去为加密后的数据,出来为加密前的数据,长度为8字节
 *  备注:	
 */
void Des_Dec(DES_HANDLE Handle, char *Block);

#ifdef __cplusplus
}
#endif

#endif //__DES_H__
