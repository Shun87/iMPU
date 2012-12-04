/* 
*  Copyright (c) 2005, ���մ����Ƽ����޹�˾
*  All rights reserved.
*  
*  �ļ�����: AviRileRead.c
*  �ļ���ʶ: 
*  ժ    Ҫ:	
*  
*  ��ǰ�汾: 
*  ��    ��: jiakuant
*  �������: 
*  ������ע: 
*  
*/

#include "../../XOS/XOS.h"
#include "../AVIFileRead.h"

typedef struct __AVIIndex_
{
	//FOURCC fcc;				//����Ϊ'idx1'
	//DWORD   cb;				//�����ݽṹ�Ĵ�С,�����������8���ֽ�(fcc��cb������)
	XOS_U32 u32ChunkID;			//���������ݿ�����ַ���
	XOS_U32 u32Flags;			//˵�������ݿ��ǲ��ǹؼ�֡\�ǲ���'rec'�б����Ϣ
	XOS_U32 u32Offset;			//�����ݿ����ļ��е�ƫ����
	XOS_U32 u32Size;			//�����ݿ�Ĵ�С
}AVIIndex, *pAVIIndex;

#define FOURCC_ID_LEN		4	//ͷ��ʶ����RIFF LIST AVI_

//�����ļ�����ָ��
CAVIFile_Open g_pAVIFile_Open = NULL;
CAVIFile_Seek g_pAVIFile_Seek = NULL;
CAVIFile_Read g_pAVIFile_Read = NULL;
CAVIFile_Write g_pAVIFile_Write = NULL;
CAVIFile_Close g_pAVIFile_Close = NULL;
CAVIFile_Eof g_pAVIFile_Eof = NULL;

//////////////////////////////////////////////////////////////////////////
//ָ���ļ���������
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
//�ڲ��ӿ�
static XOS_BOOL RIFFOpen(CHUNK *pFileChunk, XOS_CPSZ pszFileName)
{
	char szReadBuff[3*FOURCC_ID_LEN];
	memset(szReadBuff, 0, sizeof(szReadBuff));
	
	assert(pFileChunk != NULL);
	assert(g_pAVIFile_Open != NULL);
	assert(g_pAVIFile_Read != NULL);
	
	//���ļ�
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
	//���avi�ļ�ͷRIFF
	strncpy(pFileChunk->szType, szReadBuff, FOURCC_ID_LEN);
	if(strcmp(pFileChunk->szType, "RIFF") != 0)
	{
		printf("%s %d file format error!\r\n", __FILE__, __LINE__);
		return XOS_FALSE;
	}
	//Size
	memcpy(&pFileChunk->u32Size, szReadBuff+FOURCC_ID_LEN, FOURCC_ID_LEN);
	//printf("FileSize:%dBytes\r\n", pFileChunk->u32Size);
	
	//����λ��
	pFileChunk->u32Position = 2*FOURCC_ID_LEN;

	//AVI ����
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
		//ֻ��RIFF��LIST�����ӿ�,iOffSetΪ�ӿ����ʼλ��
		iOffSet = pFatherChunk->u32Position + 4;
	}
	else
	{
		return XOS_FALSE;
	}

	//��ö��ļ����ļ�ָ��
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
	
	//�����ӿ���RIFF��LIST,ȡname
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
		//�ҵ����һ����
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

	//ֻ��RIFF��LISTͷ��Type����,idx1û��Type�����Բ��ö���
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

	//Ѱ�ҵ�һ��CHUNK
	//chunk ����ʱ��ʾ�����CHUNK��Ѱ����CHUNK	���ʱ��ʾѰ�ҵ���CHUNK
	//pChildChunkΪ������ӿ�
	bRv = FindFirstChunk(&stFindData, pChunk, pChildChunk);
	if(bRv != XOS_FALSE)
	{
		if(strcmp(szIDName, pChildChunk->szName) == 0)
		{
			i++;
		}
		while(i < count)
		{
			//Ѱ����һ��Chunk   pChildChunk�������findData->LastFindChunk����
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
//�ӿں���

XOS_BOOL AVIFileRead_Open(pAVIMacroFormat pstAVIMacroFormat, XOS_CPSZ pszFileName)
{
	XOS_BOOL bRv;

	//�����ǵ��ļ������,������Ϣ�������ľ������ݿ�,�Ҽ��趼����������
	assert(pstAVIMacroFormat != NULL);
	assert(pszFileName != NULL);
	
	memset(pstAVIMacroFormat, 0, sizeof(AVIMacroFormat));

	//�ڴ򿪲����У����Ǽ����ļ��Ƿ����AVI�ļ���ʽ
	bRv = RIFFOpen(&pstAVIMacroFormat->stFileChunk, pszFileName);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile RIFFOpen failed.\r\n");
		return XOS_FALSE;
	}
	printf("RIFF: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stFileChunk.szType, pstAVIMacroFormat->stFileChunk.u32Size, 
			pstAVIMacroFormat->stFileChunk.u32Position, pstAVIMacroFormat->stFileChunk.szName);

	//���pInfoChunk��Ϣ��,��������AVI�ļ��и������ĸ�ʽ��Ϣ
	bRv = FindChildChunk(&pstAVIMacroFormat->stFileChunk, &pstAVIMacroFormat->stHdrlChunk, "hdrl", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile FindChildChunk hdrl failed.\r\n");
		return XOS_FALSE;
	}
	printf("hdrl: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stHdrlChunk.szType, pstAVIMacroFormat->stHdrlChunk.u32Size, 
			pstAVIMacroFormat->stHdrlChunk.u32Position, pstAVIMacroFormat->stHdrlChunk.szName);

	//���stMoviChunk���ݿ���Ϣ
	bRv = FindChildChunk(&pstAVIMacroFormat->stFileChunk, &pstAVIMacroFormat->stMoviChunk, "movi", 0);
	if(bRv == XOS_FALSE)
	{
		printf("AVIFileRead_OpenFile FindChildChunk movi failed.\r\n");
		return XOS_FALSE;
	}
	printf("movi: Type:%s Size:%d Pos:%d Name:%s\r\n", 
			pstAVIMacroFormat->stMoviChunk.szType, pstAVIMacroFormat->stMoviChunk.u32Size, 
			pstAVIMacroFormat->stMoviChunk.u32Position, pstAVIMacroFormat->stMoviChunk.szName);

	//���stIndexChunk�����������Ϣ
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
	//�ͷ�avi�洢�ļ��ṹ���ڴ�
	if (pstAVIMacroFormat != NULL)
	{
		if (&pstAVIMacroFormat->stFileChunk != NULL)
		{
			//�ͷ��ļ����
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

	//��stFileChunk���ҵ��ӿ�hdrlChunk,���õ�hdrlChunk��Ϣ
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

	//��hdrlChunk�����ҵ��ӿ�avihChunk,���õ�avihChunk��Ϣ
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

	//��ȡ����buff����stMainAVIHeader
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
	//����ͷ��Ϣ����
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

	//��ȡһ����ʱ,���뽫һ�����ĵ�һ��֡�ҵ�
	if (pstAVIStream->stAVIStreamHeader.fccType == streamtypeVIDEO) 
	{
		if (pstAVIStream->stAVIStreamHeader.fccHandler != encodertypeNULL) 
		{
			//ѹ����ͼ��������
			bRv = FindChildChunk(&pstAVIMacroFormat->stMoviChunk, &stMediaChunk, "00dc", 0);
		} 
		else 
		{
			//RGB������
			bRv = FindChildChunk(&pstAVIMacroFormat->stMoviChunk, &stMediaChunk, "00db", 0);
		}
	}
	else if (pstAVIStream->stAVIStreamHeader.fccType == streamtypeAUDIO) 
	{
		bRv = FindChildChunk(&pstAVIMacroFormat->stMoviChunk, &stMediaChunk, "01wb", 0);//��Ƶ������
	}
	
	//"idxl"�������Ϊ��
	if (&pstAVIMacroFormat->stIndexChunk != NULL) 
	{
		iIndexOffset = 0;
		//���С����0
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
			//��һ�ζ�����,������ȫ���������ŵ��ڴ���
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
		//�ҵ����ϵ�ý������
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
			//��������Ƶ
			if (bFindIFrame)
			{
				//��Ҫ��I֡
				if (stIndex.u32Flags != 0 && 
					(stIndex.u32ChunkID != indextype01wb) || (stIndex.u32ChunkID != indextype03wb))
				{
					//�ҵ���ƵI֡
					*piFindFrameType = AVIFILEREAD_FINDVIDEO;
					break;
				}
			}
			else
			{
				if (stIndex.u32ChunkID != indextype01wb && stIndex.u32ChunkID != indextype03wb)
				{
					//�ҵ���Ƶ֡
					*piFindFrameType = AVIFILEREAD_FINDVIDEO;
					break;
				}
			}
			//������
			(*pulIndexNum)++;
		}
		else if(*piFindFrameType == AVIFILEREAD_FINDAUDIO)
		{
			//��������Ƶ
			if (stIndex.u32ChunkID == indextype01wb || stIndex.u32ChunkID == indextype03wb)
			{
				//�ҵ���Ƶ֡
				break;
			}
			//������
			(*pulIndexNum)++;
		}
		else if (*piFindFrameType == AVIFILEREAD_FINDALL)
		{
			//ȫ����
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
	
	//��ȡ����֡
	bRv = FindChildChunk2(&pstAVIStream->stMoviChunk, &stMoviDataChunk, stIndex.u32Offset, NULL);
	if (bRv == XOS_FALSE) 
	{
		printf("AVIFileRead_Frame FindChildChunk2 failed.\r\n");
		return -1;
	}
	if (stMoviDataChunk.u32Size != stIndex.u32Size) //index�����м�¼�Ŀ��С��ʵ�ʵĴ�С�Ƚ�
	{
		printf("stMoviDataChunk.u32Size != stIndex.u32Offset.\r\n");
		return -1;
	} 

	if (*pulLen < (int)stMoviDataChunk.u32Size) 
	{
		printf("pFrameBuffer is overflow %d < %d.\r\n", *pulLen, stMoviDataChunk.u32Size);
		//Ҫ��ȡ������֡����*piLenС�����ݿ鳤��
		return -1;
	}

	rv = GetData(&stMoviDataChunk, pFrameBuffer, *pulLen);
	if (rv != (int)stMoviDataChunk.u32Size)
	{
		printf("AviFileRead_Frame GetData failed read:r%d.\r\n", rv);
		return -1;
	}

	//������ݿ鳤��
	*pulLen = stMoviDataChunk.u32Size;
	return 0;
}


