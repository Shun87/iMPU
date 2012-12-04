/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: mymxml.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2010-4-9 10:10:03
 *  ������ע: 
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

/*  ��������: ����һ��XML����/����һ��gb2312�����XML����
 *  ����˵��:
 *      const char *version [IN]:	�汾,һ��Ϊ"1.0",���ΪNULL��û�а汾
 *  ����ֵ: mymxml_t�ṹָ��,NULL��ʾ����
 *  ��ע: versionΪNULL��û������<?xml version="1.0" encoding="UTF-8"?>
 */
mymxml_t *mymxmlNew(const char *version);
mymxml_t *mymxmlNew_gb2312(const char *version);

/*  ��������: ���ؽ���XML����
 *  ����˵��:
 *      const char *s [IN]:	��Ҫ���͵�XML����
 *  ����ֵ: mymxml_t�ṹָ��,NULL��ʾ����
 */
mymxml_t *mymxmlLoadString(const char *s);

/*  ��������: ��һ�����е�mxml�ڵ���Ϊ���ڵ����
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      mxml_node_t *p_root [IN]:	��Ϊ���ڵ�Ľڵ�
 */
void mymxmlAttachRoot(mymxml_t *p_mymxml, mxml_node_t *p_root);

/*  ��������: �ϳ�XML����
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      char *buffer [IN]:		���XML���ĵĻ���
 *      int bufsize [IN]:		buffer������ܳ���
 *  ����ֵ: �ϳɵ�XML���ĵĳ���,-1��ʾ����
 *  ��ע: ������岻��,���᷵�ش���,���ᶪʧ���������
 */
int mymxmlSaveString(mymxml_t *p_mymxml, char *buffer, int bufsize);

/*  ��������: �ϳ�XML����, �ڲ������ڴ�
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *  ����ֵ: �ϳɵı����ڴ�ָ��,����ʱҪ��free�ͷ�;NULL��ʾ����
 */
char *mymxmlSaveAllocString(mymxml_t *p_mymxml);

/*  ��������: ���ò����ĵ�ǰ·��
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	��ǰ������·��,�����'/'��ͷ��Ϊ�Ǿ���·��,����Ϊ���·����ʽ����:
 *									"/tag0/tag1<id="id0">/tag2/..." 
 *									����tag0Ϊ���ڵ�,�������������,
 *									<id="id0">��ʾ���ڶ��ͬ�������ʱ��,Ҫ�ҵı�ǩ��id����ֵΪ"id0".
 *									��������XML���ĵ�·�����Ա�ʾΪ:
 *									<?xml version="1.0" encoding="utf-8"?>
 *									<a>
 *										<b>
 *											<c id="0">
 *											<c id="1">
 *										</b>
 *										<d>value</d>
 *									</a>
 *										����: a,b,c���Ǳ�ǩ
 *											  value�Ǳ�ǩd��ֵ
 *											  id�Ǳ�ǩc������,0�Ǳ�ǩc������id��ֵ.
 *									/a/b/c<id="0">	��λ��"<c id="0">"��
 *									/a/b/c<id="1">	��λ��"<c id="1">"��
 *  ��ע: �����ǩ������,����������Զ�����һ����ǩ.���·��Խ����Ҷ�ӽڵ�,Ҷ�ӽڵ��ֵ�ᱻɾ��,
 *		  ����ж������,��'&'����,��: "/tag0/tag1<id="id0"&name="name">/tag2/...".
 *		  ����mymxmlSetValue, mymxmlRemoveNode��ʱ��������Ľ��Ϊ��ǰ·���ϼ�, ��ǰ·�����ܱ��ƻ�
 */
void mymxmlSetCurPath(mymxml_t *p_mymxml, const char *path);

/*  ��������: ��ȡ�ڵ�
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	�ڵ����CurPath��·��
 *  ����ֵ: ��ȡ�Ľڵ�ָ��,NULL��ʾ������
 */
mxml_node_t *mymxmlGetNode(mymxml_t *p_mymxml, const char *path);

/*  ��������: ����ڵ�
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	�ڵ����CurPath��·��
 *		mxml_node_t *p_mxml_node_insert [IN]:	Ҫ����Ľڵ�
 *  ��ע: �����ǩ������,����������Զ�����һ����ǩ.���·��Խ����Ҷ�ӽڵ�,Ҷ�ӽڵ��ֵ�ᱻɾ��
 *		  ��ɺ�p_mxml_node_insert����Ϊpath����ʾ�ڵ�ĵ�һ���ӽڵ�
 */
void mymxmlInsertNode(mymxml_t *p_mymxml, const char *path, mxml_node_t *p_mxml_node_insert);

/*  ��������: ��ȡ��ǩֵ
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	��ǩ���CurPath��·��
 *		const char *def [IN]: Ĭ��ֵ,������ʱ�������ֵ
 *  ����ֵ: ��ȡ�ı�ǩֵ,��ǩ�����ڷ���Ĭ��ֵ
 *  ��ע: ֻ����Ҷ�ӽڵ���ܹ���ȡ��ǩֵ.����:<a>value</a>
 */
const char *mymxmlGetValue(mymxml_t *p_mymxml, const char *path, const char *def);

/*  ��������: ���ñ�ǩֵ
			  ��ɾ��ԭ����ֵ
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	��ǩ���CurPath��·��
 *      const char *value [IN]:	��ǩֵ(������XML��ʽ���ַ���)
 *  ��ע: �����ǩ������,����������Զ�����һ����ǩ.���·��Խ����Ҷ�ӽڵ�,Ҷ�ӽڵ��ֵ�ᱻɾ��;
 *		  ����mymxmlSetValue,���Ҫ���õĽڵ㲻��Ҷ�ӽڵ�,��ɾ���˽ڵ�������ӽڵ�,ʹ���ΪҶ�ӽڵ�
 *		  �ڶ���������ɾ��ԭ����ֵ,����,value������һ���ӱ�ǩ,������ֵ,�����ֵ�Ļ�,����׷����ԭֵ����.
 */
void mymxmlSetValue(mymxml_t *p_mymxml, const char *path, const char *value);
void mymxmlSetValueNoOverlay(mymxml_t *p_mymxml, const char *path, const char *value);

/*  ��������: ��ȡ����ֵ
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	��ǩ���CurPath��·��
 *      const char *attrName [IN]:	����ֵ
 *		const char *def [IN]: Ĭ��ֵ,������ʱ�������ֵ
 *  ����ֵ: ��ȡ�ı�ǩ����,���Բ�����Ĭ��ֵ
 */
const char *mymxmlGetAttributeValue(mymxml_t *p_mymxml, const char *path, const char *attrName, const char *def);

/*  ��������: ��������ֵ
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	��ǩ���CurPath��·��
 *      const char *attrName [IN]:	������
 *      const char *attrValue [IN]:	����ֵ
 *  ��ע: �����ǩ�����Բ�����,����������Զ�����һ����ǩ������.���·��Խ����Ҷ�ӽڵ�,Ҷ�ӽڵ��ֵ�ᱻɾ��;
 */
void mymxmlSetAttributeValue(mymxml_t *p_mymxml, const char *path, const char *attrName, const char *attrValue);

/*  ��������: �Ƴ��ڵ�
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      const char *path [IN]:	�ڵ����CurPath��·��
 *  ����ֵ: �Ƴ��Ľڵ�ָ��,NULL��ʾ������
 */
mxml_node_t *mymxmlRemoveNode(mymxml_t *p_mymxml, const char *path);

/*  ��������: Ѱ�ҵ�ǰ�ڵ����һ���ֵܽڵ�
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      mxml_node_t *p_mxml_node [IN]:	��ǰ�ڵ�
 *  ����ֵ: ��һ���ֵܽڵ�,NULL��ʾû����һ���ֵܽڵ���
 */
mxml_node_t *mymxmlNextNode(mymxml_t *p_mymxml, mxml_node_t *p_mxml_node);

/*  ��������: Ѱ�ҵ�ǰ�ڵ�ĵ�һ���ӽڵ�
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *      mxml_node_t *p_mxml_node [IN]:	��ǰ�ڵ�
 *  ����ֵ: ��һ���ӽڵ�,NULL��ʾû���ӽڵ�
 */
mxml_node_t *mymxmlFirstSubNode(mymxml_t *p_mymxml, mxml_node_t *node);

/*  ��������: ��ȡ�ڵ�ı�ǩֵ
 *  ����˵��:
 *      mxml_node_t *p_mxml_node [IN]:	�ڵ�ָ��
 *      const char *def [IN]:			����ڵ�û��Valueʱ��ȱʡֵ
 *  ����ֵ: �ڵ�ı�ǩֵ
 *  ��ע: ��Ҷ�ӽڵ�,������def.
 */
const char *mymxmlGetNodeValue(mxml_node_t *p_mxml_node, const char *def);

/*  ��������: ��ȡ�ڵ������ֵ
 *  ����˵��:
 *      mxml_node_t *p_mxml_node [IN]:	mymxml_t�ṹָ��
 *      const char *attrName [IN]:	��������
 *		const char *def [IN]: Ĭ��ֵ,������ʱ�������ֵ
 *  ����ֵ: ��ȡ�ı�ǩ����ֵ,���Բ�����ʱȡĬ��ֵ
 */
const char *mymxmlGetNodeAttributeValue(mxml_node_t *p_mxml_node, const char *attrName, const char *def);

/*  ��������: �ͷ�mymxml_t�ṹָ��
 *  ����˵��:
 *      mymxml_t *p_mymxml [IN]:	mymxml_t�ṹָ��
 *  ��ע: ʹ�����֮��,�����������ͷ�,������ڴ�й¶.
 */
void mymxmlDelete(mymxml_t *p_mymxml);

#ifdef __cplusplus
}
#endif

#endif //__MYMXML_H__
