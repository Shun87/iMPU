/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: PostContentParser.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-05 19:32:24
 *  修正备注: 
 *  
 */

#ifndef __POSTCONTENTPARSER_H__
#define __POSTCONTENTPARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 分析name=value&...格式字符串,一个name=value成为一个项目(Item)
 *  参数说明:
 *      int *puiItemNum [IN, OUT]:	输入的时候是能够存放的项目的最大数目
 *									输出的时候是分析出来的项目的个数
 *      char **ppItems [OUT]:		分析出来的项目的名字的指针数组
 *      char **ppValues [OUT]:		分析出来的项目的值的指针数组,根据下标一一对应
 *      char *pszPostContent [IN]:	要分析的字符串
 *  备注: 分析的过程会破坏要分析的字符串,如果字符串还有其他用途,则需要预先拷贝一份	
 */
void PostContentParser(int *piItemNum, char **ppItems, char **ppValues, char *pszPostContent);

#ifdef __cplusplus
}
#endif

#endif //__POSTCONTENTPARSER_H__
