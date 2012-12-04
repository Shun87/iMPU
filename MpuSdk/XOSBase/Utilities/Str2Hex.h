/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: Str2Hex.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: zhaoqq
 *  完成日期: 2007-02-01 11:23:09
 *  修正备注: 
 *  
 */

#ifndef __STR2HEX_H__
#define __STR2HEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 把字符串转变成二进制
 *  参数说明:
 *      char *pStr [IN]:	需要转换的字符串
 *      char *pHex [OUT]:	转换出来的二进制数据存放地址,上层保证缓冲大小够用.
 */
XOS_BOOL Str2Hex(char *pStr, char *pHex);

/*  功能描述: 把二进制转变成字符串
 *  参数说明:
 *      char *pHex [IN]:	需要转换的二进制缓冲的地址
 *      char *pStr [OUT]:	转换后的字符串存放地址,上层保证缓冲大小够用
 *      int iHexLen [IN]:	需要转换的二进制缓冲的长度
 *  返回值: 转换后的字符串指针,等于传入的pStr,这个函数总是返回成功的.
 *			除非用户传入的pStr不对,所以可以不对返回值进行判断
 *  备注: Hex2Str转换成大写字母,Hex2Strl转换成小写字母.
 */
char *Hex2Str(char *pHex, char *pStr, int iHexLen);
char *Hex2Strl(char *pHex, char *pStr, int iHexLen);

#ifdef __cplusplus
}
#endif

#endif //__STR2HEX_H__
