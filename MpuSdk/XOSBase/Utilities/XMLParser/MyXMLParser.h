/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: MyXMLParser.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2007-7-31 14:13:56
 *  ������ע: 
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

	/*  ��������: ����XML���Ľ��з���,���Ҵ���MyXMLParser��������
	 *  ����˵��:
	 *      const char *lpszXML [IN]:	��Ҫ������XML����
	 */
	bool Create(const char *lpszXML);

	/*  ��������: ��XML���ĺϳ������������
	 *  ����˵��:
	 *      const char **szaFileHeader [IN]:	������ӵ��ı�ͷ,���û����NULL
	 *      char *pszBuffer [IN]:				����Ļ����ַ
	 *      int iBufferLen [IN]:				����������󳤶�
	 *  ����ֵ: ʵ�ʺϳ��ַ����ĳ���.
	 */
	int WriteToBuffer(const char **szaFileHeader, char *pszBuffer, int iBufferLen);

	/*  ��������: ���õ�ǰ������·��
	 *  ����˵��:
	 *      const char *lpszPath [IN]:	��ǰ����·��,����·��,tag0Ϊ·��¼�ĵ�һ����·��
	 *				1. tag0/tag1.id0/tag2/..., ����tag0,tag1,tag2,...ΪXML��ǩ������,
	 *				   '.'�����id0��ʾ���ڶ��ͬ���ı�ʶ��ʱ��,��ʶҪ�ҵ�tag��id
	 *				2. tag0/tag1[idx]/tag2/..., ����tag0,tag1,tag2,...ΪXML��ǩ������,
	 *				   []�����idx��ʾ���ڶ��ͬ���ı�ʶ��ʱ��,Ҫ�ҵ�tag��λ�����,��0��ʼ.
	 *  ��ע: ȱʡ����·��Ϊ���ڵ�
	 */
	void SetCurPath(const char *lpszFullPath);

	/*  ��������: ��ȡָ��·����XML��ǩ��ֵ
	 *  ����˵��:
	 *      const char *lpszPath [IN]:	��ǩ���ڵ����·��,""��ʾ��ǰ·��,���庬������:
	 *  ����ֵ: ��ǩֵ����������ַ
	 */
	const char *GetValue(const char *lpszPath);

	/*  ��������: ����ָ��·����XML��ǩ��ֵ
	 *  ����˵��:
	 *      const char *lpszPath [IN]:	ͬ��
	 *      const char *lpszValue [IN]:	��ǩ��ֵ
	 */
	bool SetValue(const char *lpszPath, const char *lpszValue);

	/*  ��������: ��ȡָ��·����XML��ǩ������
	 *  ����˵��:
	 *      const char *lpszPath [IN]:		ͬ��
	 *      const char *lpszAttrName [IN]:	���Ե�����
	 *  ����ֵ: ��ǩ���Ե���������ַ
	 */
	const char *GetAttributeValue(const char *lpszPath, const char *lpszAttrName);

	/*  ��������: ����ָ��·����XML��ǩ������
	 *  ����˵��:
	 *      const char *lpszPath [IN]:		ͬ��
	 *      const char *lpszAttrName [IN]:	���Ե�����
	 *		const char *lpszAttrValue [IN]:	���Ե�ֵ
	 */
	bool SetAttributeValue(const char *lpszPath, const char *lpszAttrName, const char *lpszAttrValue);

	/*  ��������: ��ȡָ��·����XML��ǩ����Ƕ��ǩ�ĸ���
	 *  ����˵��:
	 *      const char *lpszPath [IN]:		ͬ��
	 *  ����ֵ: ָ��·����XML��ǩ����Ƕ��ǩ�ĸ���
	 */
	int GetEmbeddedTagNum(const char *lpszPath);

	/*  ��������: �Ƴ�ָ���ı�ǩ
	 *  ����˵��:
	 *      const char *lpszPath [IN]:		ͬ��
	 *  ����ֵ: �Ƴ�������XML��ǩ
	 */
	XMLTag *RemoveTag(const char *lpszPath);

	/*  ��������: ɾ��ָ���ı�ǩ
	 *  ����˵��:
	 *      const char *lpszPath [IN]:		ͬ��
	 */
	void DeleteTag(const char *lpszPath);

private:
	XMLParser* m_pParser;
	char m_szCurPath[MYXMLPARSER_MAX_CURPATH_LEN];

};

#endif //__MYXMLPARSER_H__
