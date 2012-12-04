/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: KeyAndMAC2Hash.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-12-22 16:38:25
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../KeyAndMAC2Hash.h"
#include "../md5.h"

void GetPrivateKey(unsigned char *pKey)
{
	int i;
	unsigned char ucaKey[16] = {0x76,0x9A,0x45,0x87, 0x36,0xAB,0x43,0x39, 
								0x42,0x73,0x39,0x2C, 0xB4,0xD7,0xE1,0x3F};
	for (i=0; i<4; i++)
	{
		pKey[i] = ucaKey[i] & 0x05;
	}
	for (i=4; i<8; i++)
	{
		pKey[i] = ucaKey[i] ^ 0x5E;
	}
	for (i=8; i<12; i++)
	{
		pKey[i] = ucaKey[i] | 0x5E;
	}
	for (i=12; i<16; i++)
	{
		pKey[i] = ucaKey[i] ^ 0xA5;
	}
}

void GetPrivateKey_reg(unsigned char *pKey)
{
	int i;
	unsigned char ucaKey[16] = {0x25,0x93,0xAB,0xD7, 0xD9,0x3A,0x9C,0x0E, 
								0xF6,0x3D,0xE4,0x1E, 0x6C,0xB6,0xE7,0x0C};
	for (i=0; i<4; i++)
	{
		pKey[i] = ucaKey[i] | 0x4D;
	}
	for (i=4; i<8; i++)
	{
		pKey[i] = ucaKey[i] & 0x0F;
	}
	for (i=8; i<12; i++)
	{
		pKey[i] = ucaKey[i] ^ 0x8A;
	}
	for (i=12; i<16; i++)
	{
		pKey[i] = ucaKey[i] | 0x75;
	}
}

static void GetPrivateKey0(unsigned char *pKey)
{
	int i;
	unsigned char ucaKey[16] = {0x65,0x93,0x40,0x83, 0x37,0xA4,0x4F,0x39, 
								0x44,0x73,0x3B,0x2C, 0xB4,0xD7,0xE1,0x33};
	for (i=0; i<4; i++)
	{
		pKey[i] = ucaKey[i] & 0x04;
	}
	for (i=4; i<8; i++)
	{
		pKey[i] = ucaKey[i] | 0x58;
	}
	for (i=8; i<12; i++)
	{
		pKey[i] = ucaKey[i] ^ 0x5E;
	}
	for (i=12; i<16; i++)
	{
		pKey[i] = ucaKey[i] & 0xA5;
	}
}

static void GetPrivateKey1(unsigned char *pKey)
{
	int i;
	unsigned char ucaKey[16] = {0x76,0x9A,0x45,0x87, 0x36,0xAB,0x43,0x39, 
								0x42,0x73,0x39,0x2C, 0xB4,0xD7,0xE1,0x3F};
	for (i=0; i<4; i++)
	{
		pKey[i] = ucaKey[i] & 0x05;
	}
	for (i=4; i<8; i++)
	{
		pKey[i] = ucaKey[i] ^ 0x5E;
	}
	for (i=8; i<12; i++)
	{
		pKey[i] = ucaKey[i] | 0x3E;
	}
	for (i=12; i<16; i++)
	{
		pKey[i] = ucaKey[i] ^ 0xAC;
	}
}

static void GetPrivateKey2(unsigned char *pKey)
{
	int i;
	unsigned char ucaKey[16] = {0x7A,0x9A,0x45,0x8B, 0x36,0xAB,0x43,0x39, 
								0x42,0x78,0x34,0x29, 0xBC,0xD7,0xE1,0x3A};
	for (i=0; i<4; i++)
	{
		pKey[i] = ucaKey[i] | 0x08;
	}
	for (i=4; i<8; i++)
	{
		pKey[i] = ucaKey[i] ^ 0x53;
	}
	for (i=8; i<12; i++)
	{
		pKey[i] = ucaKey[i] & 0x9E;
	}
	for (i=12; i<16; i++)
	{
		pKey[i] = ucaKey[i] ^ 0xA5;
	}
}

static void GetPrivateKey3(unsigned char *pKey)
{
	int i;
	unsigned char ucaKey[16] = {0x34,0xAE,0x12,0x42, 0x91,0x32,0x07,0x75, 
								0xAB,0x1E,0x8A,0x7E, 0xBC,0x77,0x25,0x35};
	for (i=0; i<4; i++)
	{
		pKey[i] = ucaKey[i] | 0x91;
	}
	for (i=4; i<8; i++)
	{
		pKey[i] = ucaKey[i] ^ 0xB4;
	}
	for (i=8; i<12; i++)
	{
		pKey[i] = ucaKey[i] & 0x29;
	}
	for (i=12; i<16; i++)
	{
		pKey[i] = ucaKey[i] ^ 0x64;
	}
}

void KeyAndMAC2Hash0(char *pKey, char *pMac, char binHash[16])
{
	char binBuffer[16+6+16] = "";
	MD5_CTX md5_ctx;

	memcpy(binBuffer, pKey, 16);
	memcpy(binBuffer+16, pMac, 6);	
	GetPrivateKey0((unsigned char *)binBuffer+16+6);

	MD5_Init(&md5_ctx);
	MD5_Update(&md5_ctx, (unsigned char *)binBuffer, 16+6+16);
	MD5_Final((unsigned char *)binHash, &md5_ctx);
	return ;
}

void KeyAndMAC2Hash1(char *pKey, char *pMac, char binHash[16])
{
	char binBuffer[16+6+16] = "";
	MD5_CTX md5_ctx;

	memcpy(binBuffer, pKey, 16);
	memcpy(binBuffer+16, pMac, 6);
	GetPrivateKey1((unsigned char *)binBuffer+16+6);

	MD5_Init(&md5_ctx);
	MD5_Update(&md5_ctx, (unsigned char *)binBuffer, 16+6+16);
	MD5_Final((unsigned char *)binHash, &md5_ctx);
	return ;
}

void KeyAndMAC2Hash2(char *pKey, char *pMac, char binHash[16])
{
	char binBuffer[16+6+16] = "";
	MD5_CTX md5_ctx;

	memcpy(binBuffer, pKey, 16);
	memcpy(binBuffer+16, pMac, 6);
	GetPrivateKey2((unsigned char *)binBuffer+16+6);

	MD5_Init(&md5_ctx);
	MD5_Update(&md5_ctx, (unsigned char *)binBuffer, 16+6+16);
	MD5_Final((unsigned char *)binHash, &md5_ctx);
	return ;
}

void SoftModel2Hash(char *pszSoftModel, char binHash[16])
{
	char binBuffer[16+6+16] = "";
	MD5_CTX md5_ctx;

	memset(binBuffer, 0, sizeof(binBuffer));
	GetPrivateKey3((unsigned char *)binBuffer);
	strncpy(binBuffer, pszSoftModel, sizeof(binBuffer)-16);

	MD5_Init(&md5_ctx);
	MD5_Update(&md5_ctx, (unsigned char *)binBuffer, 16+6+16);
	MD5_Final((unsigned char *)binHash, &md5_ctx);
	return ;
}
