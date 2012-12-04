/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: KeyMD5.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-02-16 10:55:08
 *  修正备注: 
 *  
 */
#include "../../XOS/XOS.h"
#include "../md5.h"
#include "../KeyMD5.h"

static unsigned char ipad[64] = {0}, opad[64] = {0};
static unsigned char K[64] = {0};
static unsigned char firstPad[64 + 1024 + 1] = {0};
static unsigned char midResult[64 + 16] = {0};
static MD5_CTX context;

int KeyMD5_Encode(char* Encoded, char* Data, int iSize, char *Key, int iKeyLen)
{
	int i;

	memset(ipad, 0x36, 64);
	memset(opad, 0x5c, 64);
	memcpy(K, Key, iKeyLen);

	for(i=0; i<64; i++)	{
		ipad[i] ^= K[i];
		opad[i] ^= K[i];
	}
	
	memcpy(firstPad, ipad, 64);
	memcpy(firstPad + 64, Data, iSize);
	firstPad[64 + iSize] = 0;

	MD5_Init (&context);
	MD5_Update (&context, firstPad, 64 + iSize);
	MD5_Final (midResult + 64, &context);

	memcpy(midResult, opad, 64);

	MD5_Init (&context);
	MD5_Update (&context, midResult, 64 + 16);
	MD5_Final ((unsigned char *)Encoded, &context);

	return 16;
}

