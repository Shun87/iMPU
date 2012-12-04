#include <stdio.h>
#include "../wordParser.h"

static char pDest[MAXWORDLENGHTH];
static char* pSource;

char* WPGetWord(char* text)
{
	char* p;
	int len;
	if (text == NULL)
	{
		//get next word
	}
	else
	{
		//get first word form text pointed by text
		pSource = text;	
	}

	//find begin of string 
//	while (((*pSource)=='\n') ||
//		   ((*pSource)=='\r') ||
//		   ((*pSource)=='\t') || 
//		   ((*pSource)==' '))
	while (((*pSource)=='\n') ||
		   ((*pSource)=='\r') ||
		   ((*pSource)=='\t'))
	{
		pSource++;
	}
	if (*pSource=='\0')
	{
		return NULL;
	}
	p = pDest;
	len = 0;

	//find end of string 
	while (((*pSource)!='\n') &&
		   ((*pSource)!='\r') &&
		   ((*pSource)!='\t') && 
		   ((*pSource)!='\0') && 
		   ((*pSource)!=' '))
	{
		//把'>'和'<'当成一个word单独返回
		if (((*pSource) == '>') ||
			((*pSource) == '<'))
		{
			if (pDest == p)
			{
				*p = *pSource;
				p++;
				len++;
				pSource++;
			}
			*p = '\0';
			return pDest;
		}
		*p = *pSource;
		if(len < MAXWORDLENGHTH)
		{
			p++;
			len++;
		}
		pSource++;
	}
	
	*p = '\0';
	return pDest;
}
