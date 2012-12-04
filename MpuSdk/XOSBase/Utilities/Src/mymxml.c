/*
 * Copyright (c) 2010, 安徽创世科技有限公司
 * All rights reserved.
 *
 * 文件名称: mymxml.c
 * 文件标识: 
 * 摘    要: 
 *
 * 当前版本: 
 * 作    者: shdxiang
 * 完成日期: 2010/04/09 15:26:32
 * 修正备注: 
 *
 */

#include "../../XOS/XOS.h"
#include "../mymxml.h"

#define MYMXML_MAX_PATH_LEN		1024			// 路径最大长度
#define MYMXML_MAX_NAV_LEN		256				// 标签,属性名,属性值最大长度

/* 功能描述:	检查节点的属性是否符合
 * 参数说明:	
 *				mxml_node_t *p_mxml_node[IN]: 要检查的节点
 *				const char *attr_str[IN]: 属性字符串,各个属性用空格隔开(aa="aa" bb="bb" cc="cc")
 *				int create[IN]: 属性不存在时是否创建
 * 返回值:		create为0时返回0: 属性匹配, 其他不匹配; create不为0, 返回值无意义
 * 备注:		
 */
static int mymxmlAccessAttr(mxml_node_t *p_mxml_node, const char *attr_str, int create)
{
	char *p = NULL;
	char *q = NULL;
	char attr_str_buf[MYMXML_MAX_NAV_LEN] = {0};
	char attr[MYMXML_MAX_NAV_LEN] = {0};
	char value[MYMXML_MAX_NAV_LEN] = {0};

	assert(strlen(attr_str) < MYMXML_MAX_NAV_LEN - 1);
	strcpy(attr_str_buf, attr_str);
	p = attr_str_buf;
	while (p != 0) {
		// 找一对属性名和属性值
		q = p;
		// 得到属性名
		while (*q != 0 && *q != '=')
			q++;
		if (*q == 0)
			break;
		strncpy(attr, p, q - p);
		attr[q - p] = 0;
		// 得到属性值
		while (*q != 0 && (*q == '=' || *q == '"'))
			q++;
		if (*q == 0)
			break;
		p = q;
		while (*p != 0 && *p != '"' && *p != '&')
			p++;
		strncpy(value, q, p - q);
		value[p - q] = 0;
		while (*p != 0 && (*p == '"' || *p == '&'))
			p++;
		// 检查属性
		if (!create) {
			q = (char *)mxmlElementGetAttr(p_mxml_node, attr);
			if (q == NULL || strcmp(q, value) != 0)
				return 1;
		} else {
			mxmlElementSetAttr(p_mxml_node, attr, value);
		}
	}
	return 0;
}

/* 功能描述:	由路径path找到对应的节点,当节点不存在时根据create来决定是否创建
 * 参数说明:	
 *				mymxml_t *p_mymxml[IN]: mymxml_t结构指针
 *				const char *path[IN]: 路径,如果以'/'开头认为是绝对路径,否则为相对路径
 *				int create[IN]: 当路径中的节点不存在时是否创建
 * 返回值:		找到的节点或NULL
 * 备注:		
 */
static mxml_node_t *mymxmlAccessNode(mymxml_t *p_mymxml, const char *path, int create)
{
	char path_buf[MYMXML_MAX_PATH_LEN] = {0};
	char *p = NULL;
	char *q = NULL;
	char *r = NULL;
	mxml_node_t *p_mxml_node = NULL;
	mxml_node_t *p_mxml_node_top = NULL;
	char name[MYMXML_MAX_NAV_LEN] = {0};
	char attr_str[MYMXML_MAX_NAV_LEN] = {0};
	int len = 0;
	int is_root = 0;
	
	assert(p_mymxml != NULL && path != NULL);

	len = strlen(path);
	assert(len + 1 < MYMXML_MAX_PATH_LEN);
	
	// 判断是不是绝对路径
	if (path[0] == '/')	{
		strcpy(path_buf, path);
		p_mxml_node_top = p_mymxml->hasVersion ? p_mymxml->root->last_child : p_mymxml->root;
		is_root = 1;
	} else {
		path_buf[0] = '/';
		len++;
		strcpy(path_buf + 1, path);
		p_mxml_node_top = p_mymxml->curNode;
	}

	// 去掉最后一个'/'
	if (path_buf[len - 1] == '/')
		path_buf[len - 1] = 0;

	p_mxml_node = p_mxml_node_top;
	// p指向路径某项的头
	p = path_buf;
	while (*p != 0)	{
		// q指向路径某项的尾
		q = p + 1;
		r = NULL;
		while (*q != 0 && *q != '/' && *q != '<')
			q++;
		// 如果碰到有属性'<',就先找到属性的结束'>',忽略里面的'/'
		if (*q == '<') {
			// r指向路径某项的标签尾
			r = q;
			while (*q != 0 && *q != '>')
				q++;
		}
		while (*q != 0 && *q != '/')
			q++;

		// r指向路径某项的标签尾
		if (r == NULL)
			r = q;

		// 得到标签
		len = r - p - 1;
		assert(len < MYMXML_MAX_NAV_LEN);
		strncpy(name, p + 1, len);
		name[len] = 0;
		
		// 如果有就得到属性串
		if (r < q) {
			strncpy(attr_str, r + 1, q - r - 2);
			attr_str[q - r - 2] = 0;
		}
		// 如果是根节点
		if (is_root != 0) {
			if (p_mxml_node_top == NULL || p_mxml_node_top->value.element.name == NULL || strcmp(p_mxml_node_top->value.element.name, name) != 0) {
				if (create == 0)
					return NULL;

				if (p_mymxml->hasVersion != 0) {
					mxmlDelete(p_mxml_node_top);
					p_mxml_node_top = mxmlNewElement(p_mymxml->root, name);
				} else {
					mxmlDelete(p_mxml_node_top);
					p_mymxml->root = mxmlNewElement(NULL, name);
					p_mxml_node_top = p_mymxml->root;
				}
			}
			if (r < q)
				mymxmlAccessAttr(p_mxml_node_top, attr_str, 1);
			p_mxml_node = p_mxml_node_top;
			is_root = 0;
		} else {
			// 由得到的标签,属性名,属性值找节点
			p_mxml_node = mxmlFindElement(p_mxml_node_top, p_mxml_node_top, name, NULL, NULL, MXML_DESCEND_FIRST);
			// 如果有属性则找出所有属性都匹配的节点
			if (r < q) {
				while (p_mxml_node != NULL) {
					if (mymxmlAccessAttr(p_mxml_node, attr_str, 0) == 0)
						break;
					p_mxml_node = mxmlFindElement(p_mxml_node, p_mxml_node_top, name, NULL, NULL, MXML_NO_DESCEND);
				}
			}

			if (p_mxml_node == NULL) {
				// 找不到又指定不创建则返回NULL
				if (create == 0)
					return NULL;

				// 如果父节点是叶子节点,则删除其值
				if (p_mxml_node_top != NULL && p_mxml_node_top->child != NULL && p_mxml_node_top->child->type != MXML_ELEMENT) {
					mxmlRemove(p_mxml_node_top->child);
					mxmlDelete(p_mxml_node_top->child);
				}

				// 创建节点
				p_mxml_node = mxmlNewElement(p_mxml_node_top, name);
				if (r < q)
					mymxmlAccessAttr(p_mxml_node, attr_str, 1);
			}
			p_mxml_node_top = p_mxml_node;
		}

		// p指向路径下一项的头
		p = q;
	}

	return p_mxml_node;
}

mymxml_t *mymxmldoNew(const char *version, const char *encoding)
{
	mymxml_t *p_mymxml = NULL;

	p_mymxml = malloc(sizeof(mymxml_t));
	if (p_mymxml == NULL)
		return NULL;

	if (version == NULL) {
		p_mymxml->root = NULL;
		p_mymxml->hasVersion = 0;
	} else {
		if (strcmp(encoding, "gb2312") == 0) {
			p_mymxml->root = mxmlNewXML_gb2312(version);
		}
		else {
			p_mymxml->root = mxmlNewXML(version);
		}
		p_mymxml->hasVersion = 1;
	}

	p_mymxml->curNode = NULL;

	mxmlSetWrapMargin(0);

	return p_mymxml;
}
mymxml_t *mymxmlNew(const char *version)
{
	return mymxmldoNew(version, "");
}
mymxml_t *mymxmlNew_gb2312(const char *version)
{
	return mymxmldoNew(version, "gb2312");
}

mymxml_t *mymxmlLoadString(const char *s)
{
	mymxml_t *p_mymxml = NULL;
	mxml_node_t *p_mxml_node = NULL;

	if (s == NULL)
		return NULL;

	p_mxml_node = mxmlLoadString(NULL, s, MXML_NO_CALLBACK);
	if (p_mxml_node == NULL)
		return NULL;// Load failed

	p_mymxml = malloc(sizeof(mymxml_t));
	if (p_mymxml == NULL) {
		mxmlDelete(p_mxml_node);
		return NULL;
	}

	p_mymxml->root = p_mxml_node;
	p_mymxml->curNode = NULL;
	p_mymxml->hasVersion = (s[1] == '?');

	mxmlSetWrapMargin(0);

	return p_mymxml;
}

void mymxmlAttachRoot(mymxml_t *p_mymxml, mxml_node_t *p_root)
{
	p_mymxml->root = p_root;
	p_mymxml->curNode = p_root;
	p_mymxml->hasVersion = 0;
}

int mymxmlSaveString(mymxml_t *p_mymxml, char *buffer, int bufsize)
{
	if (p_mymxml->root == NULL)
		return 0;
	return mxmlSaveString(p_mymxml->root, buffer, bufsize, MXML_NO_CALLBACK);
}

char *mymxmlSaveAllocString(mymxml_t *p_mymxml)
{
	if (p_mymxml->root == NULL)
		return 0;
	return mxmlSaveAllocString(p_mymxml->root, MXML_NO_CALLBACK);
}

void mymxmlSetCurPath(mymxml_t *p_mymxml, const char *path)
{
	mxml_node_t *p_mxml_node = NULL;

	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 1);
	assert(p_mxml_node != NULL);
	p_mymxml->curNode = p_mxml_node;
}

mxml_node_t *mymxmlGetNode(mymxml_t *p_mymxml, const char *path)
{
	return mymxmlAccessNode(p_mymxml, path, 0);
}

void mymxmlInsertNode(mymxml_t *p_mymxml, const char *path, mxml_node_t *p_mxml_node_insert)
{
	mxml_node_t *p_mxml_node = NULL;

	if (p_mymxml->root == NULL)	{
		p_mymxml->root = p_mxml_node_insert;
	} else {
		p_mxml_node = mymxmlAccessNode(p_mymxml, path, 1);
		mxmlAdd(p_mxml_node, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, p_mxml_node_insert);
	}
}

const char *mymxmlGetValue(mymxml_t *p_mymxml, const char *path, const char *def)
{
	mxml_node_t *p_mxml_node = NULL;
	
	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 0);
	if (p_mxml_node == NULL || p_mxml_node->child == NULL || p_mxml_node->child->type == MXML_ELEMENT
		|| p_mxml_node->child->value.text.string == NULL || p_mxml_node->child->value.text.string[0] == 0)
		return def;

	return p_mxml_node->child->value.text.string;
}

void mymxmlSetValue(mymxml_t *p_mymxml, const char *path, const char *value)
{
	mxml_node_t *p_mxml_node = NULL;
	mxml_node_t *p_mxml_node_child = NULL;

	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 1);
	assert(p_mxml_node != NULL);

	// Delete all children
	while (p_mxml_node->child != NULL) {
		mxmlRemove(p_mxml_node->child);
		mxmlRelease(p_mxml_node->child);
	}

	if (value !=NULL && value[0] == '<') {
		p_mxml_node_child = mxmlLoadString(NULL, value, MXML_NO_CALLBACK);
		if (p_mxml_node_child != NULL) {
			mxmlAdd(p_mxml_node, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, p_mxml_node_child);
		}
	} else {
		mxmlNewText(p_mxml_node, 0, value);
	}
}

void mymxmlSetValueNoOverlay(mymxml_t *p_mymxml, const char *path, const char *value)
{
	mxml_node_t *p_mxml_node = NULL;
	mxml_node_t *p_mxml_node_child = NULL;
	
	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 1);
	assert(p_mxml_node != NULL);

	if (value !=NULL && value[0] == '<') {
		p_mxml_node_child = mxmlLoadString(NULL, value, MXML_NO_CALLBACK);
		if (p_mxml_node_child != NULL) {
			mxmlAdd(p_mxml_node, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, p_mxml_node_child);
		}
	} else {
		mxmlNewText(p_mxml_node, 0, value);
	}
}

const char *mymxmlGetAttributeValue(mymxml_t *p_mymxml, const char *path, const char *attrName, const char *def)
{
	mxml_node_t *p_mxml_node = NULL;
	const char *p;

	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 0);
	if (p_mxml_node == NULL)
		return def;

	p = mxmlElementGetAttr(p_mxml_node, attrName);

	return p == NULL ? def : p;
}

void mymxmlSetAttributeValue(mymxml_t *p_mymxml, const char *path, const char *attrName, const char *attrValue)
{
	mxml_node_t *p_mxml_node = NULL;
	
	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 1);
	assert(p_mxml_node != NULL);

	mxmlElementSetAttr(p_mxml_node, attrName, attrValue);
}

mxml_node_t *mymxmlNextNode(mymxml_t *p_mymxml, mxml_node_t *p_mxml_node)
{
	mxml_node_t *pxmlNode;

	pxmlNode = p_mxml_node->next;
	
	while (pxmlNode)
	{
		if (pxmlNode->type == MXML_ELEMENT &&
			pxmlNode->value.element.name)
		{
			break;
		}
		pxmlNode = pxmlNode->next;
	}

	return pxmlNode;
}

mxml_node_t *mymxmlFirstSubNode(mymxml_t *p_mymxml, mxml_node_t *p_mxml_node)
{
	mxml_node_t *pxmlNode;

	pxmlNode = p_mxml_node->child;
	
	while (pxmlNode)
	{
		if (pxmlNode->type == MXML_ELEMENT &&
			pxmlNode->value.element.name)
		{
			break;
		}
		pxmlNode = pxmlNode->next;
	}

	return pxmlNode;
}

mxml_node_t *mymxmlRemoveNode(mymxml_t *p_mymxml, const char *path)
{
	mxml_node_t *p_mxml_node = NULL;
	p_mxml_node = mymxmlAccessNode(p_mymxml, path, 0);
	if (p_mxml_node == NULL)
		return NULL;
	if (p_mxml_node == p_mymxml->root)
		p_mymxml->root = NULL;
	mxmlRemove(p_mxml_node);
	return p_mxml_node;
}

void mymxmlDelete(mymxml_t *p_mymxml)
{
	if (p_mymxml == NULL)
		return;

	if (p_mymxml->root != NULL)
		mxmlDelete(p_mymxml->root);

	free(p_mymxml);
}
