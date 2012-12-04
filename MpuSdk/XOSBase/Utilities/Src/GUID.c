/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: GUID.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-02 16:29:09
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../GUID.h"
#include "../Str2Hex.h"

static char s_szGuidString[35];
static TCGUID s_guid;
static int s_bDecoder = 0;

void GUIDInit(int bDecoder)
{
	s_bDecoder = bDecoder;
}

XOS_PSZ GUID2String(const TCGUID *pGuid)
{
	XOS_U8 *puc;
	s_szGuidString[0] = 0;
	puc = (XOS_U8 *)pGuid->uGUID.GUID;
	sprintf(s_szGuidString,
			"0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
			puc[ 0], puc[ 1], puc[ 2], puc[ 3], puc[ 4], puc[ 5], puc[ 6], puc[ 7],
			puc[ 8], puc[ 9], puc[10], puc[11], puc[12], puc[13], puc[14], puc[15]
			);
	return s_szGuidString;
}

TCGUID *String2GUID(XOS_CPSZ lpszGuidString)
{
	if (strlen(lpszGuidString) != 34)
	{
		return NULL;
	}

	if (!Str2Hex((char *)lpszGuidString+2, (char *)&s_guid))
	{
		return NULL;
	}

	return &s_guid;
}

XOS_BOOL TypeAndNo2GUID(TCGUID *pGuid, XOS_CPSZ lpszType, int iNo, const char *pMAC)
{
	memset(pGuid, 0, sizeof(TCGUID));

	//平台类型
	if (s_bDecoder)
	{
		GUID_PLATFORMTYPE(pGuid) = 0x04;
	}
	else
	{
		GUID_PLATFORMTYPE(pGuid) = 0x03;
	}

	//资源类型和资源子类型
	if (strcmp(lpszType, "Station") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x00;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Input Video") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x01;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Input Audio") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x02;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Output Video") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x03;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Output Audio") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x04;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Serial Port") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x05;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "PTZ") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x05;
		GUID_SUBRESOURCETYPE(pGuid) = 0x01;
	}
	else if (strcmp(lpszType, "Input Digital Line") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x06;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Output Digital Line") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x07;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Speaker") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x09;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Storager") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x0A;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Archiver") == 0) 
	{
		GUID_RESOURCETYPE(pGuid) = 0x0A;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "Platform Manager") == 0)
	{
		GUID_RESOURCETYPE(pGuid) = 0x0C;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else if (strcmp(lpszType, "User") == 0)
	{
		GUID_PLATFORMTYPE(pGuid) = 0x0A;
		GUID_RESOURCETYPE(pGuid) = 0x00;
		GUID_SUBRESOURCETYPE(pGuid) = 0;
	}
	else {
		return XOS_FALSE;
	}

	//资源编号 
	GUID_ID(pGuid) = iNo;

	memcpy(GUID_MAC(pGuid), pMAC, 6);

	return XOS_TRUE;
}

XOS_BOOL GUID2TypeAndIdx(XOS_PSZ szType, int iTypeLen, int *piNo, TCGUID *pGuid)
{
	if (GUID_PLATFORMTYPE(pGuid) != 0x01)
	{
		return XOS_FALSE;
	}
	if (szType != 0)
	{
		if (iTypeLen < 24)
		{
			return XOS_FALSE;
		}
		szType[0] = 0;
		switch(GUID_RESOURCETYPE(pGuid))
		{
			case 0x00:
				strcpy(szType, "Station");
				break;
			case 0x01:
				strcpy(szType, "Input Video");
				break;
			case 0x02:
				strcpy(szType, "Input Audio");
				break;
			case 0x03:
				strcpy(szType, "Output Video");
				break;
			case 0x04:
				strcpy(szType, "Output Audio");
				break;
			case 0x05:
				switch(*(short *)&GUID_SUBRESOURCETYPE(pGuid)) {
					case 0:
						strcpy(szType, "Serial Port");
						break;
					case 1:
						strcpy(szType, "PTZ");
						break;
					default:
						return XOS_FALSE;
				}
				break;
			case 0x06:
				strcpy(szType, "Input Digital Line");
				break;
			case 0x07:
				strcpy(szType, "Output Digital Line");
				break;
			case 0x09:
				strcpy(szType, "Speaker");
				break;
			case 0x0A:
				strcpy(szType, "Storager");
				break;
			case 0x0B:
				strcpy(szType, "Archiver");
				break;
			case 0x0C:
				strcpy(szType, "Platform Manager");
				break;
			default:
				return XOS_FALSE;
		}
	}
	*piNo = GUID_ID(pGuid);
	return XOS_TRUE;
}
