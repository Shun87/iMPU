/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: PostContentParser.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-09-05 19:32:34
 *  ������ע: 
 *  
 */

#include "../PostContentParser.h"
#include <string.h>

void PostContentParser(int *piItemNum, char **ppItems, char **ppValues, char *pszPostContent)
{
	int iMaxItemNum = *piItemNum;
	int iCurNum = 0;
	char *p = pszPostContent;
	while (1)
	{
		if (iCurNum >= iMaxItemNum)
		{
			break;
		}
		
		ppItems[iCurNum] = p;
		p = strstr(p, "=");
		if (p == 0)
		{
			break;
		}
		p[0] = 0;
		p ++ ;
		ppValues[iCurNum] = p;

		iCurNum ++;
		if (iCurNum >= iMaxItemNum)
		{
			break;
		}

		p = strstr(p, "&");
		if (p == 0)
		{
			break;
		}
		p[0] = 0;
		p ++;
	}
	*piItemNum = iCurNum;
	return ;
}
