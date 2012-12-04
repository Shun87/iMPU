/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: CAVIWriter.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2009-11-6 17:04:26
 *  修正备注: 
 *  
 */

#include "../CAVIWriter.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
//	操作系统适配函数
//////////////////////////////////////////////////////////////////////////

#define FILEWRITER_SEEK_CUR	SEEK_CUR
#define FILEWRITER_SEEK_SET	SEEK_SET
#define FILEWRITER_SEEK_END	SEEK_END

typedef void* FILEWRITER_HANDLE;

//返回NULL,表示打开失败,其他表示打开的文件句柄
static FILEWRITER_HANDLE FileWriter_Open(const char *pszFileName)
{
	FILE *fin;
	fin = fopen(pszFileName, "wb+");
	if (fin == NULL)
	{
		return NULL;
	}
	return fin;
}

//返回读取到的字节数,-1表示失败,0表示cbBuffer为0或者文件已经读完
static int FileWriter_Read(FILEWRITER_HANDLE hFileWriter, void *pBuffer, int cbBuffer)
{
	int rv;
	rv = fread(pBuffer, 1, cbBuffer, (FILE *)hFileWriter);
	if (rv < 0)
	{
		return -1;
	}
	return rv;
}

//返回读取到的字节数,-1表示失败,0表示cbBuffer为0或者文件已经读完
static int FileWriter_Write(FILEWRITER_HANDLE hFileWriter, const void *pBuffer, int cbBuffer)
{
	int rv;
	rv = fwrite(pBuffer, 1, cbBuffer, (FILE *)hFileWriter);
	if (rv < 0)
	{
		return -1;
	}
	return rv;
}

//返回0表示成功,-1表示失败
static int FileWriter_Seek(FILEWRITER_HANDLE hFileWriter, long lOffset, int iOrigin)
{
	int rv;
	rv = fseek((FILE *)hFileWriter, lOffset, iOrigin);
	if (rv != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

static long FileWriter_Tell(FILEWRITER_HANDLE hFileWriter)
{
	return ftell((FILE *)hFileWriter);
}

static void FileWriter_Close(FILEWRITER_HANDLE hFileWriter)
{
	fclose((FILE *)hFileWriter);
	return ;
}

//////////////////////////////////////////////////////////////////////////
//	数据结构定义
//////////////////////////////////////////////////////////////////////////

#define SUBCHUNKFLAG_LEN	4
#define MOVIDATA_OFFSET		(2*1024)

//AVI file tags
#define AVIFILETAG_RIFF		"RIFF"
#define AVIFILETAG_AVI		"AVI "
#define AVIFILETAG_LIST		"LIST"
#define AVIFILETAG_JUNK		"JUNK"
#define AVIFILETAG_hdrl		"hdrl"
#define AVIFILETAG_avih		"avih"
#define AVIFILETAG_strl		"strl"
#define AVIFILETAG_strh		"strh"
#define AVIFILETAG_strf		"strf"
#define AVIFILETAG_strn		"strn"
#define AVIFILETAG_movi		"movi"
#define AVIFILETAG_idx1		"idx1"
#define AVIFILETAG_vids		"vids"
#define AVIFILETAG_PVTD		"PVTD"

#define AVIFILETAG_fxxdb	"%02ddb"
#define AVIFILETAG_fxxdc	"%02ddc"
#define AVIFILETAG_fxxwb	"%02dwb"

//AVI file structures and defines

typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef struct _MainAVIHeader {
    DWORD dwMicroSecPerFrame;
    DWORD dwMaxBytesPerSec;
    DWORD dwReserved1;
    DWORD dwFlags;
    DWORD dwTotalFrames;
    DWORD dwInitialFrames;
    DWORD dwStreams;
    DWORD dwSuggestedBufferSize;
    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwReserved2;
    DWORD dwReserved3;
    DWORD dwReserved4;
    DWORD dwReserved5;
} MainAVIHeader;

typedef struct _AVIStreamHeader {
	DWORD fccType;
    DWORD fccHandler;
    DWORD dwFlags;
    DWORD dwPriority;
    DWORD dwInitialFrames;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwStart;
    DWORD dwLength;
    DWORD dwSuggestedBufferSize;
    DWORD dwQuality;
    DWORD dwSampleSize;
	DWORD dwReserved2;
	DWORD dwReserved3;
} AVIStreamHeader;

typedef struct _AVIINDEXENTRY {
    DWORD ckid;
    DWORD dwFlags;
    DWORD dwChunkOffset;
    DWORD dwChunkLength;
} AVIINDEXENTRY;

//下面是程序内部使用的结构定义

typedef struct _TAVIIndex {
	char bUsed;
	char cFlags;
	char Reserved1;
	char Reserved2;
	DWORD ckid;
    DWORD dwChunkOffset;
    DWORD dwChunkLength;
} TAVIIndex;

typedef struct _TCAVIStream {
	int bUsed;							//这个流结构是否被使用
	int iStreamNo;						//这种类型的流的编号
	CAVIWRITER_HANDLE hWriter;		//这个流所属的CAVIWriter句柄
	unsigned int uiStreamHeaderOffset;	//流的头信息在文件中的偏移位置
	AVIStreamHeader aviStreamHeader;	//流的头信息
	char binStreamFormatBuffer[80];		//流的格式信息
	int iStreamFormatLen;				//流的格式信息的长度
	int iSampleNum;						//当前Sample的个数
	int iMaxSampleLen;					//最大帧长度
} TCAVIStream;

typedef struct _TCAVIFile {
	int bOpen;							//是否已经打开文件
	FILEWRITER_HANDLE hFileWriter;		//写文件的句柄
	TCAVIStream Streams[CAVIWRITER_MAX_STREAM_NUM];	
										//拥有流的结构数组
	int iStreamNum;						//拥有的流的数目
	unsigned int uiHeaderWriteOffset;	//写文件头的最大偏移
	unsigned int uiMoviWriteOffset;		//写movi的最大偏移
	unsigned int uiRIFFLengthOffset;	//RIFF长度信息在文件中的偏移位置
	unsigned int uiLISThdrlLengthOffset;//hdrl长度信息在文件中的偏移位置
	unsigned int uiLISTstrlLengthOffset;//strl长度信息在文件中的偏移位置
	unsigned int uiLISTmoviLengthOffset;//movi长度信息在文件中的偏移位置
	unsigned int uiMainAVIHeaderOffset;	//MainAVIHeader信息在文件中的偏移位置
	MainAVIHeader mainAVIHeader;		//AVI文件的头信息
	TAVIIndex *pIndexs;					//AVI文件的Index信息的数组
	int iMaxIndexNum;					//支持的最大的Index的数目
	int iIndexNum;						//当前的Index的数目
	char binPrivateData[CAVIWRITER_PRIVATEDATA_LEN];	//私有数据
} TCAVIFile;

//////////////////////////////////////////////////////////////////////////
//	接口实现函数
//////////////////////////////////////////////////////////////////////////

int CAVIWriter_Init(CAVIWRITER_HANDLE *phWriter, int iMaxIndexNum)
{
	TCAVIFile *pCAVIFile;
	*phWriter = 0;
	pCAVIFile = (TCAVIFile *)malloc(sizeof(TCAVIFile));
	if (pCAVIFile == 0)
	{
		return CAVIWRITER_E_MEMORY;
	}
	memset(pCAVIFile,0,sizeof(TCAVIFile));

	pCAVIFile->iMaxIndexNum = iMaxIndexNum;
	pCAVIFile->pIndexs = (TAVIIndex *)malloc(sizeof(TAVIIndex)*pCAVIFile->iMaxIndexNum);
	if (pCAVIFile->pIndexs == 0)
	{
		free(pCAVIFile);
		return CAVIWRITER_E_MEMORY;
	}
	memset(pCAVIFile->pIndexs,0,sizeof(sizeof(TAVIIndex)*pCAVIFile->iMaxIndexNum));

	*phWriter = pCAVIFile;
	return CAVIWRITER_E_OK;
}

void CAVIWriter_Close(CAVIWRITER_HANDLE hWriter)
{
	TCAVIFile *pCAVIFile;
	pCAVIFile = (TCAVIFile *)hWriter;
	free(pCAVIFile->pIndexs);
	free(pCAVIFile);
	return ;
}

int CAVIWriter_CreateFile(CAVIWRITER_HANDLE hWriter, const char *cpszFileName)
{
	DWORD dwLength;
	TCAVIFile *pCAVIFile;

	pCAVIFile = (TCAVIFile *)hWriter;

	if (pCAVIFile->bOpen == 1)
	{
		return CAVIWRITER_E_STATUS;
	}
	
	pCAVIFile->hFileWriter = FileWriter_Open(cpszFileName);
	if (pCAVIFile->hFileWriter == NULL)
	{
		return CAVIWRITER_E_FILE_OPEN;
	}

	//从头开始写
	FileWriter_Seek(pCAVIFile->hFileWriter ,0, FILEWRITER_SEEK_SET);
	
	//写RIFF
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_RIFF, strlen(AVIFILETAG_RIFF));
	
	//写RIFF长度，临时的，关闭文件的时候需要重新写
	pCAVIFile->uiRIFFLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写AVI
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_AVI, strlen(AVIFILETAG_AVI));

	//写hdrl List Chunk
	//写LIST
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_LIST, strlen(AVIFILETAG_LIST));
	
	pCAVIFile->uiLISThdrlLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	//写hdrl LIST长度，临时的，创建完所有的流的时候需要重新写
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写hdrl
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_hdrl, strlen(AVIFILETAG_hdrl));
	
	//写AVI File MainHeader
	//写avih
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_avih, strlen(AVIFILETAG_avih));
	
	//写avih长度
	dwLength = sizeof(MainAVIHeader);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写MainAVIHeader，临时的，关闭文件的时候需要重新写
	pCAVIFile->uiMainAVIHeaderOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	FileWriter_Write(pCAVIFile->hFileWriter, &pCAVIFile->mainAVIHeader, sizeof(pCAVIFile->mainAVIHeader));
	
	//记下头部写的位置,接下来添加流的时候,接着从这里写
	pCAVIFile->uiHeaderWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	//跳过2k,直接写movi
	FileWriter_Seek(pCAVIFile->hFileWriter, MOVIDATA_OFFSET-12, FILEWRITER_SEEK_SET);

	//写movi List Chunk
	//写LIST
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_LIST, strlen(AVIFILETAG_LIST));
	
	//写movi LIST长度，临时的，关闭文件的时候需要重新写
	pCAVIFile->uiLISTmoviLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写movie
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_movi, strlen(AVIFILETAG_movi));

	//记下movi开始写的位置
	pCAVIFile->uiMoviWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	pCAVIFile->bOpen = 1;

	return 0;
}

int CAVIWriter_CloseFile(CAVIWRITER_HANDLE hWriter)
{
	int i;
	DWORD dwLength;
    DWORD dwWidth = 352;
    DWORD dwHeight = 288;
	AVIINDEXENTRY aviIndexEntry;
	unsigned int uiAVIIndexPerTime;

	TCAVIFile *pCAVIFile;
	TCAVIStream *pStream;

	TAVIWriterBMPInfoHeader *pBMPInfoHeader;
	TAVIWriterWaveFormatEx *pWaveFormatEx;

	pCAVIFile = (TCAVIFile *)hWriter;
	
	//如果发现一帧都没有的话，就将各个流插入一个空帧
	for (i=0; i<CAVIWRITER_MAX_STREAM_NUM; i++)
	{
		if (pCAVIFile->Streams[i].bUsed == 0)
		{
			break;
		}

		pStream = &pCAVIFile->Streams[i];

		if (pStream->iSampleNum == 0)
		{
			//需要插空帧吗?怎么插???
			//WriteSample(pStream,0,2,AVIINDEX_FLAG_NULL);
		}
	}
	
	//写JUNK块
	//写JUNK
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiHeaderWriteOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_JUNK, strlen(AVIFILETAG_JUNK));

	//写JUNK长度
	dwLength = MOVIDATA_OFFSET - 12 - pCAVIFile->uiHeaderWriteOffset - 8;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//判断是否有足够空间写私有数据
	if (dwLength < 8+CAVIWRITER_PRIVATEDATA_LEN)
	{
		return CAVIWRITER_E_OVERFLOW;
	}

	//写PrivateData
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_PVTD, strlen(AVIFILETAG_PVTD));

	//写Private长度
	dwLength = CAVIWRITER_PRIVATEDATA_LEN;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写Private数据
	FileWriter_Write(pCAVIFile->hFileWriter, pCAVIFile->binPrivateData, CAVIWRITER_PRIVATEDATA_LEN);

	//写movi LIST长度
	dwLength = pCAVIFile->uiMoviWriteOffset - pCAVIFile->uiLISTmoviLengthOffset - sizeof(DWORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiLISTmoviLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiMoviWriteOffset, FILEWRITER_SEEK_SET);

	//写Index块
	//写idx1
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_idx1, strlen(AVIFILETAG_idx1));

	//写idx1长度
	dwLength = pCAVIFile->iIndexNum*sizeof(AVIINDEXENTRY);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	uiAVIIndexPerTime = pCAVIFile->iIndexNum/25;
	for (i=0; i<pCAVIFile->iIndexNum; i++)
	{
		aviIndexEntry.ckid = pCAVIFile->pIndexs[i].ckid;
		aviIndexEntry.dwFlags = pCAVIFile->pIndexs[i].cFlags;
		aviIndexEntry.dwChunkOffset = pCAVIFile->pIndexs[i].dwChunkOffset;
		aviIndexEntry.dwChunkLength = pCAVIFile->pIndexs[i].dwChunkLength;
		FileWriter_Write(pCAVIFile->hFileWriter, &aviIndexEntry, sizeof(aviIndexEntry));
		//写的太猛了,该成2.5秒写完,平均每次休息100ms
		if (uiAVIIndexPerTime > 0)
		{
			if ((i%uiAVIIndexPerTime) == 0)
			{
				//PC版不需要这个
				//Sleep(100);
			}
		}
	}

	//写RIFF长度
	dwLength = FileWriter_Tell(pCAVIFile->hFileWriter) - pCAVIFile->uiRIFFLengthOffset - sizeof(WORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiRIFFLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写前面留下的头
	//为个流填写AVIStreamHeader
	for (i=0; i<CAVIWRITER_MAX_STREAM_NUM; i++)
	{
		if (pCAVIFile->Streams[i].bUsed == 0)
		{
			break;
		}

		pStream = &pCAVIFile->Streams[i];

		if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeVIDEO)
		{
			pBMPInfoHeader = (TAVIWriterBMPInfoHeader *)pStream->binStreamFormatBuffer;
			dwWidth = pBMPInfoHeader->biWidth;
			dwHeight = pBMPInfoHeader->biHeight;

			pStream->aviStreamHeader.dwLength = pStream->iSampleNum;
			pStream->aviStreamHeader.dwSuggestedBufferSize = pStream->iMaxSampleLen;
			pStream->aviStreamHeader.dwQuality = 0;
			pStream->aviStreamHeader.dwSampleSize = 0;
			pStream->aviStreamHeader.dwReserved3 = (dwHeight<<16) + dwWidth;

			pCAVIFile->mainAVIHeader.dwTotalFrames = pStream->iSampleNum;
			pCAVIFile->mainAVIHeader.dwSuggestedBufferSize = pStream->iMaxSampleLen;

			FileWriter_Seek(pCAVIFile->hFileWriter, pStream->uiStreamHeaderOffset, FILEWRITER_SEEK_SET);
			FileWriter_Write(pCAVIFile->hFileWriter, &pStream->aviStreamHeader, sizeof(pStream->aviStreamHeader));
		}
		else if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeAUDIO)
		{
			pStream->aviStreamHeader.dwLength = pStream->iSampleNum;
			pStream->aviStreamHeader.dwSuggestedBufferSize = pStream->iMaxSampleLen;
			pStream->aviStreamHeader.dwQuality = 0;

			//目前只能够存PCM编码的音频
			pWaveFormatEx = (TAVIWriterWaveFormatEx *)pStream->binStreamFormatBuffer;				
			pStream->aviStreamHeader.dwSampleSize = pWaveFormatEx->nBlockAlign;

			FileWriter_Seek(pCAVIFile->hFileWriter, pStream->uiStreamHeaderOffset, FILEWRITER_SEEK_SET);
			FileWriter_Write(pCAVIFile->hFileWriter, &pStream->aviStreamHeader, sizeof(pStream->aviStreamHeader));
		}
		else
		{
			//不认识的流,啥也不做,不可能到这里
			return CAVIWRITER_E_FCCTYPE;
		}
	}

	//写MainAVIHeader
	//pCAVIFile->mainAVIHeader.dwMicroSecPerFrame = ; //前面写过了
	pCAVIFile->mainAVIHeader.dwMaxBytesPerSec = 1024*1024;	//这个值是随便估计的
		//#define AVIF_HASINDEX		0x00000010	// Index at end of file
		//#define AVIF_MUSTUSEINDEX	0x00000020
		//#define AVIF_ISINTERLEAVED	0x00000100
		//#define AVIF_TRUSTCKTYPE	0x00000800	// Use CKType to find key frames
	pCAVIFile->mainAVIHeader.dwFlags = 0x00000810;			//windows写出来的文件就这么多
	//pCAVIFile->mainAVIHeader.dwTotalFrames = ; //前面写过了
	pCAVIFile->mainAVIHeader.dwStreams = pCAVIFile->iStreamNum;
	//pCAVIFile->mainAVIHeader.dwSuggestedBufferSize = ; //前面写过了
	pCAVIFile->mainAVIHeader.dwWidth = dwWidth;
	pCAVIFile->mainAVIHeader.dwHeight = dwHeight;

	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiMainAVIHeaderOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &pCAVIFile->mainAVIHeader, sizeof(pCAVIFile->mainAVIHeader));

	//关闭文件
	FileWriter_Close(pCAVIFile->hFileWriter);
	pCAVIFile->hFileWriter = NULL;

	//清空内存
	pCAVIFile->bOpen = 0;
	memset(pCAVIFile->Streams, 0, sizeof(TCAVIStream)*CAVIWRITER_MAX_STREAM_NUM);
	memset(&pCAVIFile->mainAVIHeader, 0, sizeof(MainAVIHeader));
	memset(pCAVIFile->pIndexs, 0, sizeof(sizeof(TAVIIndex)*pCAVIFile->iMaxIndexNum));
	pCAVIFile->iIndexNum = 0;

	return CAVIWRITER_E_OK;
}

void CAVIWriter_SetPrivateData(CAVIWRITER_HANDLE hWriter, char binData[CAVIWRITER_PRIVATEDATA_LEN])
{
	TCAVIFile *pCAVIFile;
	pCAVIFile = (TCAVIFile *)hWriter;
	memcpy(pCAVIFile->binPrivateData, binData, CAVIWRITER_PRIVATEDATA_LEN);
	return ;
}

int CAVIWriter_CreateStream(CAVIWRITER_HANDLE hWriter, CAVIWRITER_STREAMHANDLE *phStream, 
							const TCAVIWriterStreamInfo *pInfo, const void *pFormat, int iFormatLen)
{
	int i;
	DWORD dwLength;
	TCAVIStream *pStream;
	TCAVIFile *pCAVIFile;
	unsigned char ucAlignByte = 0;
	TAVIWriterBMPInfoHeader *pBMPInfoHeader;

	*phStream = 0;
	pCAVIFile = (TCAVIFile *)hWriter;
	
	for (i=0; i<CAVIWRITER_MAX_STREAM_NUM; i++)
	{
		if (pCAVIFile->Streams[i].bUsed != 0)
		{
			continue;
		}
		pStream = &pCAVIFile->Streams[i];
		break;
	}
	if (i == CAVIWRITER_MAX_STREAM_NUM)
	{
		return CAVIWRITER_E_STREAMCOUNT;
	}

	//判断媒体类型
	if (pInfo->fccType == CAVIWRITER_streamtypeVIDEO)
	{
		pCAVIFile->mainAVIHeader.dwMicroSecPerFrame = 1000*1000/pInfo->dwRate;
	}
	else if (pInfo->fccType == CAVIWRITER_streamtypeAUDIO)
	{
	}
	else
	{
		return CAVIWRITER_E_FCCTYPE;
	}

	memset(pStream, 0, sizeof(TCAVIStream));
	pStream->bUsed = 1;
	pStream->iStreamNo = pCAVIFile->iStreamNum;
	pStream->hWriter = hWriter;
	pStream->aviStreamHeader.fccType = pInfo->fccType;
	pStream->aviStreamHeader.fccHandler = pInfo->fccHandler;
	pStream->aviStreamHeader.dwRate = pInfo->dwRate;
	pStream->aviStreamHeader.dwScale = pInfo->dwScale;

	//直接调到头部写的位置
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiHeaderWriteOffset, FILEWRITER_SEEK_SET);

	//写strl List Chunk
	//写LIST
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_LIST, strlen(AVIFILETAG_LIST));
	
	//写strl LIST长度，临时的，创建这个完流的时候需要重新写
	pCAVIFile->uiLISTstrlLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写strl
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strl, strlen(AVIFILETAG_strl));

	//写Stream header
	//写strh
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strh, strlen(AVIFILETAG_strh));

	//写strh长度
	dwLength = sizeof(AVIStreamHeader);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写AVIStreamHeader，临时的，关闭文件的时候需要重新写
	pStream->uiStreamHeaderOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	FileWriter_Write(pCAVIFile->hFileWriter, &pStream->aviStreamHeader, sizeof(pStream->aviStreamHeader));

	//写Stream format
	//写strf
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strf, strlen(AVIFILETAG_strf));

	//写strf长度
	dwLength = iFormatLen;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写AVIStreamFormat
	if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeVIDEO){
		pBMPInfoHeader = (TAVIWriterBMPInfoHeader *)pFormat;
		pBMPInfoHeader->biCompression = pStream->aviStreamHeader.fccHandler; 
	}
	FileWriter_Write(pCAVIFile->hFileWriter, pFormat, iFormatLen);
	
	//写Stream name
	//写strn
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strn, strlen(AVIFILETAG_strn));

	//写strn长度
	dwLength = strlen(pInfo->szName);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写StreamName
	FileWriter_Write(pCAVIFile->hFileWriter, pInfo->szName, strlen(pInfo->szName));
	if ((dwLength % 2) == 1)
	{
		//增加对齐字节
		FileWriter_Write(pCAVIFile->hFileWriter, &ucAlignByte, 1);
	}

	//记下头部写的位置
	pCAVIFile->uiHeaderWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	//写strl LIST长度,最终的
	dwLength = FileWriter_Tell(pCAVIFile->hFileWriter) - pCAVIFile->uiLISTstrlLengthOffset - sizeof(DWORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiLISTstrlLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//写hdrl LIST长度
	dwLength = pCAVIFile->uiHeaderWriteOffset - pCAVIFile->uiLISThdrlLengthOffset - sizeof(DWORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiLISThdrlLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//增加流的数目
	pCAVIFile->iStreamNum ++;

	//记下AVIStreamFormat
	memcpy(pStream->binStreamFormatBuffer, pFormat, iFormatLen);
	pStream->iStreamFormatLen = iFormatLen;

	*phStream = pStream;

	return CAVIWRITER_E_OK;
}

int CAVIWriter_WriteSample(CAVIWRITER_STREAMHANDLE hStream, const char *pSample, 
						   int iSampleLen, int iSampleNo, unsigned int uiFlags)
{
	DWORD dwSubChunkLen;
	unsigned char ucAlignByte = 0;
	char szSubChunkFlag[SUBCHUNKFLAG_LEN+1];
	TCAVIStream *pStream;
	TCAVIFile *pCAVIFile;

	pStream = (TCAVIStream *)hStream;
	pCAVIFile = (TCAVIFile *)pStream->hWriter;
	
	//判断帧序号是否连续
	if (iSampleNo != pStream->iSampleNum)
	{
		//不能够改前面已经插入的帧，返回错误
		return CAVIWRITER_E_SAMPLENO;
	}
	
	//如果是音频帧
	if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeAUDIO)
	{
		//音频帧都是I帧
		uiFlags = CAVIWRITER_AVIINDEX_FLAG_KEYFRAME;
	}

	//在内存中记下索引
	if (pCAVIFile->iIndexNum >= pCAVIFile->iMaxIndexNum)
	{
		//如果AVIIndex用完了，返回错误
		return CAVIWRITER_E_NOVALIDINDEX;
	}
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].bUsed = 1;
	if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeVIDEO)
	{
		memset(szSubChunkFlag, 0, sizeof(szSubChunkFlag));
		if (uiFlags == CAVIWRITER_AVIINDEX_FLAG_KEYFRAME)
		{
			sprintf(szSubChunkFlag, AVIFILETAG_fxxdb, pStream->iStreamNo);
		}
		else
		{
			sprintf(szSubChunkFlag, AVIFILETAG_fxxdc, pStream->iStreamNo);
		}
		memcpy(&pCAVIFile->pIndexs[pCAVIFile->iIndexNum].ckid, szSubChunkFlag, SUBCHUNKFLAG_LEN);
	}
	else if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeAUDIO)
	{
		memset(szSubChunkFlag, 0, sizeof(szSubChunkFlag));
		sprintf(szSubChunkFlag, AVIFILETAG_fxxwb, pStream->iStreamNo);	
		memcpy(&pCAVIFile->pIndexs[pCAVIFile->iIndexNum].ckid, szSubChunkFlag, SUBCHUNKFLAG_LEN);
	}
	else
	{
		return CAVIWRITER_E_FCCTYPE;
	}
	
	//调到movi写的位置
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiMoviWriteOffset, FILEWRITER_SEEK_SET);
	
	//写SubChunkFlag
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].dwChunkOffset = 
		pCAVIFile->uiMoviWriteOffset - pCAVIFile->uiLISTmoviLengthOffset - sizeof(DWORD);
	FileWriter_Write(pCAVIFile->hFileWriter, szSubChunkFlag, SUBCHUNKFLAG_LEN);

	//写dwSubChunkLen
	dwSubChunkLen = iSampleLen;
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].dwChunkLength = dwSubChunkLen;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwSubChunkLen, sizeof(dwSubChunkLen));

	//写SubChunkData
	FileWriter_Write(pCAVIFile->hFileWriter, pSample, iSampleLen);
	if ((dwSubChunkLen % 2) == 1)
	{
		//增加对齐字节
		FileWriter_Write(pCAVIFile->hFileWriter, &ucAlignByte, 1);
	}

	//记下movi写的位置
	pCAVIFile->uiMoviWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	//记下SubChunkFlags
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].cFlags = uiFlags;
	
	//记下最大的帧长度
	if (pStream->iMaxSampleLen < iSampleLen)
	{
		pStream->iMaxSampleLen = iSampleLen;
	}

	pCAVIFile->iIndexNum ++;
	pStream->iSampleNum ++;

	return CAVIWRITER_E_OK;
}
