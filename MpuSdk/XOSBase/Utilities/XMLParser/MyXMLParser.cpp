/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: MyXMLParser.cpp
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-7-31 14:13:51
 *  修正备注: 
 *  
 */


#include "MyXMLParser.h"

static XMLTag *FindSubXMLTag(XMLTag *pXMLTag, const char *lpszTagPath, XMLTag **ppSuperTag = NULL)
{
	char *p;
	UInt32 ui;
	char *pTagName;
	char *pId;
	char *pNo;
	XMLTag *pSuperTag = NULL;
	char szTagPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";

	if (strlen(lpszTagPath) >= MYXMLPARSER_MAX_CURPATH_LEN)
	{
		return NULL;
	}
	szTagPath[0] = 0;
	strcpy(szTagPath, lpszTagPath);
	
	if (strcmp(szTagPath, "") != 0)
	{
		p = szTagPath;
		pSuperTag = pXMLTag;
		while ((p != NULL) && (pXMLTag != NULL))
		{
			pTagName = p;
			p = strtok(p, "/");
			p = strtok(NULL, "\0");
			pTagName = strtok(pTagName, ".");
			pId = strtok(NULL, "\0");
			if (pId == NULL)
			{
				pTagName = strtok(pTagName, "[");
				pNo = strtok(NULL, "]");
			}
			
			if (pId != NULL)
			{
				//printf("%s.%s\r\n", pTagName, pId);
				for (ui=0; ui<pSuperTag->GetNumEmbeddedTags(); ui++)
				{
					XMLTag *pIdTag;
					pXMLTag = pSuperTag->GetEmbeddedTag(ui);
					pIdTag = pXMLTag->GetEmbeddedTagByName("id");
					if (pIdTag == NULL)
					{
						continue;
					}
					if (strcmp(pId, pIdTag->GetValue()) != 0)
					{
						continue;
					}
					break;
				}
				if (ui == pSuperTag->GetNumEmbeddedTags())
				{	
					pXMLTag = NULL;
					break;
				}
			}
			else if (pNo != NULL)
			{
				//printf("%s[%s]\r\n", pTagName, pNo);
				ui = atoi(pNo);
				pXMLTag = pSuperTag->GetEmbeddedTag(ui);
			}
			else
			{
				//printf("%s\r\n", pTagName);
				pXMLTag = pXMLTag->GetEmbeddedTagByName(pTagName);
			}
			if (p != NULL)
			{
				pSuperTag = pXMLTag;
			}
		}
	}

	if (ppSuperTag != NULL)
	{
		*ppSuperTag = pSuperTag;
	}

	return pXMLTag;
}

static XMLTag *InsertSubXMLTag(XMLTag *pXMLTag, const char *lpszTagPath, const char *lpszValue = NULL, XMLTag **ppSuperTag = NULL)
{
	char *p;
	UInt32 ui;
	char *pTagName;
	char *pId;
	XMLTag *pSuperTag = NULL;
	char szTagPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";

	if (strlen(lpszTagPath) >= MYXMLPARSER_MAX_CURPATH_LEN)
	{
		return NULL;
	}
	szTagPath[0] = 0;
	strcpy(szTagPath, lpszTagPath);
	
	if (strcmp(szTagPath, "") != 0)
	{
		p = szTagPath;
		pSuperTag = pXMLTag;
		while ((p != NULL) && (pXMLTag != NULL))
		{
			pTagName = p;
			p = strtok(p, "/");
			p = strtok(NULL, "\0");
			pTagName = strtok(pTagName, ".");
			pId = strtok(NULL, "\0");
			
			if (pId != NULL)
			{
				//printf("%s.%s\r\n", pTagName, pId);
				for (ui=0; ui<pSuperTag->GetNumEmbeddedTags(); ui++)
				{
					XMLTag *pIdTag;
					pXMLTag = pSuperTag->GetEmbeddedTag(ui);
					pIdTag = pXMLTag->GetEmbeddedTagByName("id");
					if (pIdTag == NULL)
					{
						continue;
					}
					if (strcmp(pId, pIdTag->GetValue()) != 0)
					{
						continue;
					}
					break;
				}
				if (ui == pSuperTag->GetNumEmbeddedTags())
				{	
					XMLTag *pIdTag;
					pXMLTag = new XMLTag(pTagName);
					if (pXMLTag != NULL)
					{
						pIdTag = new XMLTag("id");
						if (pIdTag != NULL)
						{
							pIdTag->SetValue(pId);
							pXMLTag->AddEmbeddedTag(pIdTag);
							pSuperTag->AddEmbeddedTag(pXMLTag);
							break;
						}
						delete pXMLTag;
						pXMLTag = NULL;
					}
					break;
				}
			}
			else
			{
				//printf("%s\r\n", pTagName);
				pXMLTag = pXMLTag->GetEmbeddedTagByName(pTagName);
				if (pXMLTag == NULL)
				{
					pXMLTag = new XMLTag(pTagName);
					if (pXMLTag != NULL)
					{
						pSuperTag->AddEmbeddedTag(pXMLTag);
					}
				}
			}
			if (p != NULL)
			{
				pSuperTag = pXMLTag;
			}
		}
	}

	if (ppSuperTag != NULL)
	{
		*ppSuperTag = pSuperTag;
	}

	if ((pXMLTag != NULL) && (lpszValue != NULL))
	{
		pXMLTag->SetValue((char *)lpszValue);
	}

	return pXMLTag;
}

CMyXMLParser::CMyXMLParser()
{
	m_pParser = NULL;
	memset(m_szCurPath, 0, MYXMLPARSER_MAX_CURPATH_LEN);
}

CMyXMLParser::~CMyXMLParser()
{
	if (m_pParser != NULL)
	{
		delete m_pParser;
		m_pParser = NULL;
	}
}

bool CMyXMLParser::Create(const char *lpszXML)
{
	static char szErrBuf[128];
	m_pParser = new	XMLParser;
	if (m_pParser == NULL)
	{
		return false;
	}
	if (!m_pParser->ParseString(szErrBuf, 128, (char *)lpszXML, strlen(lpszXML)))
	{
		return false;
	}
	return true;
}

int CMyXMLParser::WriteToBuffer(const char **szaFileHeader, char *pszBuffer, int iBufferLen)
{
	return m_pParser->WriteToBuffer((char **)szaFileHeader, pszBuffer, iBufferLen);
}

void CMyXMLParser::SetCurPath(const char *lpszFullPath)
{
	int iCopyLen;
	iCopyLen = strlen(lpszFullPath);
	if (iCopyLen >= MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		iCopyLen = MYXMLPARSER_MAX_CURPATH_LEN/2-1;
	}
	memcpy(m_szCurPath, 0, MYXMLPARSER_MAX_CURPATH_LEN);
	memcpy(m_szCurPath, lpszFullPath, iCopyLen);
}

const char * CMyXMLParser::GetValue(const char *lpszPath)
{
	char szPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";
	if (strlen(lpszPath) > MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		return "";
	}
	if (strcmp(m_szCurPath, "") != 0)
	{
		strcat(szPath, m_szCurPath);
		strcat(szPath, "/");
	}
	strcat(szPath, lpszPath);

	XMLTag *pXMLTag = FindSubXMLTag(m_pParser->GetRootTag(), szPath);
	if(pXMLTag == NULL)
	{
		return "";
	}
	return pXMLTag->GetValue();
}

bool CMyXMLParser::SetValue(const char *lpszPath, const char *lpszValue)
{
	char szPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";
	if (strlen(lpszPath) > MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		return false;
	}
	if (strcmp(m_szCurPath, "") != 0)
	{
		strcat(szPath, m_szCurPath);
		strcat(szPath, "/");
	}
	strcat(szPath, lpszPath);

	XMLTag *pXMLTag = InsertSubXMLTag(m_pParser->GetRootTag(), szPath);
	if(pXMLTag == NULL)
	{
		return false;
	}

	pXMLTag->SetValue((char *)lpszValue);

	return true;
}

const char * CMyXMLParser::GetAttributeValue(const char *lpszPath, const char *lpszAttrName)
{
	char *pValue;
	char szPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";
	if (strlen(lpszPath) > MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		return "";
	}
	if (strcmp(m_szCurPath, "") != 0)
	{
		strcat(szPath, m_szCurPath);
		strcat(szPath, "/");
	}
	strcat(szPath, lpszPath);

	XMLTag *pXMLTag = FindSubXMLTag(m_pParser->GetRootTag(), szPath);
	if(pXMLTag == NULL)
	{
		return "";
	}
	pValue = pXMLTag->GetAttributeValue(lpszAttrName);
	if (pValue == NULL)
	{
		return "";
	}
	else
	{
		return pValue;
	}
}

bool CMyXMLParser::SetAttributeValue(const char *lpszPath, const char *lpszAttrName, const char *lpszAttrValue)
{
	char szPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";
	if (strlen(lpszPath) > MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		return false;
	}
	if (strcmp(m_szCurPath, "") != 0)
	{
		strcat(szPath, m_szCurPath);
		strcat(szPath, "/");
	}
	strcat(szPath, lpszPath);

	XMLTag *pXMLTag = InsertSubXMLTag(m_pParser->GetRootTag(), szPath);
	if(pXMLTag == NULL)
	{
		return false;
	}

	pXMLTag->RemoveAttribute((char *)lpszAttrName);
	pXMLTag->AddAttribute((char *)lpszAttrName, (char *)lpszAttrValue);

	return true;
}

int CMyXMLParser::GetEmbeddedTagNum(const char *lpszPath)
{
	char szPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";
	if (strlen(lpszPath) > MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		return 0;
	}
	if (strcmp(m_szCurPath, "") != 0)
	{
		strcat(szPath, m_szCurPath);
		strcat(szPath, "/");
	}
	strcat(szPath, lpszPath);

	XMLTag *pXMLTag = FindSubXMLTag(m_pParser->GetRootTag(), szPath);
	if(pXMLTag == NULL)
	{
		return 0;
	}
	return pXMLTag->GetNumEmbeddedTags();
}

XMLTag *CMyXMLParser::RemoveTag(const char *lpszPath)
{
	char szPath[MYXMLPARSER_MAX_CURPATH_LEN] = "";
	if (strlen(lpszPath) > MYXMLPARSER_MAX_CURPATH_LEN/2)
	{
		return 0;
	}
	if (strcmp(m_szCurPath, "") != 0)
	{
		strcat(szPath, m_szCurPath);
		strcat(szPath, "/");
	}
	strcat(szPath, lpszPath);

	XMLTag *pSuperTag = NULL;
	XMLTag *pXMLTag = FindSubXMLTag(m_pParser->GetRootTag(), szPath, &pSuperTag);
	if(pXMLTag == NULL)
	{
		return 0;
	}
	pSuperTag->RemoveEmbeddedTag(pXMLTag);

	return pXMLTag;
}

void CMyXMLParser::DeleteTag(const char *lpszPath)
{
	XMLTag *pXMLTag = RemoveTag(lpszPath);
	if (pXMLTag != NULL)
	{
		delete pXMLTag;
	}
}
