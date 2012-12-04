/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: MyXMLParser.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-7-31 14:13:56
 *  修正备注: 
 *  
 */

#ifndef __MYXMLPARSER_H__
#define __MYXMLPARSER_H__

#include "XMLParser.h"

#define MYXMLPARSER_MAX_CURPATH_LEN		(256)

class CMyXMLParser
{
public:
	CMyXMLParser();
	~CMyXMLParser();

	/*  功能描述: 输入XML报文进行分析,并且创建MyXMLParser分析对象
	 *  参数说明:
	 *      const char *lpszXML [IN]:	需要分析的XML报文
	 */
	bool Create(const char *lpszXML);

	/*  功能描述: 将XML报文合成输出到缓冲中
	 *  参数说明:
	 *      const char **szaFileHeader [IN]:	额外添加的文本头,如果没有添NULL
	 *      char *pszBuffer [IN]:				输出的缓冲地址
	 *      int iBufferLen [IN]:				输出缓冲的最大长度
	 *  返回值: 实际合成字符串的长度.
	 */
	int WriteToBuffer(const char **szaFileHeader, char *pszBuffer, int iBufferLen);

	/*  功能描述: 设置当前操作的路径
	 *  参数说明:
	 *      const char *lpszPath [IN]:	当前操作路径,绝对路径,tag0为路径录的第一个子路径
	 *				1. tag0/tag1.id0/tag2/..., 其中tag0,tag1,tag2,...为XML标签的名称,
	 *				   '.'后面的id0表示存在多个同名的标识的时候,标识要找的tag的id
	 *				2. tag0/tag1[idx]/tag2/..., 其中tag0,tag1,tag2,...为XML标签的名称,
	 *				   []里面的idx表示存在多个同名的标识的时候,要找的tag的位置序号,从0开始.
	 *  备注: 缺省操作路径为根节点
	 */
	void SetCurPath(const char *lpszFullPath);

	/*  功能描述: 获取指定路径的XML标签的值
	 *  参数说明:
	 *      const char *lpszPath [IN]:	标签所在的相对路径,""表示当前路径,具体含义如下:
	 *  返回值: 标签值的输出缓冲地址
	 */
	const char *GetValue(const char *lpszPath);

	/*  功能描述: 设置指定路径的XML标签的值
	 *  参数说明:
	 *      const char *lpszPath [IN]:	同上
	 *      const char *lpszValue [IN]:	标签的值
	 */
	bool SetValue(const char *lpszPath, const char *lpszValue);

	/*  功能描述: 获取指定路径的XML标签的属性
	 *  参数说明:
	 *      const char *lpszPath [IN]:		同上
	 *      const char *lpszAttrName [IN]:	属性的名称
	 *  返回值: 标签属性的输出缓冲地址
	 */
	const char *GetAttributeValue(const char *lpszPath, const char *lpszAttrName);

	/*  功能描述: 设置指定路径的XML标签的属性
	 *  参数说明:
	 *      const char *lpszPath [IN]:		同上
	 *      const char *lpszAttrName [IN]:	属性的名称
	 *		const char *lpszAttrValue [IN]:	属性的值
	 */
	bool SetAttributeValue(const char *lpszPath, const char *lpszAttrName, const char *lpszAttrValue);

	/*  功能描述: 获取指定路径的XML标签的内嵌标签的个数
	 *  参数说明:
	 *      const char *lpszPath [IN]:		同上
	 *  返回值: 指定路径的XML标签的内嵌标签的个数
	 */
	int GetEmbeddedTagNum(const char *lpszPath);

	/*  功能描述: 移除指定的标签
	 *  参数说明:
	 *      const char *lpszPath [IN]:		同上
	 *  返回值: 移除出来的XML标签
	 */
	XMLTag *RemoveTag(const char *lpszPath);

	/*  功能描述: 删除指定的标签
	 *  参数说明:
	 *      const char *lpszPath [IN]:		同上
	 */
	void DeleteTag(const char *lpszPath);

private:
	XMLParser* m_pParser;
	char m_szCurPath[MYXMLPARSER_MAX_CURPATH_LEN];

};

#endif //__MYXMLPARSER_H__
