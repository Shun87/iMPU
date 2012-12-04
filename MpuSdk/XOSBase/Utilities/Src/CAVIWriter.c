/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: CAVIWriter.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2009-11-6 17:04:26
 *  ������ע: 
 *  
 */

#include "../CAVIWriter.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
//	����ϵͳ���亯��
//////////////////////////////////////////////////////////////////////////

#define FILEWRITER_SEEK_CUR	SEEK_CUR
#define FILEWRITER_SEEK_SET	SEEK_SET
#define FILEWRITER_SEEK_END	SEEK_END

typedef void* FILEWRITER_HANDLE;

//����NULL,��ʾ��ʧ��,������ʾ�򿪵��ļ����
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

//���ض�ȡ�����ֽ���,-1��ʾʧ��,0��ʾcbBufferΪ0�����ļ��Ѿ�����
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

//���ض�ȡ�����ֽ���,-1��ʾʧ��,0��ʾcbBufferΪ0�����ļ��Ѿ�����
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

//����0��ʾ�ɹ�,-1��ʾʧ��
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
//	���ݽṹ����
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

//�����ǳ����ڲ�ʹ�õĽṹ����

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
	int bUsed;							//������ṹ�Ƿ�ʹ��
	int iStreamNo;						//�������͵����ı��
	CAVIWRITER_HANDLE hWriter;		//�����������CAVIWriter���
	unsigned int uiStreamHeaderOffset;	//����ͷ��Ϣ���ļ��е�ƫ��λ��
	AVIStreamHeader aviStreamHeader;	//����ͷ��Ϣ
	char binStreamFormatBuffer[80];		//���ĸ�ʽ��Ϣ
	int iStreamFormatLen;				//���ĸ�ʽ��Ϣ�ĳ���
	int iSampleNum;						//��ǰSample�ĸ���
	int iMaxSampleLen;					//���֡����
} TCAVIStream;

typedef struct _TCAVIFile {
	int bOpen;							//�Ƿ��Ѿ����ļ�
	FILEWRITER_HANDLE hFileWriter;		//д�ļ��ľ��
	TCAVIStream Streams[CAVIWRITER_MAX_STREAM_NUM];	
										//ӵ�����Ľṹ����
	int iStreamNum;						//ӵ�е�������Ŀ
	unsigned int uiHeaderWriteOffset;	//д�ļ�ͷ�����ƫ��
	unsigned int uiMoviWriteOffset;		//дmovi�����ƫ��
	unsigned int uiRIFFLengthOffset;	//RIFF������Ϣ���ļ��е�ƫ��λ��
	unsigned int uiLISThdrlLengthOffset;//hdrl������Ϣ���ļ��е�ƫ��λ��
	unsigned int uiLISTstrlLengthOffset;//strl������Ϣ���ļ��е�ƫ��λ��
	unsigned int uiLISTmoviLengthOffset;//movi������Ϣ���ļ��е�ƫ��λ��
	unsigned int uiMainAVIHeaderOffset;	//MainAVIHeader��Ϣ���ļ��е�ƫ��λ��
	MainAVIHeader mainAVIHeader;		//AVI�ļ���ͷ��Ϣ
	TAVIIndex *pIndexs;					//AVI�ļ���Index��Ϣ������
	int iMaxIndexNum;					//֧�ֵ�����Index����Ŀ
	int iIndexNum;						//��ǰ��Index����Ŀ
	char binPrivateData[CAVIWRITER_PRIVATEDATA_LEN];	//˽������
} TCAVIFile;

//////////////////////////////////////////////////////////////////////////
//	�ӿ�ʵ�ֺ���
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

	//��ͷ��ʼд
	FileWriter_Seek(pCAVIFile->hFileWriter ,0, FILEWRITER_SEEK_SET);
	
	//дRIFF
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_RIFF, strlen(AVIFILETAG_RIFF));
	
	//дRIFF���ȣ���ʱ�ģ��ر��ļ���ʱ����Ҫ����д
	pCAVIFile->uiRIFFLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дAVI
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_AVI, strlen(AVIFILETAG_AVI));

	//дhdrl List Chunk
	//дLIST
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_LIST, strlen(AVIFILETAG_LIST));
	
	pCAVIFile->uiLISThdrlLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	//дhdrl LIST���ȣ���ʱ�ģ����������е�����ʱ����Ҫ����д
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дhdrl
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_hdrl, strlen(AVIFILETAG_hdrl));
	
	//дAVI File MainHeader
	//дavih
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_avih, strlen(AVIFILETAG_avih));
	
	//дavih����
	dwLength = sizeof(MainAVIHeader);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дMainAVIHeader����ʱ�ģ��ر��ļ���ʱ����Ҫ����д
	pCAVIFile->uiMainAVIHeaderOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	FileWriter_Write(pCAVIFile->hFileWriter, &pCAVIFile->mainAVIHeader, sizeof(pCAVIFile->mainAVIHeader));
	
	//����ͷ��д��λ��,�������������ʱ��,���Ŵ�����д
	pCAVIFile->uiHeaderWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	//����2k,ֱ��дmovi
	FileWriter_Seek(pCAVIFile->hFileWriter, MOVIDATA_OFFSET-12, FILEWRITER_SEEK_SET);

	//дmovi List Chunk
	//дLIST
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_LIST, strlen(AVIFILETAG_LIST));
	
	//дmovi LIST���ȣ���ʱ�ģ��ر��ļ���ʱ����Ҫ����д
	pCAVIFile->uiLISTmoviLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дmovie
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_movi, strlen(AVIFILETAG_movi));

	//����movi��ʼд��λ��
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
	
	//�������һ֡��û�еĻ����ͽ�����������һ����֡
	for (i=0; i<CAVIWRITER_MAX_STREAM_NUM; i++)
	{
		if (pCAVIFile->Streams[i].bUsed == 0)
		{
			break;
		}

		pStream = &pCAVIFile->Streams[i];

		if (pStream->iSampleNum == 0)
		{
			//��Ҫ���֡��?��ô��???
			//WriteSample(pStream,0,2,AVIINDEX_FLAG_NULL);
		}
	}
	
	//дJUNK��
	//дJUNK
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiHeaderWriteOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_JUNK, strlen(AVIFILETAG_JUNK));

	//дJUNK����
	dwLength = MOVIDATA_OFFSET - 12 - pCAVIFile->uiHeaderWriteOffset - 8;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//�ж��Ƿ����㹻�ռ�д˽������
	if (dwLength < 8+CAVIWRITER_PRIVATEDATA_LEN)
	{
		return CAVIWRITER_E_OVERFLOW;
	}

	//дPrivateData
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_PVTD, strlen(AVIFILETAG_PVTD));

	//дPrivate����
	dwLength = CAVIWRITER_PRIVATEDATA_LEN;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дPrivate����
	FileWriter_Write(pCAVIFile->hFileWriter, pCAVIFile->binPrivateData, CAVIWRITER_PRIVATEDATA_LEN);

	//дmovi LIST����
	dwLength = pCAVIFile->uiMoviWriteOffset - pCAVIFile->uiLISTmoviLengthOffset - sizeof(DWORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiLISTmoviLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiMoviWriteOffset, FILEWRITER_SEEK_SET);

	//дIndex��
	//дidx1
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_idx1, strlen(AVIFILETAG_idx1));

	//дidx1����
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
		//д��̫����,�ó�2.5��д��,ƽ��ÿ����Ϣ100ms
		if (uiAVIIndexPerTime > 0)
		{
			if ((i%uiAVIIndexPerTime) == 0)
			{
				//PC�治��Ҫ���
				//Sleep(100);
			}
		}
	}

	//дRIFF����
	dwLength = FileWriter_Tell(pCAVIFile->hFileWriter) - pCAVIFile->uiRIFFLengthOffset - sizeof(WORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiRIFFLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дǰ�����µ�ͷ
	//Ϊ������дAVIStreamHeader
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

			//Ŀǰֻ�ܹ���PCM�������Ƶ
			pWaveFormatEx = (TAVIWriterWaveFormatEx *)pStream->binStreamFormatBuffer;				
			pStream->aviStreamHeader.dwSampleSize = pWaveFormatEx->nBlockAlign;

			FileWriter_Seek(pCAVIFile->hFileWriter, pStream->uiStreamHeaderOffset, FILEWRITER_SEEK_SET);
			FileWriter_Write(pCAVIFile->hFileWriter, &pStream->aviStreamHeader, sizeof(pStream->aviStreamHeader));
		}
		else
		{
			//����ʶ����,ɶҲ����,�����ܵ�����
			return CAVIWRITER_E_FCCTYPE;
		}
	}

	//дMainAVIHeader
	//pCAVIFile->mainAVIHeader.dwMicroSecPerFrame = ; //ǰ��д����
	pCAVIFile->mainAVIHeader.dwMaxBytesPerSec = 1024*1024;	//���ֵ�������Ƶ�
		//#define AVIF_HASINDEX		0x00000010	// Index at end of file
		//#define AVIF_MUSTUSEINDEX	0x00000020
		//#define AVIF_ISINTERLEAVED	0x00000100
		//#define AVIF_TRUSTCKTYPE	0x00000800	// Use CKType to find key frames
	pCAVIFile->mainAVIHeader.dwFlags = 0x00000810;			//windowsд�������ļ�����ô��
	//pCAVIFile->mainAVIHeader.dwTotalFrames = ; //ǰ��д����
	pCAVIFile->mainAVIHeader.dwStreams = pCAVIFile->iStreamNum;
	//pCAVIFile->mainAVIHeader.dwSuggestedBufferSize = ; //ǰ��д����
	pCAVIFile->mainAVIHeader.dwWidth = dwWidth;
	pCAVIFile->mainAVIHeader.dwHeight = dwHeight;

	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiMainAVIHeaderOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &pCAVIFile->mainAVIHeader, sizeof(pCAVIFile->mainAVIHeader));

	//�ر��ļ�
	FileWriter_Close(pCAVIFile->hFileWriter);
	pCAVIFile->hFileWriter = NULL;

	//����ڴ�
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

	//�ж�ý������
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

	//ֱ�ӵ���ͷ��д��λ��
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiHeaderWriteOffset, FILEWRITER_SEEK_SET);

	//дstrl List Chunk
	//дLIST
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_LIST, strlen(AVIFILETAG_LIST));
	
	//дstrl LIST���ȣ���ʱ�ģ��������������ʱ����Ҫ����д
	pCAVIFile->uiLISTstrlLengthOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	dwLength = 0;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дstrl
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strl, strlen(AVIFILETAG_strl));

	//дStream header
	//дstrh
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strh, strlen(AVIFILETAG_strh));

	//дstrh����
	dwLength = sizeof(AVIStreamHeader);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дAVIStreamHeader����ʱ�ģ��ر��ļ���ʱ����Ҫ����д
	pStream->uiStreamHeaderOffset = FileWriter_Tell(pCAVIFile->hFileWriter);
	FileWriter_Write(pCAVIFile->hFileWriter, &pStream->aviStreamHeader, sizeof(pStream->aviStreamHeader));

	//дStream format
	//дstrf
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strf, strlen(AVIFILETAG_strf));

	//дstrf����
	dwLength = iFormatLen;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дAVIStreamFormat
	if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeVIDEO){
		pBMPInfoHeader = (TAVIWriterBMPInfoHeader *)pFormat;
		pBMPInfoHeader->biCompression = pStream->aviStreamHeader.fccHandler; 
	}
	FileWriter_Write(pCAVIFile->hFileWriter, pFormat, iFormatLen);
	
	//дStream name
	//дstrn
	FileWriter_Write(pCAVIFile->hFileWriter, AVIFILETAG_strn, strlen(AVIFILETAG_strn));

	//дstrn����
	dwLength = strlen(pInfo->szName);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дStreamName
	FileWriter_Write(pCAVIFile->hFileWriter, pInfo->szName, strlen(pInfo->szName));
	if ((dwLength % 2) == 1)
	{
		//���Ӷ����ֽ�
		FileWriter_Write(pCAVIFile->hFileWriter, &ucAlignByte, 1);
	}

	//����ͷ��д��λ��
	pCAVIFile->uiHeaderWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	//дstrl LIST����,���յ�
	dwLength = FileWriter_Tell(pCAVIFile->hFileWriter) - pCAVIFile->uiLISTstrlLengthOffset - sizeof(DWORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiLISTstrlLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//дhdrl LIST����
	dwLength = pCAVIFile->uiHeaderWriteOffset - pCAVIFile->uiLISThdrlLengthOffset - sizeof(DWORD);
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiLISThdrlLengthOffset, FILEWRITER_SEEK_SET);
	FileWriter_Write(pCAVIFile->hFileWriter, &dwLength, sizeof(dwLength));

	//����������Ŀ
	pCAVIFile->iStreamNum ++;

	//����AVIStreamFormat
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
	
	//�ж�֡����Ƿ�����
	if (iSampleNo != pStream->iSampleNum)
	{
		//���ܹ���ǰ���Ѿ������֡�����ش���
		return CAVIWRITER_E_SAMPLENO;
	}
	
	//�������Ƶ֡
	if (pStream->aviStreamHeader.fccType == CAVIWRITER_streamtypeAUDIO)
	{
		//��Ƶ֡����I֡
		uiFlags = CAVIWRITER_AVIINDEX_FLAG_KEYFRAME;
	}

	//���ڴ��м�������
	if (pCAVIFile->iIndexNum >= pCAVIFile->iMaxIndexNum)
	{
		//���AVIIndex�����ˣ����ش���
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
	
	//����moviд��λ��
	FileWriter_Seek(pCAVIFile->hFileWriter, pCAVIFile->uiMoviWriteOffset, FILEWRITER_SEEK_SET);
	
	//дSubChunkFlag
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].dwChunkOffset = 
		pCAVIFile->uiMoviWriteOffset - pCAVIFile->uiLISTmoviLengthOffset - sizeof(DWORD);
	FileWriter_Write(pCAVIFile->hFileWriter, szSubChunkFlag, SUBCHUNKFLAG_LEN);

	//дdwSubChunkLen
	dwSubChunkLen = iSampleLen;
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].dwChunkLength = dwSubChunkLen;
	FileWriter_Write(pCAVIFile->hFileWriter, &dwSubChunkLen, sizeof(dwSubChunkLen));

	//дSubChunkData
	FileWriter_Write(pCAVIFile->hFileWriter, pSample, iSampleLen);
	if ((dwSubChunkLen % 2) == 1)
	{
		//���Ӷ����ֽ�
		FileWriter_Write(pCAVIFile->hFileWriter, &ucAlignByte, 1);
	}

	//����moviд��λ��
	pCAVIFile->uiMoviWriteOffset = FileWriter_Tell(pCAVIFile->hFileWriter);

	//����SubChunkFlags
	pCAVIFile->pIndexs[pCAVIFile->iIndexNum].cFlags = uiFlags;
	
	//��������֡����
	if (pStream->iMaxSampleLen < iSampleLen)
	{
		pStream->iMaxSampleLen = iSampleLen;
	}

	pCAVIFile->iIndexNum ++;
	pStream->iSampleNum ++;

	return CAVIWRITER_E_OK;
}
