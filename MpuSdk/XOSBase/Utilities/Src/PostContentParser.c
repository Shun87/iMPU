/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: PostContentParser.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-05 19:32:34
 *  修正备注: 
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
