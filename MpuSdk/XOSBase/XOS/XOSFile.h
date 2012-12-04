/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSFile.h
 *  文件标识: 
 *  摘    要: 封装了对磁盘文件和目录访问接口
 *            注意只对磁盘文件,其他形似的文件也许不适合这些函数
 *			  Linux系统中的驱动文件怎么办?比如找磁盘文件的时候?
 *			  靠路径区分?
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-6-29 15:23:19
 *  修正备注: 
 *  
 */

#ifndef __XOSFILE_H__
#define __XOSFILE_H__

#define XOSFILE_SEEK_ORIGIN_BEGIN	0
#define XOSFILE_SEEK_ORIGIN_CUR		1
#define XOSFILE_SEEK_ORIGIN_END		2

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* 文件读写函数                                                         */
/************************************************************************/

/*  功能描述: 获取文件大小
 *  参数说明:
 *      XOS_CPSZ lpszFileName [IN]:	文件名
 *  返回值: 文件字节数,-1表示没有这个文件
 */
int XOSFile_Length(XOS_CPSZ cpszFileName);

/*  功能描述: 打开文件
 *  参数说明:
 *      XOS_CPSZ cpszFileName [IN]:	文件名
 *		XOS_BOOL bReadOnly [IN]: 是否只读
 *  返回值: 文件句柄，NULLL:打开失败
 */
XOS_HANDLE XOSFile_Open(XOS_CPSZ cpszFileName, XOS_BOOL bReadOnly);

/*  功能描述: 将数据写入文件流中
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
 *		void *pData [IN]: 指向欲写入的数据地址
 *		XOS_U32 uiLen [IN]: 总共写入的字符数
 *  返回值: 返回实际写入的字节数目
 */
int XOSFile_Write(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen);

/*  功能描述: 从文件流读取数据
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
 *		void *pData [OUT]: 欲存放读取进来的数据空间
 *		XOS_U32 uiLen [IN]: 读取的字符数
 *  返回值: 返回实际读取到的字节数目
 */
int XOSFile_Read(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen);

/*  功能描述: 为给出的流设置位置数据
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
 *		int iOffset [IN]: 它表示位置指针相对于“起始点”移动的字节数
 *						  大于0表示往后移,小于0表示往前移
 *		int iOrigin [IN]: 偏移起始点						
 *						 XOSFILE_SEEK_ORIGIN_BEGIN:	文件开头 
 *						 XOSFILE_SEEK_ORIGIN_CUR:	文件当前位置 
 *						 XOSFILE_SEEK_ORIGIN_END:	文件末尾
 */
XOS_BOOL XOSFile_Seek(XOS_HANDLE hFile, int iOffset, int iOrigin);

/*  功能描述: 得到文件的当前位置
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
 *  返回值: 文件当前位置相对于文件开头的字节数,-1表示出错
 */
int XOSFile_Tell(XOS_HANDLE hFile);

/*  功能描述: 清除文件缓冲区,文件以写方式打开时将缓冲区内容写入文件
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
 */
void XOSFile_Flush(XOS_HANDLE hFile);

/*  功能描述: 判断文件指针是否指到文件尾
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
 */
XOS_BOOL XOSFile_Eof(XOS_HANDLE hFile);

/*  功能描述: 关闭由文件指针指定的文件
 *  参数说明:
 *      XOS_HANDLE hFile [IN]: 已打开的文件指针
	备注: 打开的文件必须使用这个函数关闭
 */
void XOSFile_Close(XOS_HANDLE hFile);

/************************************************************************/
/* 目录创建删除和文件删除函数                                           */
/************************************************************************/

/*  功能描述: 创建文件夹
 *  参数说明:
 *      XOS_CPSZ cpszFullPath [IN]: 文件夹的绝对路径,可包括多级路径
 *	备注: 传入的路径字符串虽然不会更改其内容,可是会临时修改,然后再改回去
 *		  所以不能够传入常量字符串.
 */
XOS_BOOL XOSFile_MakeDir(XOS_CPSZ cpszFullPath);

/*  功能描述: 移除文件夹
 *  参数说明:
 *      XOS_CPSZ cpszFullPath [IN]: 文件夹路径绝对路径
 */
void XOSFile_RemoveDir(XOS_CPSZ cpszFullPath);

/*  功能描述: 移除文件
 *  参数说明:
 *		XOS_CPSZ cpszFullName [IN]:	全路径文件名
 *		XOS_CPSZ cpszFullPath [IN]: 文件路径
 *      XOS_CPSZ cpszFileName [IN]: 要移除文件名
 */
void XOSFile_RemoveFile2(XOS_CPSZ cpszFullName);
void XOSFile_RemoveFile(XOS_CPSZ cpszFullPath, XOS_CPSZ cpszFileName);

/************************************************************************/
/* 目录和文件遍历函数                                                   */
/************************************************************************/

/*  功能描述: 查找第一个子目录/子文件
 *  参数说明:
 *      XOS_HANDLE *phFind [OUT]:		文件查找句柄
 *      XOS_CPSZ cpszFullPath [IN]:		目录路径
 *		XOS_BOOL bdir [IN]:				是否是目录,XOS_TRUE为目录,XOS_FALSE为文件
 *		char *pszFileName [OUT]:		存放查询到的子目录/文件名的缓冲地址
 *		int iFileNameLen [IN]:			上述缓冲地址的最大长度
 *  返回值: 子目录名称,NULL表示失败,其他表示成功, 
 */
XOS_PSZ XOSFile_FindFirstSub(XOS_HANDLE *phFind, XOS_CPSZ cpszFullPath, XOS_BOOL bDir, 
							 char *pszFileName, int iFileNameLen);

/*  功能描述: 查找下一个子目录/子文件
 *  参数说明:
 *      XOS_HANDLE hFind [IN]:			文件查找句柄
 *      XOS_CPSZ cpszFullPath [IN]:		目录路径,必须与调用XOSFile_FindFirstSub时相同
 *		XOS_BOOL bdir [IN]:				是否是目录,必须与调用XOSFile_FindFirstSub时相同
 *		char *pszFileName [OUT]:		存放查询到的子目录/文件名的缓冲地址
 *		int iFileNameLen [IN]:			上述缓冲地址的最大长度
 *  返回值: 同上
 *  备注: 要先调用XOSFile_FindFirstSub后,才能够使用.
 */
XOS_PSZ XOSFile_FindNextSub(XOS_HANDLE hFind, XOS_CPSZ cpszFullPath, XOS_BOOL bDir,
							char *pszFileName, int iFileNameLen);

/*  功能描述: 结束子目录/文件的查找
 *  参数说明:
 *      XOS_HANDLE hFind [IN]:			文件查找句柄
 *  备注: 查找结束的时候一定要调用此函数结束查找
 */
void  XOSFile_FindSubClose(XOS_HANDLE hFind);

#ifdef __cplusplus
}
#endif

#endif //__XOSFILE_H__
