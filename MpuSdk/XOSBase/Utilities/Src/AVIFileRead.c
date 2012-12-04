/* 
*  Copyright (c) 2005, 安徽创世科技有限公司
*  All rights reserved.
*  
*  文件名称: AviRileRead.c
*  文件标识: 
*  摘    要:	
*  
*  当前版本: 
*  作    者: jiakuant
*  完成日期: 
*  修正备注: 
*  
*/

#include "../../XOS/XOS.h"
#include "../AVIFileRead.h"

typedef struct __AVIIndex_
{
	//FOURCC fcc;				//必须为'idx1'
	//DWORD   cb;				//本数据结构的大小,不包括最初的8个字节(fcc和cb两个域)
	XOS_U32 u32ChunkID;			//表征本数据块的四字符码
	XOS_U32 u32Flags;			//说明本数据块是不是关键帧\是不是'rec'列表等信息
	XOS_U32 u32Offset;			//本数据块在文件中的偏移量
	XOS_U32 u32Size;			//本数据块的大小
}AVIIndex, *pAVIIndex;

#define FOURCC_ID_LEN		4	//头标识长度RIFF LIST AVI_

//定义文件操作指针
CAVIFile_Open g_pAVIFile_Open = NULL;
CAVIFile_Seek g_pAVIFile_Seek = NULL;
CAVIFile_Read g_pAVIFile_Read = NULL;
CAVIFile_Write g_pAVIFile_Write = NULL;
CAVIFile_Close g_pAVIFile_Close = NULL;
CAVIFile_Eof g_pAVIFile_Eof = NULL;

//////////////////////////////////////////////////////////////////////////
//指定文件操作函数
void AVIFile_RegisterOpen(CAVIFile_Open pAVIFileOpen)
{
	g_pAVIFile_Open = pAVIFileOpen;
}

void AVIFile_RegisterSeek(CAVIFile_Seek pAVIFileSeek)
{
	g_pAVIFile_Seek = pAVIFileSeek;
}

void AVIFile_RegisterRead(CAVIFile_Read pAVIFileRead)
{
	g_pAVIFile_Read = pAVIFileRead;
}

void AVIFile_RegisterWrite(CAVIFile_Write pAVIFileWrite)
{
	g_pAVIFile_Write = pAVIFileWrite;
}

void AVIFile_RegisterEof(CAVIFile_Eof pAVIFileEof)
{
	g_pAVIFile_Eof = pAVIFileEof;
}

void AVIFile_RegisterClose(CAVIFile_Close pAVIFileClose)
{
	g_pAVIFile_Close = pAVIFileClose;
}

//////////////////////////////////////////////////////////////////////////
//内部接口
static XOS_BOOL RIFFOpen(CHUNK *pFileChunk, XOS_CPSZ pszFileName)
{
	char szReadBuff[3*FOURCC_ID_LEN];
	memset(szReadBuff, 0, sizeof(szReadBuff));
	
	assert(pFileChunk != NULL);
	assert(g_pAVIFile_Open != NULL);
	assert(g_pAVIFile_Read != NULL);
	
	//打开文件
	pFileChunk->pFHandle = g_pAVIFile_Open(pszFileName, XOS_TRUE);
	if(pFileChunk->pFHandle == NULL)
	{
		printf("fopen %s failed.!\r\n", pszFileName);
		return XOS_FALSE;
	}

	if(g_pAVIFile_Read(pFileChunk->pFHandle, szReadBuff, sizeof(szReadBuff)) != sizeof(szReadBuff))
	{
		printf("RIFFOpen g_pAVIFile_Read failed!\r\n");
		return XOS_FALSE;
	}
	//检查avi文件头RIFF
	strncpy(pFileChunk->szType, szReadBuff, FOURCC_ID_LEN);
	if(strcmp(pFileChunk->szType, "RIFF") != 0)
	{
		printf("%s %d file format error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	//Size
	memcpy(&pFileChunk->u32Size, szReadBuff+FOURCC_ID_LEN, FOURCC_ID_LEN);
	//printf("FileSize:%dBytes\r\n", pFileChunk->u32Size);
	
	//数据位置
	pFileChunk->u32Position = 2*FOURCC_ID_LEN;

	//AVI 名称
	strncpy(pFileChunk->szName, szReadBuff+2*FOURCC_ID_LEN, FOURCC_ID_LEN);
	return XOS_TRUE;
}

static XOS_BOOL FindFirstChunk(RIFF_FindData_t *pRtFindData, const CHUNK *pFatherChunk, CHUNK *pChildChunk)
{
	int iOffSet;
	char fc[8];
	memset(fc, 0, sizeof(fc));

	assert(pFatherChunk != NULL);
	assert(pChildChunk != NULL);
	assert(g_pAVIFile_Read != NULL);

	pRtFindData->RootChunk = *pFatherChunk;
	if(strcmp(pFatherChunk->szType, "RIFF") == 0 ||
	   strcmp(pFatherChunk->szType, "LIST") == 0)
	{
		//只有RIFF和LIST才有子块,iOffSet为子块的起始位置
		iOffSet = pFatherChunk->u32Position + 4;
	}
	else
	{
		return XOS_FALSE;
	}

	//获得读文件的文件指针
	pRtFindData->LastFindChunk.pFHandle = pFatherChunk->pFHandle;
	assert(g_pAVIFile_Seek != NULL);
	g_pAVIFile_Seek(pFatherChunk->pFHandle, iOffSet, SEEK_SET);
	if(g_pAVIFile_Read(pFatherChunk->pFHandle, fc, 4) != 4)
	{
		printf("%s %d fread error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	
	if(strcmp(fc, "LIST") == 0)
	{
		strcpy(pRtFindData->LastFindChunk.szType, "LIST");
	}
	else
	{
		strcpy(pRtFindData->LastFindChunk.szType, "ATOM");
		strcpy(pRtFindData->LastFindChunk.szName, fc);
	}
	
	if(g_pAVIFile_Read(pFatherChunk->pFHandle, &pRtFindData->LastFindChunk.u32Size, 4) != 4)
	{
		printf("%s %d fread error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	
	pRtFindData->LastFindChunk.u32Position = iOffSet + 8;
	
	//发现子块是RIFF和LIST,取name
	if(strcmp(pRtFindData->LastFindChunk.szType, "RIFF") == 0 ||
	   strcmp(pRtFindData->LastFindChunk.szType, "LIST") == 0)
	{
		if(g_pAVIFile_Read(pFatherChunk->pFHandle, fc, 4) != 4)
		{
			printf("%s %d fread error!\r\n", __FILE__, __LINE__);
			return XOS_FALSE;
		}
		strcpy(pRtFindData->LastFindChunk.szName, fc);
	}

	memcpy(pChildChunk, &pRtFindData->LastFindChunk, sizeof(CHUNK));
	return XOS_TRUE;
}

static XOS_BOOL GetBrotherChunk(CHUNK *pChunk, CHUNK *pBrotherChunk)
{
	char fc[8];
	int opo;
	
	memset(fc, 0, sizeof(fc));
	assert(pChunk != NULL);
	assert(pBrotherChunk != NULL);
	assert(g_pAVIFile_Seek != NULL);
	assert(g_pAVIFile_Read != NULL);
	assert(g_pAVIFile_Eof != NULL);

	g_pAVIFile_Seek(pChunk->pFHandle, pChunk->u32Position + pChunk->u32Size, SEEK_SET);
	if(g_pAVIFile_Eof(pChunk->pFHandle) == XOS_TRUE)
	{
		printf("reach the end of the file!\r\n");
		return XOS_FALSE;
	}

	//1
	pBrotherChunk->pFHandle = pChunk->pFHandle;
	//2
	pBrotherChunk->u32Position = pChunk->u32Position + pChunk->u32Size + 8;
	//3
	opo = pChunk->u32Position + pChunk->u32Size;
	g_pAVIFile_Seek(pChunk->pFHandle, opo, SEEK_SET);
	if(g_pAVIFile_Read(pChunk->pFHandle, fc, 4) != 4)
	{
		printf("%s %d fread error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	
	if(strcmp(fc, "LIST") == 0)
	{
		strcpy(pBrotherChunk->szType, "LIST");
	}
	else
	{
		strcpy(pBrotherChunk->szType, "ATOM");
		strcpy(pBrotherChunk->szName, fc);
	}
	//
	if(g_pAVIFile_Read(pBrotherChunk->pFHandle, &pBrotherChunk->u32Size, 4) != 4)
	{
		printf("%s %d fread error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	//
	if(strcmp(pBrotherChunk->szType, "RIFF") == 0 || 
	   strcmp(pBrotherChunk->szType, "LIST") == 0)
	{
		if(g_pAVIFile_Read(pBrotherChunk->pFHandle, pBrotherChunk->szName, 4) != 4)
		{
			printf("%s %d fread error!\r\n", __FILE__, __LINE__);
			return XOS_FALSE;
		}
	}
	return XOS_TRUE;
}

static XOS_BOOL FindNextChunk(RIFF_FindData_t *handle, CHUNK *pChunk)
{
	int opo;									//operate position
	char fc[8];
	memset(fc, 0, sizeof(fc));

	assert(handle != NULL && pChunk != NULL);
	assert(g_pAVIFile_Seek != NULL);
	assert(g_pAVIFile_Read != NULL);
	
	if((handle->LastFindChunk.u32Position + handle->LastFindChunk.u32Size)
		>= (handle->RootChunk.u32Position + handle->RootChunk.u32Size))
	{
		//找到最后一个块
		printf("%s %d This is the last chunk!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}

	pChunk->pFHandle = handle->RootChunk.pFHandle;
	pChunk->u32Position = handle->LastFindChunk.u32Position 
						+ handle->LastFindChunk.u32Size + 8;
	opo = handle->LastFindChunk.u32Position + handle->LastFindChunk.u32Size;
	g_pAVIFile_Seek(pChunk->pFHandle, opo, SEEK_SET);
	if(g_pAVIFile_Read(pChunk->pFHandle, fc, 4) != 4)
	{
		printf("%s %d fread error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	if(strcmp(fc, "LIST") == 0)
	{
		strcpy(pChunk->szType, "LIST");
	}
	else
	{
		strcpy(pChunk->szType, "ATOM");
		strcpy(pChunk->szName, fc);
	}

	if(g_pAVIFile_Read(pChunk->pFHandle, &pChunk->u32Size, 4) != 4)
	{
		printf("%s %d fread error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}

	//只有RIFF和LIST头有Type类型,idx1没有Type，所以不用读了
	if(strcmp(pChunk->szType, "RIFF") == 0 ||
	   strcmp(pChunk->szType, "LIST") == 0)
	{
		if(g_pAVIFile_Read(pChunk->pFHandle, fc, 4) != 4)
		{
			printf("%s %d fread error!\r\n", __FILE__, __LINE__);
			return XOS_FALSE;
		}
		strcpy(pChunk->szName, fc);
	}
	
	memcpy(&handle->LastFindChunk, pChunk, sizeof(CHUNK));
	return XOS_TRUE;
}

static int GetData(CHUNK *pChunk, char *buf, int len)
{
	assert(pChunk != NULL);
	assert(g_pAVIFile_Seek != NULL);
	assert(g_pAVIFile_Read != NULL);
	assert(strcmp(pChunk->szType, "ATOM") == 0);
	if((int)pChunk->u32Size > len)
	{
		printf("GetData buf too small!\r\n");
		return -1;
	}

	g_pAVIFile_Seek(pChunk->pFHandle, pChunk->u32Position, SEEK_SET);
	return g_pAVIFile_Read(pChunk->pFHandle, buf, pChunk->u32Size);
}

static int GetData2(CHUNK *pChunk, int iStart, char *buf, int len)
{
	assert(pChunk != NULL);
	assert(strcmp(pChunk->szType, "ATOM") == 0);
	assert(g_pAVIFile_Seek != NULL);
	assert(g_pAVIFile_Read != NULL);
	
	g_pAVIFile_Seek(pChunk->pFHandle, pChunk->u32Position + iStart, SEEK_SET);
	return g_pAVIFile_Read(pChunk->pFHandle, buf, len);
}

static void CloseFile(CHUNK *pChunk)
{
	assert(pChunk != NULL);
	assert(g_pAVIFile_Close != NULL);
	if (pChunk->pFHandle != NULL)
	{
		g_pAVIFile_Close(pChunk->pFHandle);
	}
	memset(pChunk, 0, sizeof(CHUNK));
}

static XOS_BOOL FindChildChunk(CHUNK *pChunk, CHUNK *pChildChunk, char *szIDName, int count)
{
	XOS_BOOL bRv;
	int i = -1;
	RIFF_FindData_t stFindData;
	
	assert(pChunk != NULL && pChildChunk != NULL);
	if(count < 0)
	{
		return XOS_FALSE;
	}

	//寻找第一个CHUNK
	//chunk 输入时表示在这个CHUNK中寻找子CHUNK	输出时表示寻找到的CHUNK
	//pChildChunk为输出的子块
	bRv = FindFirstChunk(&stFindData, pChunk, pChildChunk);
	if(bRv != XOS_FALSE)
	{
		if(strcmp(szIDName, pChildChunk->szName) == 0)
		{
			i++;
		}
		while(i < count)
		{
			//寻找下一个Chunk   pChildChunk用作填充findData->LastFindChunk数据
			if(FindNextChunk(&stFindData, pChildChunk) == XOS_FALSE)
			{
				return XOS_FALSE;
			}

			if(szIDName == NULL)
			{
				i++;			
			}
			else if(strcmp(szIDName, pChildChunk->szName) == 0)
			{
				i++;
			}
		}
		return XOS_TRUE;
	}
	else
	{
		return XOS_FALSE;
	}
}

XOS_BOOL FindChildChunk2(CHUNK *pChunk, CHUNK *pChildChunk, int ioffset, char *szname)
{
	XOS_BOOL bRv;
	char fc[8];
	memset(fc, 0, sizeof(fc));
	assert(g_pAVIFile_Seek != NULL);
	assert(g_pAVIFile_Read != NULL);

	pChildChunk->pFHandle = pChunk->pFHandle;
	bRv = g_pAVIFile_Seek(pChunk->pFHandle, pChunk->u32Position + ioffset, SEEK_SET);
	if (bRv == XOS_FALSE)
	{
		return XOS_FALSE;
	}

	if (g_pAVIFile_Read(pChunk->pFHandle, fc, 4) != 4)
	{
		return XOS_FALSE;
	}
	
	if (strcmp(fc, "LIST") == 0) 
	{
		strcpy(pChildChunk->szType, "LIST");
	} 
	else if (strcmp(fc, "RIFF") == 0) 
	{
		strcpy(pChildChunk->szType, "RIFF");
	} 
	else 
	{
		strcpy(pChildChunk->szType, "ATOM");
		strcpy(pChildChunk->szName, fc);
		if (szname != NULL && strcmp(szname, pChildChunk->szName) != 0) 
		{
			return XOS_FALSE;	
		}
	}

	pChildChunk->u32Position = pChunk->u32Position + ioffset + 8;
	bRv = g_pAVIFile_Seek(pChunk->pFHandle, pChunk->u32Position + ioffset + 4, SEEK_SET);
	if (bRv != XOS_TRUE)
	{
		return XOS_FALSE;
	}
	if (g_pAVIFile_Read(pChunk->pFHandle, &pChildChunk->u32Size, 4) != 4)
	{
		return XOS_FALSE;
	}
	if(pChildChunk->u32Position + pChildChunk->u32Size > pChunk->u32Position + pChunk->u32Size) 
	{
		return XOS_FALSE;
	}
	return XOS_TRUE;
}

//////////////////////////////////////////////////////////////////////////
//接口函数

XOS_BOOL AVIFileRead_Open(pAVIMacroFormat pstAVIMacroFormat, XOS_CPSZ pszFileName)
{
	XOS_BOOL bRv;

	//在我们的文件设计中,假设信息块后面跟的就是数据块,且假设都存在索引块
	assert(pstAVIMacroFormat != NULL);
	assert(pszFileName != NULL);
	
	memset(pstAVIMacroFormat, 0, sizeof(AVIMacroFormat));

	//在打开操作中，我们检查该文件是否符合AVI文件格式
	bRv = RIFFOpen(&pstAVIMacroFormat->stFileChunk, pszFileName);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile RIFFOpen failed.\r\n");
		return XOS_FALSE;
	}
	printf("RIFF: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stFileChunk.szType, pstAVIMacroFormat->stFileChunk.u32Size, 
			pstAVIMacroFormat->stFileChunk.u32Position, pstAVIMacroFormat->stFileChunk.szName);

	//获得pInfoChunk信息块,用于描述AVI文件中各个流的格式信息
	bRv = FindChildChunk(&pstAVIMacroFormat->stFileChunk, &pstAVIMacroFormat->stHdrlChunk, "hdrl", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile FindChildChunk hdrl failed.\r\n");
		return XOS_FALSE;
	}
	printf("hdrl: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stHdrlChunk.szType, pstAVIMacroFormat->stHdrlChunk.u32Size, 
			pstAVIMacroFormat->stHdrlChunk.u32Position, pstAVIMacroFormat->stHdrlChunk.szName);

	//获得stMoviChunk数据块信息
	bRv = FindChildChunk(&pstAVIMacroFormat->stFileChunk, &pstAVIMacroFormat->stMoviChunk, "movi", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile FindChildChunk movi failed.\r\n");
		return XOS_FALSE;
	}
	printf("movi: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stMoviChunk.szType, pstAVIMacroFormat->stMoviChunk.u32Size, 
			pstAVIMacroFormat->stMoviChunk.u32Position, pstAVIMacroFormat->stMoviChunk.szName);

	//获得stIndexChunk索引块基本信息
	bRv = FindChildChunk(&pstAVIMacroFormat->stFileChunk, &pstAVIMacroFormat->stIndexChunk, "idx1", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile FindChildChunk idx1 failed.\r\n");
		return XOS_FALSE;
	}
	printf("idx1: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stIndexChunk.szType, pstAVIMacroFormat->stIndexChunk.u32Size, 
			pstAVIMacroFormat->stIndexChunk.u32Position, pstAVIMacroFormat->stIndexChunk.szName);

	return XOS_TRUE;
}

void AVIFileRead_Release(pAVIMacroFormat pstAVIMacroFormat, pMyAVIStream pstAVIStream)
{
	//释放avi存储文件结构的内存
	if (pstAVIMacroFormat != NULL)
	{
		if (&pstAVIMacroFormat->stFileChunk != NULL)
		{
			//释放文件句柄
			CloseFile(&pstAVIMacroFormat->stFileChunk);
		}
		memset(pstAVIMacroFormat, 0, sizeof(pAVIMacroFormat));
	}

	if (pstAVIStream->pIndexDataBuffer != NULL)
	{
		free(pstAVIStream->pIndexDataBuffer);
		pstAVIStream->pIndexDataBuffer = NULL;
	}
}

XOS_BOOL AVIFileRead_MainInfo(pAVIMacroFormat pstAVIMacroFormat, pAVIMainHeader pstAVIMainHeader)
{
	int rv;
	XOS_BOOL bRv;
	CHUNK hdrlChunk, avihChunk;
	assert(pstAVIMacroFormat != NULL && pstAVIMainHeader != NULL);

	//在stFileChunk里找到子块hdrlChunk,并得到hdrlChunk信息
	bRv = FindChildChunk(&pstAVIMacroFormat->stFileChunk, &hdrlChunk, "hdrl", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_MainInfo FindChildChunk hdrl failed.");
		return XOS_FALSE;
	}
	
	if(memcmp(hdrlChunk.szType, "LIST", FOURCC_ID_LEN) != 0)
	{
		printf("AVIFileRead_MainInfo hdrl chunk type error!\r\n");
		return XOS_FALSE;
	}

	//在hdrlChunk里面找到子块avihChunk,并得到avihChunk信息
	bRv = FindChildChunk(&hdrlChunk, &avihChunk, "avih", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_MainInfo FindChildChunk avih failed.");
		return XOS_FALSE;
	}
	
	if(memcmp(avihChunk.szType, "ATOM", FOURCC_ID_LEN) != 0)
	{
		printf("AVIFileRead_MainInfo avih chunk type error!\r\n");
		return XOS_FALSE;
	}

	//获取数据buff放在stMainAVIHeader
	rv = GetData(&avihChunk, (char *)pstAVIMainHeader, sizeof(AVIMainHeader));
	if(rv != sizeof(AVIMainHeader))
	{
		printf("AVIFileRead_MainInfo avih GetData failed!\r\n");
		return XOS_FALSE;
	}

	printf("AVI MainHeader Info:\r\n\
u32MicroSecPerFrame:%d \r\n\
u32MaxBytesPerSec:%d\r\n\
u32PaddingGranularity:%d\r\n\
u32Flages:%d\r\n\
u32TotalFrame:%d\r\n\
u32InitialFrames:%d\r\n\
u32Streams:%d\r\n\
u32SuggestedBufferSize:%d\r\n\
u32Width:%d\r\n\
u32Height:%d\r\n\
",pstAVIMainHeader->u32MicroSecPerFrame, pstAVIMainHeader->u32MaxBytesPerSec, 
pstAVIMainHeader->u32PaddingGranularity, pstAVIMainHeader->u32Flages, 
pstAVIMainHeader->u32TotalFrame, pstAVIMainHeader->u32InitialFrames, 
pstAVIMainHeader->u32Streams, pstAVIMainHeader->u32SuggestedBufferSize, 
pstAVIMainHeader->u32Width, pstAVIMainHeader->u32Height);

	return XOS_TRUE;
}

XOS_BOOL AVIFileRead_StreamInfo(pAVIMacroFormat pstAVIMacroFormat, pMyAVIStream pstAVIStream, int *piTotalIndex)
{
	int rv;
	XOS_BOOL bRv;	
	int iIndexOffset;
	AVIIndex stIndex;
	CHUNK stStreamHeaderChunk, stMediaChunk;

	memset(pstAVIStream, 0, sizeof(MyAVIStream));
	assert(pstAVIMacroFormat != NULL &&	pstAVIStream != NULL);

	//strl{strh,strf,strd}
	bRv = FindChildChunk(&pstAVIMacroFormat->stHdrlChunk, &pstAVIStream->stStrlChunk, "strl", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_GetStream FindChildChunk strl failed.\r\n");
		return XOS_FALSE;
	}

	pstAVIStream->stMoviChunk = pstAVIMacroFormat->stMoviChunk;
	pstAVIStream->stIndexChunk = pstAVIMacroFormat->stIndexChunk;
	//流的头信息数据
	bRv = FindChildChunk(&pstAVIStream->stStrlChunk, &stStreamHeaderChunk, "strh", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_GetStream FindChildChunk strh failed.\r\n");
		return XOS_FALSE;
	}

	rv = GetData(&stStreamHeaderChunk, (char *)&pstAVIStream->stAVIStreamHeader, sizeof(AVIStreamHeader));
	if (rv != sizeof(AVIStreamHeader))
	{
		printf("AVIFileRead_GetStream GetData failed.\r\n");
		return XOS_FALSE;
	}
	printf("AVI Stream Info:\r\n\
fccType:%c%c%c%c \r\n\
fccHandler:%c%c%c%c\r\n\
u32Flags:%d\r\n\
u16Priority:%d\r\n\
u16Language:%d\r\n\
u32InitalFrames:%d\r\n\
u32Scale:%d\r\n\
u32Rate:%dfps\r\n\
u32Start:%d\r\n\
u32Length:%d\r\n\
u32SuggestedBufferSize:%dBytes\r\n\
u32Quality:%d\r\n\
u32SampleSize:%d\r\n\
left:%d right:%d top:%d bottom:%d\r\n",
	((char *)&pstAVIStream->stAVIStreamHeader.fccType)[0], ((char *)&pstAVIStream->stAVIStreamHeader.fccType)[1],
	((char *)&pstAVIStream->stAVIStreamHeader.fccType)[2], ((char *)&pstAVIStream->stAVIStreamHeader.fccType)[3],
	((char *)&pstAVIStream->stAVIStreamHeader.fccHandler)[0], ((char *)&pstAVIStream->stAVIStreamHeader.fccHandler)[1],
	((char *)&pstAVIStream->stAVIStreamHeader.fccHandler)[2], ((char *)&pstAVIStream->stAVIStreamHeader.fccHandler)[3],
	pstAVIStream->stAVIStreamHeader.u32Flags, pstAVIStream->stAVIStreamHeader.u16Priority, 
	pstAVIStream->stAVIStreamHeader.u16Language, pstAVIStream->stAVIStreamHeader.u32InitalFrames, 
	pstAVIStream->stAVIStreamHeader.u32Scale, pstAVIStream->stAVIStreamHeader.u32Rate, 
	pstAVIStream->stAVIStreamHeader.u32Start, pstAVIStream->stAVIStreamHeader.u32Length,
	pstAVIStream->stAVIStreamHeader.u32SuggestedBufferSize, pstAVIStream->stAVIStreamHeader.u32Quality, 
	pstAVIStream->stAVIStreamHeader.u32SampleSize, pstAVIStream->stAVIStreamHeader.Rect.left, 
	pstAVIStream->stAVIStreamHeader.Rect.right, pstAVIStream->stAVIStreamHeader.Rect.top, 
	pstAVIStream->stAVIStreamHeader.Rect.bottom);

	//获取一个流时,必须将一个流的第一个帧找到
	if (pstAVIStream->stAVIStreamHeader.fccType == streamtypeVIDEO) 
	{
		if (pstAVIStream->stAVIStreamHeader.fccHandler != encodertypeNULL) 
		{
			//压缩的图像数据流
			bRv = FindChildChunk(&pstAVIMacroFormat->stMoviChunk, &stMediaChunk, "00dc", 0);
		} 
		else 
		{
			//RGB数据流
			bRv = FindChildChunk(&pstAVIMacroFormat->stMoviChunk, &stMediaChunk, "00db", 0);
		}
	}
	else if (pstAVIStream->stAVIStreamHeader.fccType == streamtypeAUDIO) 
	{
		bRv = FindChildChunk(&pstAVIMacroFormat->stMoviChunk, &stMediaChunk, "01wb", 0);//音频数据流
	}
	
	//"idxl"块如果不为空
	if (&pstAVIMacroFormat->stIndexChunk != NULL) 
	{
		iIndexOffset = 0;
		//块大小大于0
		while (iIndexOffset < (int)pstAVIMacroFormat->stIndexChunk.u32Size) 
		{
			rv = GetData2(&pstAVIMacroFormat->stIndexChunk, iIndexOffset, (char *)&stIndex, sizeof(AVIIndex));
			if (rv != sizeof(AVIIndex))
			{
				printf("AVIFileRead_StreamInfo GetData2 failed.\r\n");
				return XOS_FALSE;
			}
			printf("stIndex.u32ChunkID:%c%c%c%c\r\n", ((char *)&stIndex.u32ChunkID)[0], ((char *)&stIndex.u32ChunkID)[1],
													((char *)&stIndex.u32ChunkID)[2], ((char *)&stIndex.u32ChunkID)[3]);
			printf("fccType:%c%c%c%c\r\n", ((char *)&pstAVIStream->stAVIStreamHeader.fccType)[0], 
											((char *)&pstAVIStream->stAVIStreamHeader.fccType)[1],
											((char *)&pstAVIStream->stAVIStreamHeader.fccType)[2], 
											((char *)&pstAVIStream->stAVIStreamHeader.fccType)[3]);
			if (((pstAVIStream->stAVIStreamHeader.fccType == streamtypeAUDIO || 
				  pstAVIStream->stAVIStreamHeader.fccType == streamtypeVIDEO) && 
				 ((stIndex.u32ChunkID == indextype00db && pstAVIStream->stAVIStreamHeader.fccHandler == encodertypeNULL) ||
				  (stIndex.u32ChunkID == indextype00dc && pstAVIStream->stAVIStreamHeader.fccHandler != encodertypeNULL) ||
				  (stIndex.u32ChunkID == indextype02dc))))
			{
				break;
			}

			if ((pstAVIStream->stAVIStreamHeader.fccType == streamtypeAUDIO || 
				 pstAVIStream->stAVIStreamHeader.fccType == streamtypeVIDEO) && 
				(stIndex.u32ChunkID == indextype01wb) || (stIndex.u32ChunkID == indextype03wb))
			{
				break;
			}
			
			iIndexOffset += sizeof(AVIIndex);
		}	
		
		printf("AVIFileRead_StreamInfo Index malloc u32Size:%d\r\n", pstAVIMacroFormat->stIndexChunk.u32Size);
		pstAVIStream->pIndexDataBuffer = malloc(pstAVIMacroFormat->stIndexChunk.u32Size);
		if (pstAVIStream->pIndexDataBuffer == NULL)
		{
			printf("index data malloc failed.\r\n");
			return XOS_FALSE;
		}
		else
		{
			//第一次读索引,把索引全部读出来放到内存中
			TCS_PUSH("GetData2");
			bRv = GetData2(&pstAVIStream->stIndexChunk, iIndexOffset, 
							pstAVIStream->pIndexDataBuffer, pstAVIStream->stIndexChunk.u32Size);
			TCS_POP;
			if (bRv == XOS_FALSE) 
			{
				printf("Index GetData2 failed!\r\n");
				return XOS_FALSE;
			}
			*piTotalIndex = pstAVIStream->stIndexChunk.u32Size / sizeof(AVIIndex);
		}
	}
	
	return XOS_TRUE;
}

static XOS_BOOL GetIndexChunk(pMyAVIStream pstAVIStream, pAVIIndex pstIndex, int iIndexNum)
{
	assert(pstAVIStream != NULL && iIndexNum >= 0);
	assert(&pstAVIStream->stIndexChunk != NULL);
	assert(pstIndex != NULL);

	memset(pstIndex, 0, sizeof(AVIIndex));
	memcpy((char*)pstIndex, pstAVIStream->pIndexDataBuffer + iIndexNum*sizeof(AVIIndex), sizeof(AVIIndex));
	/*
	printf("u32ChunkID:%c%c%c%c IFrame:%d u32Offset:%d u32Size:%d\r\n", 
			((char *)&pstIndex->u32ChunkID)[0], ((char *)&pstIndex->u32ChunkID)[1],
			((char *)&pstIndex->u32ChunkID)[2], ((char *)&pstIndex->u32ChunkID)[3], pstIndex->u32Flags, 
			pstIndex->u32Offset, pstIndex->u32Size);
	*/
	if (((pstIndex->u32ChunkID == indextype00db && pstAVIStream->stAVIStreamHeader.fccHandler == encodertypeNULL) || 
		  (pstIndex->u32ChunkID == indextype00dc && pstAVIStream->stAVIStreamHeader.fccHandler != encodertypeNULL) ||
		  (pstIndex->u32ChunkID == indextype02dc)) &&  
(pstAVIStream->stAVIStreamHeader.fccType == streamtypeAUDIO || pstAVIStream->stAVIStreamHeader.fccType == streamtypeVIDEO))
	{
		//找到符合的媒体数据
		return XOS_TRUE;
	}

	if ((pstAVIStream->stAVIStreamHeader.fccType == streamtypeAUDIO || 
		 pstAVIStream->stAVIStreamHeader.fccType == streamtypeVIDEO) &&
		 (pstIndex->u32ChunkID == indextype01wb || pstIndex->u32ChunkID == indextype03wb))
	{
		return XOS_TRUE;
	}
	return XOS_FALSE;
}

int AVIFileRead_Frame(pMyAVIStream pstAVIStream, unsigned long *pulIndexNum, char *pFrameBuffer, 
					  unsigned long *pulLen, XOS_BOOL *pbIFrame, int *piFindFrameType, XOS_BOOL bFindIFrame)
{
	int rv;
	XOS_BOOL bRv;
	AVIIndex stIndex;
	CHUNK stMoviDataChunk;
	unsigned long ulTotalIndexNum;	
	assert(pFrameBuffer != NULL);
	assert(pulLen != NULL && pbIFrame != NULL);
	assert(*pulIndexNum >= 0);
	
	ulTotalIndexNum = pstAVIStream->stIndexChunk.u32Size/sizeof(AVIIndex);
	if (*pulIndexNum >= ulTotalIndexNum)
	{
		printf("AVIFileRead_Frame reach the end of this file . %d >= %d.\r\n", *pulIndexNum, ulTotalIndexNum);
		return 1;
	}
	while (1)
	{
		bRv = GetIndexChunk(pstAVIStream, &stIndex, *pulIndexNum);
		if (bRv == XOS_FALSE)
		{
			printf("GetIndexChunk failed.\r\n");
			return -1;
		}

		if (*piFindFrameType == AVIFILEREAD_FINDVIDEO)
		{
			//仅仅找视频
			if (bFindIFrame)
			{
				//需要找I帧
				if (stIndex.u32Flags != 0 && 
					(stIndex.u32ChunkID != indextype01wb) || (stIndex.u32ChunkID != indextype03wb))
				{
					//找到视频I帧
					*piFindFrameType = AVIFILEREAD_FINDVIDEO;
					break;
				}
			}
			else
			{
				if (stIndex.u32ChunkID != indextype01wb && stIndex.u32ChunkID != indextype03wb)
				{
					//找到视频帧
					*piFindFrameType = AVIFILEREAD_FINDVIDEO;
					break;
				}
			}
			//继续找
			(*pulIndexNum)++;
		}
		else if(*piFindFrameType == AVIFILEREAD_FINDAUDIO)
		{
			//仅仅找音频
			if (stIndex.u32ChunkID == indextype01wb || stIndex.u32ChunkID == indextype03wb)
			{
				//找到音频帧
				break;
			}
			//继续找
			(*pulIndexNum)++;
		}
		else if (*piFindFrameType == AVIFILEREAD_FINDALL)
		{
			//全部找
			if (stIndex.u32ChunkID != indextype01wb && stIndex.u32ChunkID != indextype03wb)
			{	
				*piFindFrameType = AVIFILEREAD_FINDVIDEO;
			}
			else
			{
				*piFindFrameType = AVIFILEREAD_FINDAUDIO;
			}
			break;
		}
		else
		{
			printf("AVIFileRead_Frame Type error.\r\n");
			assert(0);
		}
	}

	*pbIFrame = (stIndex.u32Flags != 0 && 
				(stIndex.u32ChunkID != indextype01wb) || (stIndex.u32ChunkID != indextype03wb));
	
	//读取数据帧
	bRv = FindChildChunk2(&pstAVIStream->stMoviChunk, &stMoviDataChunk, stIndex.u32Offset, NULL);
	if (bRv == XOS_FALSE) 
	{
		printf("AVIFileRead_Frame FindChildChunk2 failed.\r\n");
		return -1;
	}
	if (stMoviDataChunk.u32Size != stIndex.u32Size) //index索引中记录的块大小和实际的大小比较
	{
		printf("stMoviDataChunk.u32Size != stIndex.u32Offset.\r\n");
		return -1;
	} 

	if (*pulLen < (int)stMoviDataChunk.u32Size) 
	{
		printf("pFrameBuffer is overflow %d < %d.\r\n", *pulLen, stMoviDataChunk.u32Size);
		//要读取的数据帧长度*piLen小于数据块长度
		return -1;
	}

	rv = GetData(&stMoviDataChunk, pFrameBuffer, *pulLen);
	if (rv != (int)stMoviDataChunk.u32Size)
	{
		printf("AviFileRead_Frame GetData failed read:r%d.\r\n", rv);
		return -1;
	}

	//获得数据块长度
	*pulLen = stMoviDataChunk.u32Size;
	return 0;
}


