/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: Str2Hex.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: zhaoqq
 *  �������: 2007-02-01 11:23:09
 *  ������ע: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../Str2Hex.h"

static char Char2Hex(char c)
{
	if ((c >= 'a') && (c <= 'f'))
	{
		return c - 'a' + 10;
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		return c - 'A' + 10;
	}
	else if ((c >= '0') && (c <= '9'))
	{
		return c - '0';
	}
	else
	{
		return -1;
	}
}

XOS_BOOL Str2Hex(char *pStr, char *pHex)
{
	char c0,c1;
	char *hp,*sp,*sep;

	if ((strlen(pStr) % 2) != 0)
	{
		return XOS_FALSE;
	}

	sep = pStr + strlen(pStr);
	hp = pHex;
	sp = pStr;
	while (sp < sep)
	{
		c0 = Char2Hex(sp[0]);
		c1 = Char2Hex(sp[1]);
		if ((c0 == (char)-1) || (c1 == (char)-1))
		{
			return XOS_FALSE;
		}
		hp[0] = (c0 << 4) + c1;
		sp += 2;
		hp ++;
	}

	return XOS_TRUE;
}

char *Hex2Str(char *pHex, char *pStr, int iHexLen)
{
	int i;
		
	for (i=0; i<iHexLen; i++)
	{
		sprintf(pStr+2*i, "%02X", (unsigned char)*(pHex+i));
	}
	return pStr;
}

char *Hex2Strl(char *pHex, char *pStr, int iHexLen)
{
	int i;
	
	for (i=0; i<iHexLen; i++)
	{
		sprintf(pStr+2*i, "%02x", (unsigned char)*(pHex+i));
	}
	return pStr;
}
