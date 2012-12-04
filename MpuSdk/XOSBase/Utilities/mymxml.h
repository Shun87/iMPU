/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: mymxml.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2010-4-9 10:10:03
 *  修正备注: 
 *  
 */

#ifndef __MYMXML_H__
#define __MYMXML_H__

#include "mxml.h"

typedef struct _mymxml_t
{
	mxml_node_t	*root;
	mxml_node_t *curNode;
	int hasVersion;
} mymxml_t;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 创建一个XML报文/创建一个gb2312编码的XML报文
 *  参数说明:
 *      const char *version [IN]:	版本,一般为"1.0",如果为NULL则没有版本
 *  返回值: mymxml_t结构指针,NULL表示出错
 *  备注: version为NULL就没有类似<?xml version="1.0" encoding="UTF-8"?>
 */
mymxml_t *mymxmlNew(const char *version);
mymxml_t *mymxmlNew_gb2312(const char *version);

/*  功能描述: 加载解释XML报文
 *  参数说明:
 *      const char *s [IN]:	需要解释的XML报文
 *  返回值: mymxml_t结构指针,NULL表示出错
 */
mymxml_t *mymxmlLoadString(const char *s);

/*  功能描述: 绑定一个已有的mxml节点作为根节点分析
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      mxml_node_t *p_root [IN]:	作为根节点的节点
 */
void mymxmlAttachRoot(mymxml_t *p_mymxml, mxml_node_t *p_root);

/*  功能描述: 合成XML报文
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      char *buffer [IN]:		存放XML报文的缓冲
 *      int bufsize [IN]:		buffer缓冲的总长度
 *  返回值: 合成的XML报文的长度,-1表示出错
 *  备注: 如果缓冲不够,不会返回错误,但会丢失多余的数据
 */
int mymxmlSaveString(mymxml_t *p_mymxml, char *buffer, int bufsize);

/*  功能描述: 合成XML报文, 内部分配内存
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *  返回值: 合成的报文内存指针,不用时要用free释放;NULL表示出错
 */
char *mymxmlSaveAllocString(mymxml_t *p_mymxml);

/*  功能描述: 设置操作的当前路径
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	当前操作的路径,如果以'/'开头认为是绝对路径,否则为相对路径格式如下:
 *									"/tag0/tag1<id="id0">/tag2/..." 
 *									其中tag0为根节点,后面的依次类推,
 *									<id="id0">表示存在多个同名的项的时候,要找的标签的id属性值为"id0".
 *									例如如下XML报文的路径可以表示为:
 *									<?xml version="1.0" encoding="utf-8"?>
 *									<a>
 *										<b>
 *											<c id="0">
 *											<c id="1">
 *										</b>
 *										<d>value</d>
 *									</a>
 *										其中: a,b,c都是标签
 *											  value是标签d的值
 *											  id是标签c的属性,0是标签c的属性id的值.
 *									/a/b/c<id="0">	定位到"<c id="0">"项
 *									/a/b/c<id="1">	定位到"<c id="1">"项
 *  备注: 如果标签不存在,这个函数会自动插入一个标签.如果路径越过了叶子节点,叶子节点的值会被删除,
 *		  如果有多个属性,用'&'隔开,如: "/tag0/tag1<id="id0"&name="name">/tag2/...".
 *		  调用mymxmlSetValue, mymxmlRemoveNode等时如果操作的结点为当前路径上级, 则当前路径可能被破坏
 */
void mymxmlSetCurPath(mymxml_t *p_mymxml, const char *path);

/*  功能描述: 获取节点
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	节点相对CurPath的路径
 *  返回值: 获取的节点指针,NULL表示不存在
 */
mxml_node_t *mymxmlGetNode(mymxml_t *p_mymxml, const char *path);

/*  功能描述: 插入节点
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	节点相对CurPath的路径
 *		mxml_node_t *p_mxml_node_insert [IN]:	要插入的节点
 *  备注: 如果标签不存在,这个函数会自动插入一个标签.如果路径越过了叶子节点,叶子节点的值会被删除
 *		  完成后p_mxml_node_insert将成为path所表示节点的第一个子节点
 */
void mymxmlInsertNode(mymxml_t *p_mymxml, const char *path, mxml_node_t *p_mxml_node_insert);

/*  功能描述: 获取标签值
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	标签相对CurPath的路径
 *		const char *def [IN]: 默认值,不存在时返回这个值
 *  返回值: 读取的标签值,标签不存在返回默认值
 *  备注: 只能是叶子节点才能够获取标签值.例如:<a>value</a>
 */
const char *mymxmlGetValue(mymxml_t *p_mymxml, const char *path, const char *def);

/*  功能描述: 设置标签值
			  不删除原来的值
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	标签相对CurPath的路径
 *      const char *value [IN]:	标签值(可以是XML格式的字符串)
 *  备注: 如果标签不存在,这个函数会自动插入一个标签.如果路径越过了叶子节点,叶子节点的值会被删除;
 *		  对于mymxmlSetValue,如果要设置的节点不是叶子节点,则删除此节点的所有子节点,使其成为叶子节点
 *		  第二个函数不删除原来的值,但是,value必须是一个子标签,不能是值,如果是值的话,将会追加在原值后面.
 */
void mymxmlSetValue(mymxml_t *p_mymxml, const char *path, const char *value);
void mymxmlSetValueNoOverlay(mymxml_t *p_mymxml, const char *path, const char *value);

/*  功能描述: 获取属性值
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	标签相对CurPath的路径
 *      const char *attrName [IN]:	属性值
 *		const char *def [IN]: 默认值,不存在时返回这个值
 *  返回值: 读取的标签属性,属性不存在默认值
 */
const char *mymxmlGetAttributeValue(mymxml_t *p_mymxml, const char *path, const char *attrName, const char *def);

/*  功能描述: 设置属性值
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	标签相对CurPath的路径
 *      const char *attrName [IN]:	属性名
 *      const char *attrValue [IN]:	属性值
 *  备注: 如果标签或属性不存在,这个函数会自动插入一个标签或属性.如果路径越过了叶子节点,叶子节点的值会被删除;
 */
void mymxmlSetAttributeValue(mymxml_t *p_mymxml, const char *path, const char *attrName, const char *attrValue);

/*  功能描述: 移除节点
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      const char *path [IN]:	节点相对CurPath的路径
 *  返回值: 移除的节点指针,NULL表示不存在
 */
mxml_node_t *mymxmlRemoveNode(mymxml_t *p_mymxml, const char *path);

/*  功能描述: 寻找当前节点的下一个兄弟节点
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      mxml_node_t *p_mxml_node [IN]:	当前节点
 *  返回值: 下一个兄弟节点,NULL表示没有下一个兄弟节点了
 */
mxml_node_t *mymxmlNextNode(mymxml_t *p_mymxml, mxml_node_t *p_mxml_node);

/*  功能描述: 寻找当前节点的第一个子节点
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *      mxml_node_t *p_mxml_node [IN]:	当前节点
 *  返回值: 第一个子节点,NULL表示没有子节点
 */
mxml_node_t *mymxmlFirstSubNode(mymxml_t *p_mymxml, mxml_node_t *node);

/*  功能描述: 获取节点的标签值
 *  参数说明:
 *      mxml_node_t *p_mxml_node [IN]:	节点指针
 *      const char *def [IN]:			如果节点没有Value时的缺省值
 *  返回值: 节点的标签值
 *  备注: 非叶子节点,将返回def.
 */
const char *mymxmlGetNodeValue(mxml_node_t *p_mxml_node, const char *def);

/*  功能描述: 获取节点的属性值
 *  参数说明:
 *      mxml_node_t *p_mxml_node [IN]:	mymxml_t结构指针
 *      const char *attrName [IN]:	属性名称
 *		const char *def [IN]: 默认值,不存在时返回这个值
 *  返回值: 读取的标签属性值,属性不存在时取默认值
 */
const char *mymxmlGetNodeAttributeValue(mxml_node_t *p_mxml_node, const char *attrName, const char *def);

/*  功能描述: 释放mymxml_t结构指针
 *  参数说明:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t结构指针
 *  备注: 使用完成之后,必须调用这个释放,否则会内存泄露.
 */
void mymxmlDelete(mymxml_t *p_mymxml);

#ifdef __cplusplus
}
#endif

#endif //__MYMXML_H__
