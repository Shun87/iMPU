/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: StreamComposer.h
 *  文件标识: 
 *  摘    要: 二进制流合成工具,将多个简单数据以追加的方式合成为一个二进制流缓冲
 *			  并且完成从主机字节序到网络字节序的转换
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-28 11:29:20
 *  修正备注: 
 *  
 */

#ifndef __STREAMCOMPOSER_H__
#define __STREAMCOMPOSER_H__

typedef struct _TStreamComposer TStreamComposer;
struct _TStreamComposer  
{
//public:
	void (*Construct)(TStreamComposer *pStreamComposer);
	void (*Destroy)(TStreamComposer *pStreamComposer);

	/*  功能描述: 创建二进制流合成工具
	 *  参数说明:
	 *      void *pBuf [IN]:	存放合成后的二进制流的缓冲地址
	 *      int iMaxSize [IN]:	存放合成后的二进制流的缓冲的最大长度
	 *		XOS_BOOL bNBO [IN]:	是否是网络字节序
	 *  备注: 此函数不负责内存分配
	 */
	XOS_BOOL (*Create)(TStreamComposer *pStreamComposer, void *pBuf, int iMaxSize, XOS_BOOL bNBO);
	
	//这个函数因为有内存分配,所有注释不用
	//XOS_BOOL (*Create2)(TStreamComposer *pStreamComposer, int iMaxSize);

	/*  功能描述: 获取二进制流存放的缓冲地址
	 *  返回值: 二进制流存放的缓冲地址
	 */
	char* (*GetBuffer)(TStreamComposer *pStreamComposer);

	/*  功能描述: 获取二进制流当前填充到的缓冲地址
	 *  返回值: 二进制流当前填充到的缓冲地址
	 *	备注: 在填充的过程中,当前填充到的缓冲地址会往后移
	 */
	char* (*GetCurPos)(TStreamComposer *pStreamComposer);

	/*  功能描述: 获取已经填充的二进制流的长度
	 *  返回值: 已经填充的二进制流的长度
	 */
	int (*GetLength)(TStreamComposer *pStreamComposer);

	/*  功能描述: 获取剩下的还能够填充的字节数
	 *  返回值: 剩下的还能够填充的字节数
	 */
	int (*LeftLength)(TStreamComposer *pStreamComposer);
	
	/*  功能描述: 跳过若干字节不填充
	 *  参数说明:
	 *      int iSize [IN]:	跳过的字节数
	 *  备注: 之后直接跳过指定字节数填充新的数据
	 */
	XOS_BOOL (*SkipLength)(TStreamComposer *pStreamComposer, int iSize);

	/*  功能描述: 添加一个2字节对齐填充
	 */
	XOS_BOOL (*AddPAD2)(TStreamComposer *pStreamComposer);

	/*  功能描述: 添加一个4字节对齐填充
	 */
	XOS_BOOL (*AddPAD4)(TStreamComposer *pStreamComposer);

	/*  功能描述: 添加一个8字节对齐填充
	 */
	XOS_BOOL (*AddPAD8)(TStreamComposer *pStreamComposer);

	/*  功能描述: 添加一个字符
	 *  参数说明:
	 *      XOS_S8 c [IN]:	要添加的字符
	 */
	XOS_BOOL (*AddCHAR)(TStreamComposer *pStreamComposer, XOS_S8 c);

	/*  功能描述: 添加一个无符号字符
	 *  参数说明:
	 *      XOS_U8 uc [IN]:	要添加的字符
	 */
	XOS_BOOL (*AddUCHAR)(TStreamComposer *pStreamComposer, XOS_U8 uc);

	/*  功能描述: 添加一个短整型数据(2字节)
	 *  参数说明:
	 *      XOS_S16 s [IN]:	要添加的短整型数据
	 */
	XOS_BOOL (*AddSHORT)(TStreamComposer *pStreamComposer, XOS_S16 s);

	/*  功能描述: 添加一个无符号短整型数据(2字节)
	 *  参数说明:
	 *      XOS_S16 s [IN]:	要添加的无符号短整型数据
	 */
	XOS_BOOL (*AddUSHORT)(TStreamComposer *pStreamComposer, XOS_U16 us);

	/*  功能描述: 添加一个无符号整型数据(4字节)
	 *  参数说明:
	 *      XOS_S32 i [IN]:	要添加的整型数据
	 */
	XOS_BOOL (*AddINT)(TStreamComposer *pStreamComposer, XOS_S32 i);

	/*  功能描述: 添加一个无符号整型数据(4字节)
	 *  参数说明:
	 *      XOS_U32 ui [IN]:	要添加的整型数据
	 */
	XOS_BOOL (*AddUINT)(TStreamComposer *pStreamComposer, XOS_U32 ui);

	/*  功能描述: 添加一个4字节IP地址
	 *  参数说明:
	 *      XOS_U32 ui [IN]:	要添加的IP地址
	 */
	XOS_BOOL (*AddIP)(TStreamComposer *pStreamComposer, XOS_U32 ip);

	/*  功能描述: 添加一个16字节GUID数据
	 *  参数说明:
	 *      const TCGUID * pGuid [IN]:	要添加的GUID数据
	 */
	XOS_BOOL (*AddGUID)(TStreamComposer *pStreamComposer, const TCGUID* pGuid);

	/*  功能描述: 添加一个以0结尾的字符串
	 *  参数说明:
	 *      XOS_PSZ psz [IN]:	要添加的字符串
	 */
	XOS_BOOL (*AddString)(TStreamComposer *pStreamComposer, XOS_CPSZ cpsz);

	/*  功能描述: 添加若干字节的二进制数据
	 *  参数说明:
	 *      const void *pBuf [IN]:	要添加的二进制数据的地址
	 *      int iLen [IN]:			要添加的二进制数据的长度
	 */
	XOS_BOOL (*AddBuffer)(TStreamComposer *pStreamComposer, const void *pBuf, int iLen);

//private:
	XOS_BOOL (*BeforeIncSize)(TStreamComposer *pStreamComposer, int iSize);
	XOS_BOOL (*IncSize)(TStreamComposer *pStreamComposer, int iSize);
	
	char *m_pBuf;
	int m_iSize;
	int m_iMaxSize;
	XOS_BOOL m_bDeleteBuf;
	XOS_BOOL bNBO;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TStreamComposer STREAMCOMPOSER;

#ifdef __cplusplus
}
#endif

#endif //__STREAMCOMPOSER_H__
