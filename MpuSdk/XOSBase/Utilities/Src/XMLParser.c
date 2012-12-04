/************************************************************************/
/*                            注释                                    *  /

	xml报文中除了value中有空格,其它地方都不能有空格,换行使用tab键.

/************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../XMLParser.h"

/************************************************************************/
/*               error message                                          */
/************************************************************************/
static char* err_msg[] = {
/* 0 */	"NULL XML",
/* 1 */	"leaf node doesn't have value!\n",
/* 2 */	"buffer is too small!\n"
/* 3 */
};

/************************************************************************/
/*				   XML class                                            */
/************************************************************************/
static char *XMLParseHeader(char *pXML)
{
	char *p;
	p = strstr(pXML, "?>");
	if (p == NULL) {
		return pXML;
	}

	p = strstr(p, "<");

	return p;
}

PXMLTAG XMLParse(char *pXML)
{
	PXMLTAG rt = NULL, pNode = NULL;
	char* str;

	//跳过XML语句的声明部分
	pXML = XMLParseHeader(pXML);
	
	//开始分析正文
	str = WPGetWord(pXML);
	while (str != NULL)
	{
		if (strcmp(str, "<") == 0)
		{
			//get tag name
			str = WPGetWord(NULL);
			if ((*str) != '/')
			{
				//left tag name
				if (rt == NULL)
				{
					rt = (PXMLTAG)malloc(sizeof(XMLTAG));
					if (rt == NULL)
					{
						printf("malloc error\n");
						return NULL;
					}
					memset(rt, 0, sizeof(XMLTAG));
					pNode = rt;
				}
				else
				{
					if (pNode->pChild == NULL)
					{
						pNode->pChild = (PXMLTAG)malloc(sizeof(XMLTAG));
						if (pNode->pChild == NULL)
						{
							printf("malloc error\n");
							XMLDestroy(rt);
							return NULL;
						}
						memset(pNode->pChild, 0, sizeof(XMLTAG));
						pNode->pChild->pParent = pNode;
						pNode = pNode->pChild;
					}
					else
					{
						PXMLTAG p = pNode->pChild;
						while (p->pBrother != NULL)
						{
							p = p->pBrother;
						}
						p->pBrother = (PXMLTAG)malloc(sizeof(XMLTAG));
						if (p->pBrother == NULL)
						{
							printf("malloc error\n");
							XMLDestroy(rt);
							return NULL;
						}
						memset(p->pBrother, 0, sizeof(XMLTAG));
						pNode = p->pBrother;
						pNode->pParent = p->pParent;
					}
				}
				memcpy(pNode->tagname, str, MAXNAMELEN-1);
			}
			else 
			{
				//right tag name
				str += 1;
				if (pNode == NULL)
				{
					printf("XML should't begin with </...>\n");
					return NULL;
				}
				if (strcmp(str, pNode->tagname) != 0)
				{
					printf("XML letf tag name mismatch right tag name\n");
					XMLDestroy(rt);
					return NULL;
				}
				else
				{
					if (pNode == rt)
					{
						return rt;
					}
					else
					{
						/*
						if (pNode->value == NULL && pNode->pChild == NULL)
						{
							printf("the leafnode %s don't have value or child\n", pNode->tagname);
							XMLDestroy(rt);
							return NULL;
						}
						*/
						if (pNode->pChild == NULL)
						{
							if (pNode->value == NULL)
							{
								pNode->value = (char*)malloc(MAXVALUELEN);
								if (pNode->value == NULL)
								{
									printf("malloc error\n");
									XMLDestroy(rt);
									return NULL;
								}
								memset(pNode->value, 0, MAXVALUELEN);
							}
						}
						pNode= pNode->pParent;
					}
				}
			}
			
			str = WPGetWord(NULL);
			if (strcmp(str, ">") != 0)
			{
				printf("node %s less a '>'\n", pNode->tagname);
				XMLDestroy(rt);
				return NULL;
			}		
		}
		else
		{
			//test that if it is a legal value
			if (pNode->value != NULL)
			{
				printf("%s have too many values\n", pNode->tagname);
				XMLDestroy(rt);
				return NULL;
			}
			else
			{
				pNode->value = (char*)malloc(MAXVALUELEN);
				if (pNode->value == NULL)
				{
					printf("malloc error\n");
					XMLDestroy(rt);
					return NULL;
				}
				memset(pNode->value, 0, MAXVALUELEN);
				memcpy(pNode->value, str, MAXVALUELEN-1);
			}
		}

		str = WPGetWord(NULL);
	}

	printf("XML error\n");
	XMLDestroy(rt);
	return NULL;
}

//返回添加后还剩下的长度,0表示溢出
static int stradd(char *str, char *addstr, int leftlen)
{
	if ((int)strlen(addstr) >= leftlen)
	{
		return 0;
	}
	strcat(str, addstr);
	return (leftlen-strlen(addstr));
}

int XMLCompose(PXMLTAG XMLTree, char* pSentence, int totaltlen)
{
	static int depth = 0;
	int i;
	int leftlen = totaltlen;
	PXMLTAG pNode = XMLTree, p;

	pSentence[0] = '\0';

	if (XMLTree == NULL)
	{
		printf(err_msg[0]);
		return 0;
	}
	else
	{
		//add left tag name
		for (i=depth; i>0; i--)
		{
			leftlen = stradd(pSentence, "\t", leftlen);
			if (leftlen == 0)
			{
				printf(err_msg[2]);
				return 0;
			}
		}
		if (((leftlen=stradd(pSentence, "<", leftlen)) == 0) ||
		    ((leftlen=stradd(pSentence, pNode->tagname, leftlen)) == 0) ||
			((leftlen=stradd(pSentence, ">", leftlen)) == 0))
		{
			printf(err_msg[2]);
			return 0;
		}

		//add tag value or sub tag
		if (pNode->pChild != NULL)
		{
			//add tag sub tag
			leftlen = stradd(pSentence, "\r\n", leftlen);
			p = pNode->pChild;
			depth ++;	
			while (p != NULL) 
			{
				int templen;
				templen = XMLCompose(p, pSentence+(totaltlen-leftlen), leftlen);
				if (templen == 0)
				{
					depth --;
					return 0;
				}
				leftlen -= templen;	
				p = p->pBrother;
			}
			depth --;
			for (i=depth; i>0; i--)
			{
				leftlen = stradd(pSentence, "\t", leftlen);
				if (leftlen == 0)
				{
					printf(err_msg[2]);
					return 0;
				}
			}
		}
		else //if (pNode->pChild == NULL)
		{
			//add tag value
			if (pNode->value == NULL)
			{
				//if it doesn't have value, error
				printf(err_msg[1]);
				return 0;
			}
			leftlen = stradd(pSentence, pNode->value, leftlen);
			if (leftlen == 0)
			{
				printf(err_msg[2]);
				return 0;
			}
		}
		
		//add right tag name
		if (((leftlen=stradd(pSentence, "</", leftlen)) == 0) ||
		    ((leftlen=stradd(pSentence, pNode->tagname, leftlen)) == 0) ||
			((leftlen=stradd(pSentence, ">\r\n", leftlen)) == 0))
		{
			printf(err_msg[2]);
			return 0;
		}

		return (totaltlen-leftlen);	
	}
}

PXMLTAG XMLTag_Create(char *pName)
{
	PXMLTAG rtXMLTag;
	rtXMLTag = (PXMLTAG)malloc(sizeof(XMLTAG));
	if (rtXMLTag != NULL)
	{
		memset(rtXMLTag, 0, sizeof(XMLTAG));
		memcpy(rtXMLTag->tagname, pName, MAXNAMELEN-1);
		return rtXMLTag;
	}
	else
	{
		return NULL;
	}
}

PXMLTAG XMLTag_CreateLeaf(char *pName, char *pValue)
{
	PXMLTAG rtXMLTag;
	rtXMLTag = (PXMLTAG)malloc(sizeof(XMLTAG));
	if (rtXMLTag != NULL)
	{
		memset(rtXMLTag, 0, sizeof(XMLTAG));
		memcpy(rtXMLTag->tagname, pName, MAXNAMELEN-1);
		rtXMLTag->value = (char*)malloc(MAXVALUELEN);
		if(rtXMLTag->value != NULL)
		{
			memset(rtXMLTag->value, 0, MAXVALUELEN);
			memcpy(rtXMLTag->value, pValue, MAXVALUELEN-1);
			return rtXMLTag;
		}
		else
		{
			free(rtXMLTag);
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void XMLDestroy(PXMLTAG XMLTree)
{
	if (XMLTree == NULL)
	{
		return;
	}

	//printf("%s\r\n", XMLTree->tagname);
	
	if (XMLTree->value != NULL)
	{
		free(XMLTree->value);
	}

	if (XMLTree->pBrother != NULL)
	{
		XMLDestroy(XMLTree->pBrother);
	}

	if (XMLTree->pChild != NULL)
	{
		XMLDestroy(XMLTree->pChild);
	}

	free(XMLTree);
}

char *XMLTag_GetName(PXMLTAG pXMLTag)
{
	if (pXMLTag != NULL)
	{
		return pXMLTag->tagname;
	}
	else
	{
		return NULL;
	}
}

char *XMLTag_GetValue(PXMLTAG pXMLTag)
{
	if (pXMLTag != NULL)
	{
		return pXMLTag->value;
	}
	else
	{
		return NULL;
	}
}

void XMLTag_SetName (PXMLTAG pXMLTag, char *pName)
{
	if (pXMLTag != NULL)
	{
		memset(pXMLTag->tagname, 0, MAXNAMELEN);
		memcpy(pXMLTag->tagname, pName, MAXNAMELEN-1);
	}
}

void XMLTag_SetValue(PXMLTAG pXMLTag, char *pValue)
{
	if (pXMLTag != NULL)
	{
		if (pXMLTag->value != NULL)
		{	
			memcpy(pXMLTag->value, pValue, MAXVALUELEN-1);
		}
	}
}

int XMLTag_GetSubTagNum(PXMLTAG pXMLTag)
{
	PXMLTAG p;
	int rt = 0;
	if (pXMLTag != NULL)
	{
		if (pXMLTag->pChild != 0)
		{
			p = pXMLTag->pChild;
			while (p != NULL)
			{
				p = p->pBrother;
				rt++;
			}
			return rt;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void XMLTag_AddSubTag(PXMLTAG pXMLTag, PXMLTAG pSubXMLTAG)
{
	PXMLTAG p;
	if ((pXMLTag != NULL) && (pSubXMLTAG != NULL))
	{
		if (pXMLTag->pChild == NULL)
		{
			pXMLTag->pChild = pSubXMLTAG;
			pSubXMLTAG->pParent = pXMLTag;
		}
		else
		{
			p = pXMLTag->pChild;
			while (p->pBrother != NULL)
			{
				p = p->pBrother;
			}
			p->pBrother = pSubXMLTAG;
			pSubXMLTAG->pParent = p->pParent;
			pSubXMLTAG->pBrother = NULL;
		}
	}
	else
	{
		if (pSubXMLTAG != NULL)
		{
			XMLDestroy(pSubXMLTAG);
		}
	}
}

void XMLTag_DeleteSubTag(PXMLTAG pXMLTag, PXMLTAG pSubTag)
{
	XMLTag_RemoveSubTag(pXMLTag, pSubTag);
	XMLDestroy(pSubTag);
}

void XMLTag_RemoveSubTag(PXMLTAG pXMLTag, PXMLTAG pSubTag)
{
	PXMLTAG p;

	if ((pXMLTag != NULL) && (pSubTag != NULL))
	{
		if (pXMLTag->pChild == NULL)
		{
			return;
		}
		else
		{
			p = pXMLTag->pChild;
			if (p == pSubTag)
			{
				pXMLTag->pChild = pXMLTag->pChild->pBrother;
				pSubTag->pParent = NULL;
				pSubTag->pBrother = NULL;
				return;
			}	
			else
			{
				while (p != NULL)
				{
					if (p->pBrother == pSubTag)
					{
						p->pBrother = p->pBrother->pBrother;	
						pSubTag->pParent = NULL;
						pSubTag->pBrother = NULL;
						return;
					}
					p = p->pBrother;
				}
			}
		}
	}
	else
	{
		return;
	}
}

PXMLTAG XMLTag_FindSubTag(PXMLTAG pXMLTag, char *pName)
{
	PXMLTAG p;
	if (pXMLTag != NULL)
	{
		if (pXMLTag->pChild == NULL)
		{
			return NULL;
		}
		else
		{
			p = pXMLTag->pChild;
			while (p != NULL)
			{
				if (strcmp(p->tagname, pName) == 0)
				{
					return p;
				}
				p = p->pBrother;
			}
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

PXMLTAG XMLTag_GetSubTag(PXMLTAG pXMLTag, int idx)
{
	PXMLTAG p;
	int count = 0;
	if (pXMLTag != NULL)
	{
		if (pXMLTag->pChild == NULL)
		{
			return NULL;
		}
		else
		{
			p = pXMLTag->pChild;
			while (p != NULL)
			{
				if (count == idx)
				{
					return p;
				}
				p = p->pBrother;
				count ++;
			}
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void XMLTag_DeleteSubTagByIdx(PXMLTAG pXMLTag, int idx)
{
	XMLTag_DeleteSubTag(pXMLTag, XMLTag_GetSubTag(pXMLTag, idx));
}

void XMLTag_RemoveSubTagByIdx(PXMLTAG pXMLTag, int idx)
{
	XMLTag_RemoveSubTag(pXMLTag, XMLTag_GetSubTag(pXMLTag, idx));
}

void XMLTag_DeleteSubTagByName(PXMLTAG pXMLTag, char *pName)
{
	XMLTag_DeleteSubTag(pXMLTag, XMLTag_FindSubTag(pXMLTag, pName));
}

void XMLTag_RemoveSubTagByName(PXMLTAG pXMLTag, char *pName)
{
	XMLTag_RemoveSubTag(pXMLTag, XMLTag_FindSubTag(pXMLTag, pName));
}
