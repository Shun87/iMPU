/*
 * Copyright (c) 2010, ���մ����Ƽ����޹�˾
 * All rights reserved.
 *
 * �ļ�����: mymxml.c
 * �ļ���ʶ: 
 * ժ    Ҫ: 
 *
 * ��ǰ�汾: 
 * ��    ��: shdxiang
 * �������: 2010/04/09 15:26:32
 * ������ע: 
 *
 */

#include "../../XOS/XOS.h"
#include "../mymxml.h"

#define MYMXML_MAX_PATH_LEN		1024			// ·����󳤶�
#define MYMXML_MAX_NAV_LEN		256				// ��ǩ,������,����ֵ��󳤶�

/* ��������:	���ڵ�������Ƿ����
 * ����˵��:	
 *				mxml_node_t *p_mxml_node[IN]: Ҫ���Ľڵ�
 *				const char *attr_str[IN]: �����ַ���,���������ÿո����(aa="aa" bb="bb" cc="cc")
 *				int create[IN]: ���Բ�����ʱ�Ƿ񴴽�
 * ����ֵ:		createΪ0ʱ����0: ����ƥ��, ������ƥ��; create��Ϊ0, ����ֵ������
 * ��ע:		
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
		// ��һ��������������ֵ
		q = p;
		// �õ�������
		while (*q != 0 && *q != '=')
			q++;
		if (*q == 0)
			break;
		strncpy(attr, p, q - p);
		attr[q - p] = 0;
		// �õ�����ֵ
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
		// �������
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

/* ��������:	��·��path�ҵ���Ӧ�Ľڵ�,���ڵ㲻����ʱ����create�������Ƿ񴴽�
 * ����˵��:	
 *				mymxml_t *p_mymxml[IN]: mymxml_t�ṹָ��
 *				const char *path[IN]: ·��,�����'/'��ͷ��Ϊ�Ǿ���·��,����Ϊ���·��
 *				int create[IN]: ��·���еĽڵ㲻����ʱ�Ƿ񴴽�
 * ����ֵ:		�ҵ��Ľڵ��NULL
 * ��ע:		
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
	
	// �ж��ǲ��Ǿ���·��
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

	// ȥ�����һ��'/'
	if (path_buf[len - 1] == '/')
		path_buf[len - 1] = 0;

	p_mxml_node = p_mxml_node_top;
	// pָ��·��ĳ���ͷ
	p = path_buf;
	while (*p != 0)	{
		// qָ��·��ĳ���β
		q = p + 1;
		r = NULL;
		while (*q != 0 && *q != '/' && *q != '<')
			q++;
		// �������������'<',�����ҵ����ԵĽ���'>',���������'/'
		if (*q == '<') {
			// rָ��·��ĳ��ı�ǩβ
			r = q;
			while (*q != 0 && *q != '>')
				q++;
		}
		while (*q != 0 && *q != '/')
			q++;

		// rָ��·��ĳ��ı�ǩβ
		if (r == NULL)
			r = q;

		// �õ���ǩ
		len = r - p - 1;
		assert(len < MYMXML_MAX_NAV_LEN);
		strncpy(name, p + 1, len);
		name[len] = 0;
		
		// ����о͵õ����Դ�
		if (r < q) {
			strncpy(attr_str, r + 1, q - r - 2);
			attr_str[q - r - 2] = 0;
		}
		// ����Ǹ��ڵ�
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
			// �ɵõ��ı�ǩ,������,����ֵ�ҽڵ�
			p_mxml_node = mxmlFindElement(p_mxml_node_top, p_mxml_node_top, name, NULL, NULL, MXML_DESCEND_FIRST);
			// ������������ҳ��������Զ�ƥ��Ľڵ�
			if (r < q) {
				while (p_mxml_node != NULL) {
					if (mymxmlAccessAttr(p_mxml_node, attr_str, 0) == 0)
						break;
					p_mxml_node = mxmlFindElement(p_mxml_node, p_mxml_node_top, name, NULL, NULL, MXML_NO_DESCEND);
				}
			}

			if (p_mxml_node == NULL) {
				// �Ҳ�����ָ���������򷵻�NULL
				if (create == 0)
					return NULL;

				// ������ڵ���Ҷ�ӽڵ�,��ɾ����ֵ
				if (p_mxml_node_top != NULL && p_mxml_node_top->child != NULL && p_mxml_node_top->child->type != MXML_ELEMENT) {
					mxmlRemove(p_mxml_node_top->child);
					mxmlDelete(p_mxml_node_top->child);
				}

				// �����ڵ�
				p_mxml_node = mxmlNewElement(p_mxml_node_top, name);
				if (r < q)
					mymxmlAccessAttr(p_mxml_node, attr_str, 1);
			}
			p_mxml_node_top = p_mxml_node;
		}

		// pָ��·����һ���ͷ
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
