#ifndef __MD5_H__
#define __MD5_H__

//////////////////////////////////////////////////////////////////////////
//	DES内部数据结构和定义
//////////////////////////////////////////////////////////////////////////

/*MD5.H - MD5C.C头文件*/
#define MD5_DIGEST_LEN		16
/* POINTER 定义成一个普通的指针类型 */
typedef unsigned char *POINTER;
/* UINT2 定义成两字节的字 */
typedef unsigned short UINT2;
/* UINT4定一成四字节的字  */
typedef unsigned int UINT4;
/* MD5 context. */
typedef struct 
{
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* 位数量, 模 2^64 (低位在前) */
  unsigned char buffer[64];                         /* 输入缓冲器 */
} MD5_CTX;

//////////////////////////////////////////////////////////////////////////
//	DES函数接口
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/*
	使用方法:
		1.先调用MD5_Init
		2.然后调用MD5_Update,传入原文
		3.然后调用MD5_Final得到散列摘要
 */

/*  功能描述: 初始化md5数据结构
 *  参数说明:
 *      MD5_CTX *context [IN]:	需要初始化的md5数据结构指针
 */
void MD5_Init(MD5_CTX *context);

/*  功能描述: 更新需要散列的数据
 *  参数说明:
 *      MD5_CTX *context [IN]:	初始化好的md5数据结构指针
 *      unsigned char *input [IN]:	需要散列的数据缓冲地址
 *      unsigned int inputLen [IN]:	需要散列的数据的长度
 */
void MD5_Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);

/*  功能描述: 计算散列结果
 *  参数说明:
 *      unsigned char digest[16] [OUT]:	计算出来的16字节的散列数据
 *      MD5_CTX *context [IN]:			初始化好的md5数据结构指针
 *  返回值: void 
 *  备注:	
 */
void MD5_Final(unsigned char digest[16], MD5_CTX *context);

#ifdef __cplusplus
}
#endif

#endif //__MD5_H__
