/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: Base64.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-02-16 08:39:34
 *  修正备注: 
 *  
 */

#include "../Base64.h"
#include <ctype.h>
#include <string.h>

const unsigned char c_Base64Table[64] = 
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', '+', '/'
};

int Base64_Encode(char *pszEncoded, char *pData, int iSize)
{
	int DataIdx;
	int EncodedIdx;

	EncodedIdx = 0;
	DataIdx = 0;

	while (iSize >= 3) {
		pszEncoded[EncodedIdx] = c_Base64Table[((unsigned char)pData[DataIdx] >> 2)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = c_Base64Table[(((unsigned char)pData[DataIdx] & 0x03) << 4) + ((unsigned char)pData[DataIdx+1] >> 4)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = c_Base64Table[(((unsigned char)pData[DataIdx+1] & 0x0F) << 2) + ((unsigned char)pData[DataIdx+2] >> 6)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = c_Base64Table[((unsigned char)pData[DataIdx+2] & 0x3F)];
		EncodedIdx ++;

		DataIdx += 3;
		iSize -= 3;
	}

	if (iSize == 2) {
		pszEncoded[EncodedIdx] = c_Base64Table[((unsigned char)pData[DataIdx] >> 2)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = c_Base64Table[(((unsigned char)pData[DataIdx] & 0x03) << 4) + ((unsigned char)pData[DataIdx+1] >> 4)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = c_Base64Table[(((unsigned char)pData[DataIdx+1] & 0x0F) << 2)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = '=';
		EncodedIdx ++;
	}
	else if (iSize == 1) {
		pszEncoded[EncodedIdx] = c_Base64Table[((unsigned char)pData[DataIdx] >> 2)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = c_Base64Table[(((unsigned char)pData[DataIdx] & 0x03) << 4)];
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = '=';
		EncodedIdx ++;
		pszEncoded[EncodedIdx] = '=';
		EncodedIdx ++;
	}
	return EncodedIdx;
}

static unsigned char Base64_Decode1Byte(char Data)
{
	char c = 0;

	if(isalpha(Data))	{
		if(isupper(Data))
			c = Data - 'A' + 0;
		else
			c = Data - 'a' + 26;
	}
	else if(isdigit(Data)) {
		c = Data - '0' + 52;
	}
	else if(Data == '+') {
		c =  62;
	}
	else if(Data == '/') {
		c =  63;
	}

	return c;
}

int Base64_Decode(char *pszDecoded, char *pData, int iSize)
{
	int DataIdx;
	int DecodedIdx;

	DecodedIdx = 0;
	DataIdx = 0;

	if ((strlen(pszDecoded) % 4) != 0) {
		//不是4字节的整数倍,返回
		return DecodedIdx;
	}

	while (iSize > 0) {
		if (pData[DataIdx] == '=') {
			return DecodedIdx;
		}
		pszDecoded[DecodedIdx] = (unsigned char)(Base64_Decode1Byte(pData[DataIdx+0]) << 2) + (unsigned char)(Base64_Decode1Byte(pData[DataIdx+1]) >> 4);
		DecodedIdx ++;

		if (pData[DataIdx+2] == '=') {
			return DecodedIdx;
		}
		pszDecoded[DecodedIdx] = (unsigned char)((Base64_Decode1Byte(pData[DataIdx+1]) & 0x0F) << 4) + (unsigned char)(Base64_Decode1Byte(pData[DataIdx+2]) >> 2);
		DecodedIdx ++;

		if (pData[DataIdx+3] == '=') {
			return DecodedIdx;
		}
		pszDecoded[DecodedIdx] = (unsigned char)((Base64_Decode1Byte(pData[DataIdx+2]) & 3) << 6) + (unsigned char)(Base64_Decode1Byte(pData[DataIdx+3]));
		DecodedIdx ++;

		DataIdx += 4;
		iSize -= 4;
	}

	return DecodedIdx;
}
