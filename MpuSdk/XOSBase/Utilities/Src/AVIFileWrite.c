/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: AVIFileWrite.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-6-21 20:27:53
 *  修正备注: 
 *  
 */

#include "../../XOS/XOS.h"
#include "../AVIFileRead.h"
#include "../AVIFileWrite.h"

#define SUBCHUNKFLAG_LEN	4

//AVI file tags
#define AVIFILETAG_RIFF		"RIFF"
#define AVIFILETAG_AVI		"AVI "
#define AVIFILETAG_LIST		"LIST"
#define AVIFILETAG_hdrl		"hdrl"
#define AVIFILETAG_avih		"avih"
#define AVIFILETAG_strl		"strl"
#define AVIFILETAG_strh		"strh"
#define AVIFILETAG_strf		"strf"
#define AVIFILETAG_strd		"strd"
#define AVIFILETAG_strn		"strn"
#define AVIFILETAG_movi		"movi"
#define AVIFILETAG_00db		"00db"
#define AVIFILETAG_00dc		"00dc"
#define AVIFILETAG_01wb		"01wb"
#define AVIFILETAG_00pc		"00pc"
#define AVIFILETAG_rec		"rec "
#define AVIFILETAG_idx1		"idx1"
#define AVIFILETAG_DIVX		"DIVX"
#define AVIFILETAG_vids		"vids"

//AVI file structures and defines

//typedef unsigned int XOS_U32;
//typedef unsigned short XOS_U16;

typedef struct MainAVIHeader{
    XOS_U32 dwMicroSecPerFrame;
    XOS_U32 dwMaxBytesPerSec;
    XOS_U32 dwReserved1;
    XOS_U32 dwFlags;
    XOS_U32 dwTotalFrames;
    XOS_U32 dwInitialFrames;
    XOS_U32 dwStreams;
    XOS_U32 dwSuggestedBufferSize;
    XOS_U32 dwWidth;
    XOS_U32 dwHeight;
    XOS_U32 dwReserved2;
    XOS_U32 dwReserved3;
    XOS_U32 dwReserved4;
    XOS_U32 dwReserved5;
}MainAVIHeader;

#define AVIF_HASINDEX		0x00000010  // Index at end of file?
#define AVIF_MUSTUSEINDEX	0x00000020
#define AVIF_ISINTERLEAVED	0x00000100
#define AVIF_TRUSTCKTYPE	0x00000800	// Use CKType to find key frames?
#define AVIF_WASCAPTUREFILE	0x00010000
#define AVIF_COPYRIGHTED	0x00020000

#define AVIF_KNOWN_FLAGS	0x00030130

#define Defined_MainAVIHeader_Size      (14*4)

#define AVISF_DISABLED			0x00000001
#define AVISF_VIDEO_PALCHANGES	0x00010000

#define AVISF_KNOWN_FLAGS		0x00010001

#define Defined_AVIStreamHeader_Size_old  (12*4)
#define Defined_AVIStreamHeader_Size      (14*4)

typedef struct AVIPALCHANGE{
    unsigned char bFirstEntry;
    unsigned char bNumEntries;
    unsigned short wFlags;
} AVIPALCHANGE;

typedef struct AVIINDEXENTRY{
    XOS_U32 ckid;
    XOS_U32 dwFlags;
    XOS_U32 dwChunkOffset;
    XOS_U32 dwChunkLength;
} AVIINDEXENTRY;

#define AVIIF_LIST		0x00000001
#define AVIIF_KEYFRAME	0x00000010
#define AVIIF_FIRSTPART	0x00000020
#define AVIIF_LASTPART	0x00000040
#define AVIIF_MIDPART	(AVIIF_LASTPART | AVIIF_FIRSTPART)
#define AVIIF_NOTIME	0x00000100
#define AVIIF_COMPUSE	0x0fff0000
#define AVIIF_KNOWN_FLAGS 0x0fff0171

#define Defined_AVIINDEXENTRY_Size	(4*4)

/**************************************************************************************
*
*	CAVIFile implements
*
**************************************************************************************/

//CAVIFile structures and defines

const char AVIIndexTypes[][5] = {
	AVIFILETAG_00db,
	AVIFILETAG_00dc,
	AVIFILETAG_01wb,
	AVIFILETAG_00pc
};

#define AVIINDEX_TYPE_00db		0
#define AVIINDEX_TYPE_00dc		1
#define AVIINDEX_TYPE_01wb		2
#define AVIINDEX_TYPE_00pc		3

const unsigned int AVIIndexFlags[] = {
	0,
	AVIIF_LIST,
	AVIIF_KEYFRAME,
	AVIIF_FIRSTPART,
	AVIIF_LASTPART,
	AVIIF_MIDPART,
	AVIIF_NOTIME,
	AVIIF_COMPUSE,
	AVIIF_KNOWN_FLAGS
};

typedef struct _TAVIIndex {
	char bUsed;
	char Type;
	char Flags;
	char Reserved1;
    XOS_U32 dwChunkOffset;
    XOS_U32 dwChunkLength;
}TAVIIndex;

typedef struct _TCAVIStream {
	int bUsed;							//这个流结构是否被使用
	XOS_HANDLE hCAVIFile;					//这个流所属的CAVIFile的句柄
	unsigned int AVIStreamHeaderPos;	//流的头信息在文件中的偏移位置
	AVIStreamHeader aviStreamHeader;	//流的头信息
	char pAVIStreamFormat[80];			//流的格式信息
	int AVIStreamFormatLen;				//流的格式信息的长度
	int SampleNum;						//当前Sample的个数
	unsigned int SampleLen;				//帧的长度,对PCM的音频数据有效
	int bHasNullSample;					//这个流是否已经插入过空帧了
	int NullSampleIndexNo;				//这个流的空帧在总Index中的位置
}TCAVIStream,*CAVISTREAM_HANDLE;

typedef struct _TCAVIFile {
	int bOpen;									//CAVIFile模块是否已经打开文件
	int bWrittenSample;							//已经写过帧了
	XOS_HANDLE hFile;								//AVI文件的句柄
	CAVISTREAM_HANDLE *hCAVIStreams;			//CAVIFile所拥有的流的句柄的数组
	unsigned int MaxCAVIStreamNum;				//CAVIFile支持的最大的流的数目
	unsigned int CAVIStreamNum;					//CAVIFile所拥有的流的数目
	unsigned int WritePos;						//CAVIFile写文件的最大便宜,其实就是文件当前的大小
	unsigned int RIFFLengthPos;					//RIFF长度信息在文件中的偏移位置
	unsigned int LISThdrlLengthPos;				//hdrl长度信息在文件中的偏移位置
	unsigned int LISTstrlLengthPos;				//strl长度信息在文件中的偏移位置
	unsigned int LISTmoviLengthPos;				//movi长度信息在文件中的偏移位置
	unsigned int MainAVIHeaderPos;				//MainAVIHeader信息在文件中的偏移位置
	MainAVIHeader mainAVIHeader;				//AVI文件的头信息
//	TAVIIndex * AVIIndexs;						//AVI文件的Index信息的数组
	AVIINDEXENTRY * AVIIndexs;					//AVI文件的Index信息的数组
	unsigned int MaxAVIIndexNum;				//CAVIFile支持的最大的Index的数目
	unsigned int AVIIndexNum;					//CAVIFile当前的Index的数目
}TCAVIFile,*CAVIFILE_HANDLE;


extern CAVIFile_Open g_pAVIFile_Open;
extern CAVIFile_Seek g_pAVIFile_Seek;
extern CAVIFile_Read g_pAVIFile_Read;
extern CAVIFile_Write g_pAVIFile_Write;
extern CAVIFile_Close g_pAVIFile_Close;
extern CAVIFile_Eof g_pAVIFile_Eof;

//CAVIFile functions

static int WriteSample( CAVISTREAM_HANDLE hCAVIStream,char *Data,int Len,
						unsigned int Flags)
{
	unsigned int i;
	unsigned char AlignByte = 0;
	XOS_U16 NullByteData = 0;
	char  SubChunkFlag[SUBCHUNKFLAG_LEN];
	XOS_U32 SubChunkLen;
	char *pSubChunkData;
	CAVIFILE_HANDLE hCAVIFile;
	hCAVIFile = hCAVIStream->hCAVIFile;

	//如果是音频帧并且PCM编码，则不可变
	if (  (hCAVIStream->aviStreamHeader.fccType == streamtypeAUDIO) 
		&&(hCAVIStream->aviStreamHeader.fccHandler == 0)) {
		if (  (hCAVIStream->SampleLen != 0)
			&&(hCAVIStream->SampleLen != (unsigned int)Len)) {
			return -1;
		}
	}

	if (hCAVIFile->AVIIndexNum >= hCAVIFile->MaxAVIIndexNum) {
		//如果AVIIndex用完了，返回错误
		return EC_CAVISTREAM_WRITESAMPLE_NOVALIDINDEX;
	}
//	hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].bUsed = 1;

	if (hCAVIStream->aviStreamHeader.fccType == streamtypeVIDEO){
		memcpy(SubChunkFlag,AVIFILETAG_00dc,SUBCHUNKFLAG_LEN);	
//		hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].Type = AVIINDEX_TYPE_00dc;
		memcpy(&(hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].ckid),AVIIndexTypes[AVIINDEX_TYPE_00dc],SUBCHUNKFLAG_LEN);
	}
	else if (hCAVIStream->aviStreamHeader.fccType == streamtypeAUDIO) {
		memcpy(SubChunkFlag,AVIFILETAG_01wb,SUBCHUNKFLAG_LEN);	
//		hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].Type = AVIINDEX_TYPE_01wb;
		memcpy(&(hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].ckid),AVIIndexTypes[AVIINDEX_TYPE_01wb],SUBCHUNKFLAG_LEN);
	}
	else {
		return -1;
	}
	
	//写SubChunkFlag
	hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].dwChunkOffset = hCAVIFile->WritePos - hCAVIFile->LISTmoviLengthPos - sizeof(XOS_U32);
	g_pAVIFile_Write(hCAVIFile->hFile,SubChunkFlag,SUBCHUNKFLAG_LEN);
	hCAVIFile->WritePos += SUBCHUNKFLAG_LEN;

	//写SubChunkLen
	SubChunkLen = Len;
	hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].dwChunkLength = SubChunkLen;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&SubChunkLen,sizeof(SubChunkLen));
	hCAVIFile->WritePos += sizeof(SubChunkLen);

	//写SubChunkData
	if (Data == 0) {
		pSubChunkData = (char*)&NullByteData;
		for (i=0;i<SubChunkLen/sizeof(NullByteData);i++){
			g_pAVIFile_Write(hCAVIFile->hFile,pSubChunkData,sizeof(NullByteData));
			hCAVIFile->WritePos += sizeof(NullByteData);
		}
	}
	else{
		pSubChunkData = Data;
		g_pAVIFile_Write(hCAVIFile->hFile,pSubChunkData,SubChunkLen);
		hCAVIFile->WritePos += SubChunkLen;
	}
	if((SubChunkLen % 2) == 1){
		//增加对齐字节
		g_pAVIFile_Write(hCAVIFile->hFile,(char *)&AlignByte,1);
		hCAVIFile->WritePos += 1;
	}

	//记下SubChunkFlags
//	hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].Flags = Flags;
	hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].dwFlags = AVIIndexFlags[Flags];
	
	hCAVIFile->AVIIndexNum ++;
	hCAVIStream->SampleNum ++;

	//初始化音频帧的长度，如果是PCM的则不可变
	if (  (hCAVIStream->aviStreamHeader.fccType == streamtypeAUDIO) 
		&&(hCAVIStream->aviStreamHeader.fccHandler == 0)) {
		if (hCAVIStream->SampleLen == 0){
			hCAVIStream->SampleLen = Len;
		}
	}

	return 0;
}

static int WritemoviHeader(CAVIFILE_HANDLE hCAVIFile)
{
	XOS_U32 Length;

	hCAVIFile->bWrittenSample = 1;
	
	//写hdrl LIST长度，最终的
	Length = hCAVIFile->WritePos - hCAVIFile->LISThdrlLengthPos - sizeof(XOS_U32);
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->LISThdrlLengthPos,XOSFILE_SEEK_ORIGIN_BEGIN);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);

	//写movi List Chunk
	//写LIST
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_LIST,strlen(AVIFILETAG_LIST));
	hCAVIFile->WritePos += strlen(AVIFILETAG_LIST);
	
	hCAVIFile->LISTmoviLengthPos = hCAVIFile->WritePos;
	//写movi LIST长度，临时的，关闭文件的时候需要重新写
	Length = 0;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	//写movie
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_movi,strlen(AVIFILETAG_movi));
	hCAVIFile->WritePos += strlen(AVIFILETAG_movi);

	return 0;
}

/************************************************************************/
/* 外部接口                                                             */
/************************************************************************/

//初始化CAVIFile模块
int AVIFileWrite_Init(XOS_HANDLE *pHandle,int MaxIndexNum,int MaxStreamNum)
{
	unsigned int i,j;
	CAVIFILE_HANDLE hCAVIFile;
	*pHandle = 0;
	hCAVIFile = (CAVIFILE_HANDLE)malloc(sizeof(TCAVIFile));
	if (hCAVIFile == 0){
		return -1;
	}
	memset(hCAVIFile,0,sizeof(TCAVIFile));
	
	assert(g_pAVIFile_Open != NULL && g_pAVIFile_Seek!= NULL &&
		   g_pAVIFile_Read != NULL && g_pAVIFile_Write != NULL &&
		   g_pAVIFile_Close != NULL && g_pAVIFile_Eof != NULL);

	hCAVIFile->MaxAVIIndexNum = MaxIndexNum;
//	hCAVIFile->AVIIndexs = (TAVIIndex *)malloc(sizeof(TAVIIndex)*hCAVIFile->MaxAVIIndexNum);
	hCAVIFile->AVIIndexs = (AVIINDEXENTRY *)malloc(sizeof(AVIINDEXENTRY)*hCAVIFile->MaxAVIIndexNum);
	if (hCAVIFile->AVIIndexs == 0){
		free(hCAVIFile);
		return -1;
	}
	memset(hCAVIFile->AVIIndexs,0,sizeof(AVIINDEXENTRY)*hCAVIFile->MaxAVIIndexNum);

	hCAVIFile->MaxCAVIStreamNum = MaxStreamNum;
	hCAVIFile->hCAVIStreams = (CAVISTREAM_HANDLE *)malloc(sizeof(CAVISTREAM_HANDLE)*hCAVIFile->MaxCAVIStreamNum);
	if (hCAVIFile->hCAVIStreams == 0){
		free(hCAVIFile->AVIIndexs);
		free(hCAVIFile);
		return -1;
	}
	memset(hCAVIFile->hCAVIStreams,0,sizeof(sizeof(XOS_HANDLE)*hCAVIFile->MaxCAVIStreamNum));
	
	for (i=0;i<hCAVIFile->MaxCAVIStreamNum;i++){
		hCAVIFile->hCAVIStreams[i] = (CAVISTREAM_HANDLE)malloc(sizeof(TCAVIStream));
		if (hCAVIFile->hCAVIStreams[i] == 0){
			for (j=0;j<i;j++){
				free(hCAVIFile->hCAVIStreams[j]);
			}
			free(hCAVIFile->hCAVIStreams);
			free(hCAVIFile->AVIIndexs);
			free(hCAVIFile);
			return -1;
		}
		memset(hCAVIFile->hCAVIStreams[i],0,sizeof(TCAVIStream));
	}
	

	*pHandle = hCAVIFile;
	return 0;
}

//新建一个AVI文件
int AVIFileWrite_CreateFile(XOS_HANDLE Handle,char * FileName)
{
	XOS_U32 Length = 0;
	CAVIFILE_HANDLE hCAVIFile;

	hCAVIFile = (CAVIFILE_HANDLE)Handle;

	if (hCAVIFile->bOpen == 1){
		return -1;
	}
	
	TCS_PUSH("g_pAVIFile_Open");
	hCAVIFile->hFile = g_pAVIFile_Open(FileName, XOS_FALSE);
	if (hCAVIFile->hFile == NULL)
	{
		TCS_POP;
		return -1;
	}
	TCS_POP;
	
	TCS_PUSH("g_pAVIFile_Seek");
	g_pAVIFile_Seek(hCAVIFile->hFile,0,XOSFILE_SEEK_ORIGIN_BEGIN);
	hCAVIFile->WritePos = 0;
	TCS_POP;
	
	TCS_PUSH("g_pAVIFile_Write");
	//写RIFF
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_RIFF,strlen(AVIFILETAG_RIFF));
	hCAVIFile->WritePos += strlen(AVIFILETAG_RIFF);
	
	hCAVIFile->RIFFLengthPos = hCAVIFile->WritePos;
	//写RIFF长度，临时的，关闭文件的时候需要重新写
	Length = 0;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	//写AVI
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_AVI,strlen(AVIFILETAG_AVI));
	hCAVIFile->WritePos += strlen(AVIFILETAG_AVI);

	//写hdrl List Chunk
	//写LIST
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_LIST,strlen(AVIFILETAG_LIST));
	hCAVIFile->WritePos += strlen(AVIFILETAG_LIST);
	
	hCAVIFile->LISThdrlLengthPos = hCAVIFile->WritePos;
	//写hdrl LIST长度，临时的，创建完所有的流的时候需要重新写
	Length = 0;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	//写hdrl
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_hdrl,strlen(AVIFILETAG_hdrl));
	hCAVIFile->WritePos += strlen(AVIFILETAG_hdrl);
	
	//写AVI File MainHeader
	//写avih
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_avih,strlen(AVIFILETAG_avih));
	hCAVIFile->WritePos += strlen(AVIFILETAG_avih);
	
	//写avih长度
	Length = sizeof(MainAVIHeader);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	hCAVIFile->MainAVIHeaderPos = hCAVIFile->WritePos;
	//写MainAVIHeader，临时的，关闭文件的时候需要重新写
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&hCAVIFile->mainAVIHeader,sizeof(hCAVIFile->mainAVIHeader));
	hCAVIFile->WritePos += sizeof(hCAVIFile->mainAVIHeader);
	
	TCS_POP;
	
	hCAVIFile->bOpen = 1;

	return 0;
}

//为新建的AVI文件新建一个流，
//必须在创建好所有的流之后才能够写数据，
//并且开始写数据之后就不能再创建流
int AVIFileWrite_CreateStream(XOS_HANDLE Handle,XOS_HANDLE *phStream,TCAVIStreamInfo *pAVIStreamInfo,
								void *pAVIStreamFormat,int AVIStreamFormatLen)
{
	unsigned int i;
	XOS_U32 Length;
	CAVISTREAM_HANDLE hCAVIStream;
	CAVIFILE_HANDLE hCAVIFile;
	SFBitmapInfoHeader *pSFBITMAPINFOHEADER;
	*phStream = 0;
	hCAVIFile = (CAVIFILE_HANDLE)Handle;
	
	for (i=0;i<hCAVIFile->MaxCAVIStreamNum;i++){
		if (hCAVIFile->hCAVIStreams[i]->bUsed != 0){
			continue;
		}
		hCAVIStream = hCAVIFile->hCAVIStreams[i];
		break;
	}
	
	memset(hCAVIStream,0,sizeof(TCAVIStream));
	hCAVIStream->bUsed = 1;
	hCAVIStream->hCAVIFile = hCAVIFile;
	hCAVIStream->aviStreamHeader.fccType = pAVIStreamInfo->fccType;
	hCAVIStream->aviStreamHeader.fccHandler = pAVIStreamInfo->fccHandler;
	hCAVIStream->aviStreamHeader.u32Rate = pAVIStreamInfo->dwRate;

	//写strl List Chunk
	//写LIST
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_LIST,strlen(AVIFILETAG_LIST));
	hCAVIFile->WritePos += strlen(AVIFILETAG_LIST);
	
	hCAVIFile->LISTstrlLengthPos = hCAVIFile->WritePos;
	//写strl LIST长度，临时的，创建这个完流的时候需要重新写
	Length = 0;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	//写strl
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_strl,strlen(AVIFILETAG_strl));
	hCAVIFile->WritePos += strlen(AVIFILETAG_strl);

	//写Stream header
	//写strh
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_strh,strlen(AVIFILETAG_strh));
	hCAVIFile->WritePos += strlen(AVIFILETAG_strh);

	//写strh长度
	Length = sizeof(AVIStreamHeader);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	hCAVIStream->AVIStreamHeaderPos = hCAVIFile->WritePos;
	//写AVIStreamHeader，临时的，关闭文件的时候需要重新写
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&hCAVIStream->aviStreamHeader,sizeof(hCAVIStream->aviStreamHeader));
	hCAVIFile->WritePos += sizeof(hCAVIFile->mainAVIHeader);

	//写Stream format
	//写strf
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_strf,strlen(AVIFILETAG_strf));
	hCAVIFile->WritePos += strlen(AVIFILETAG_strf);

	//写strf长度
	Length = AVIStreamFormatLen;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	//写AVIStreamFormat
	if (hCAVIStream->aviStreamHeader.fccType == streamtypeVIDEO){
		pSFBITMAPINFOHEADER = (SFBitmapInfoHeader *)pAVIStreamFormat;
		pSFBITMAPINFOHEADER->biCompression =  hCAVIStream->aviStreamHeader.fccHandler; 
	}
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)pAVIStreamFormat,AVIStreamFormatLen);
	hCAVIFile->WritePos += AVIStreamFormatLen;
	
	//暂不支持strd
	//Stream name
	//写strn
	//g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_strn,strlen(AVIFILETAG_strn));
	//hCAVIFile->WritePos += strlen(AVIFILETAG_strn);

	//暂不支持strn
	//写strn长度
	//Length = strlen("abc") + 1;
	//Length = 0;
	//g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	//hCAVIFile->WritePos += sizeof(Length);

	//写StreamName
	//g_pAVIFile_Write(hCAVIFile->hFile,"abc",strlen("abc") + 1);
	//hCAVIFile->WritePos += strlen("abc") + 1;

	//写strl LIST长度，最终的
	Length = hCAVIFile->WritePos - hCAVIFile->LISTstrlLengthPos - sizeof(XOS_U32);
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->LISTstrlLengthPos,XOSFILE_SEEK_ORIGIN_BEGIN);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);


	//记下AVIStreamFormat
	memcpy(hCAVIStream->pAVIStreamFormat,pAVIStreamFormat,AVIStreamFormatLen);
	hCAVIStream->AVIStreamFormatLen = AVIStreamFormatLen;

	*phStream = hCAVIStream;

	return 0;
}

//向AVI流中写数据帧
int AVIFileWrite_WriteSample(XOS_HANDLE Handle,char *Data,int Len, int SampleNo,unsigned int Flags)
{
	int rv;
	//unsigned char AlignByte = 0;
	//XOS_U16 NullWordData = 0;
	CAVISTREAM_HANDLE hCAVIStream;
	CAVIFILE_HANDLE hCAVIFile;

	hCAVIStream = (CAVISTREAM_HANDLE)Handle;
	hCAVIFile = hCAVIStream->hCAVIFile;
	
	if (hCAVIFile->bWrittenSample == 0) {
		//如果没有写过帧
		
		WritemoviHeader(hCAVIFile);
		
		//以后就可以填写SubChunk了
	}

	if (SampleNo > hCAVIStream->SampleNum) {
		//需要插入空帧，然后插入数据
		//看是否有空帧数据
		if (hCAVIStream->bHasNullSample == 0){

			//如果没有则插入空帧数据，并且记下来
			if (hCAVIStream->aviStreamHeader.fccType == streamtypeVIDEO){
				rv = WriteSample(hCAVIStream,0,0,AVIINDEX_FLAG_NULL);
				if (rv != 0){
					return rv;
				}
			}
			else if (hCAVIStream->aviStreamHeader.fccType == streamtypeAUDIO) {
				//目前只能够存PCM编码的音频
				if (hCAVIStream->aviStreamHeader.fccHandler == 0){
					rv = WriteSample(hCAVIStream,Data,Len,AVIINDEX_FLAG_NULL);
					if (rv != 0){
						return rv;
					}
				}
				else{
					return -1;
				}
			}
			else {
				return -1;
			}

			//记下SubChunkFlags
//			hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].Flags = AVIINDEX_FLAG_NULL;
			hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].dwFlags = AVIIndexFlags[AVIINDEX_FLAG_NULL];
			
			//记下空帧的位置
			hCAVIStream->NullSampleIndexNo = hCAVIFile->AVIIndexNum - 1;

			hCAVIStream->bHasNullSample = 1;
	
		}
		
		//向Index中添加空帧数据，直到SampleNo = hCAVIStream->SampleNum
		while (hCAVIStream->SampleNum < SampleNo){

			if (hCAVIFile->AVIIndexNum >= hCAVIFile->MaxAVIIndexNum) {
				//如果AVIIndex用完了，返回错误
				return EC_CAVISTREAM_WRITESAMPLE_NOVALIDINDEX;
			}
//			hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum].bUsed = 1;
			
			memcpy( &(hCAVIFile->AVIIndexs[hCAVIFile->AVIIndexNum]),
					&(hCAVIFile->AVIIndexs[hCAVIStream->NullSampleIndexNo]),
					sizeof(AVIINDEXENTRY));
			hCAVIFile->AVIIndexNum ++;
			hCAVIStream->SampleNum ++;
		}
		
	}

	//插完空帧之后插入数据帧
	if (SampleNo == hCAVIStream->SampleNum) {
		//刚好，可以插入数据，同时在AVIIndex中增加项
		rv = WriteSample(hCAVIStream,Data,Len,Flags);
		if (rv != 0) {
			return rv;
		}
	}
	else {
		//不能够改前面已经插入的帧，返回错误
		return EC_CAVISTREAM_WRITESAMPLE_SAMPLENO;
	}

	return 0;
}

//关闭AVI文件
int AVIFileWrite_CloseFile(XOS_HANDLE Handle)
{
	int iwriteindextime;
	unsigned int i;
	unsigned int uiAVIIndexPerTime;
	XOS_U32 dwTotalFrames = 0,dwStreams = 0;
    XOS_U32 dwWidth = 0;
    XOS_U32 dwHeight = 0;
	XOS_U32 Length;
//	AVIINDEXENTRY aviIndexEntry;
	CAVISTREAM_HANDLE hCAVIStream;
	CAVIFILE_HANDLE hCAVIFile;
	SFBitmapInfoHeader *pSFBITMAPINFOHEADER;
	SFWaveFormatEX *pSFWAVEFORMATEX;
	hCAVIFile = (CAVIFILE_HANDLE)Handle;
	
	if (hCAVIFile->bWrittenSample == 0) {
		//如果没有写过帧,则填写一个movi头
		WritemoviHeader(hCAVIFile);
	}

	//如果发现一帧都没有的话，就将各个流插入一个空帧
	for (i=0;i<hCAVIFile->MaxCAVIStreamNum;i++){
		if (hCAVIFile->hCAVIStreams[i]->bUsed == 0){
			break;
		}

		hCAVIStream = hCAVIFile->hCAVIStreams[i];

		if (hCAVIStream->SampleNum == 0){
			if (hCAVIStream->aviStreamHeader.fccType == streamtypeVIDEO){
				WriteSample(hCAVIStream,0,2,AVIINDEX_FLAG_NULL);
			}
			else if (hCAVIStream->aviStreamHeader.fccType == streamtypeAUDIO) {
				WriteSample(hCAVIStream,0,2,AVIINDEX_FLAG_NULL);
			}
		}
	}
	
	//写movi LIST长度
	Length = hCAVIFile->WritePos - hCAVIFile->LISTmoviLengthPos - sizeof(XOS_U32);
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->LISTmoviLengthPos,XOSFILE_SEEK_ORIGIN_BEGIN);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);

	
	//写Index块
	//写idx1
	g_pAVIFile_Write(hCAVIFile->hFile,AVIFILETAG_idx1,strlen(AVIFILETAG_idx1));
	hCAVIFile->WritePos += strlen(AVIFILETAG_idx1);

	//写idx1长度
	Length = hCAVIFile->AVIIndexNum * Defined_AVIINDEXENTRY_Size;
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	hCAVIFile->WritePos += sizeof(Length);

	//写Index
	if (sizeof(AVIINDEXENTRY) != Defined_AVIINDEXENTRY_Size) {
		//如果Index的结构不对，返回
		g_pAVIFile_Close(hCAVIFile->hFile);
		return -1;
	}

	uiAVIIndexPerTime = hCAVIFile->AVIIndexNum/25;
	
	/*
	for (i=0;i<hCAVIFile->AVIIndexNum;i++)
	{
		memcpy(&(aviIndexEntry.ckid),AVIIndexTypes[hCAVIFile->AVIIndexs[i].Type],SUBCHUNKFLAG_LEN);
		aviIndexEntry.dwFlags = AVIIndexFlags[hCAVIFile->AVIIndexs[i].Flags];
		aviIndexEntry.dwChunkOffset = hCAVIFile->AVIIndexs[i].dwChunkOffset;
		aviIndexEntry.dwChunkLength = hCAVIFile->AVIIndexs[i].dwChunkLength;
		g_pAVIFile_Write(hCAVIFile->hFile,(char*)&aviIndexEntry,sizeof(aviIndexEntry));
		hCAVIFile->WritePos += sizeof(aviIndexEntry);

		//写的太猛了,该成2.5秒写完,平均每次休息100ms
		if (uiAVIIndexPerTime > 0)
		{
			if ((i%uiAVIIndexPerTime) == 0)
			{
				XOS_Sleep(100);
			}
		}
	}
	*/
	iwriteindextime = XOS_GetTickCount();
	for (i=0;i<hCAVIFile->AVIIndexNum;)
	{
		int iTemp;
		iTemp = 4096;
		if ((hCAVIFile->AVIIndexNum-i) < 4096)
		{
			iTemp = hCAVIFile->AVIIndexNum-i;
		}
		g_pAVIFile_Write(hCAVIFile->hFile,(char*)&hCAVIFile->AVIIndexs[i],sizeof(AVIINDEXENTRY)*iTemp);
		hCAVIFile->WritePos += sizeof(AVIINDEXENTRY)*iTemp;
		i+=iTemp;
		XOS_Sleep(1);
	}
	printf("======================write index need time: %d\r\n", XOS_GetTickCount()-iwriteindextime);

	//写RIFF长度
	Length = hCAVIFile->WritePos - hCAVIFile->RIFFLengthPos - sizeof(XOS_U16);
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->RIFFLengthPos,XOSFILE_SEEK_ORIGIN_BEGIN);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&Length,sizeof(Length));
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);

	//写前面留下的头
	//为个流填写AVIStreamHeader
	for (i=0;i<hCAVIFile->MaxCAVIStreamNum;i++){
		if (hCAVIFile->hCAVIStreams[i]->bUsed == 0){
			break;
		}

		hCAVIStream = hCAVIFile->hCAVIStreams[i];

		if (hCAVIStream->aviStreamHeader.fccType == streamtypeVIDEO){
			hCAVIStream->aviStreamHeader.u32Scale = 1;
			hCAVIStream->aviStreamHeader.u32Length = hCAVIStream->SampleNum;
			hCAVIStream->aviStreamHeader.u32SuggestedBufferSize =  352*288*3;
			hCAVIStream->aviStreamHeader.u32Quality = 0;
			hCAVIStream->aviStreamHeader.u32SampleSize = 0;
			/*
			hCAVIStream->aviStreamHeader.dwReserved2 = 288;
			hCAVIStream->aviStreamHeader.dwReserved3 = 352*2;
			hCAVIStream->aviStreamHeader.rcFrame.top = 0;
			hCAVIStream->aviStreamHeader.rcFrame.left = 0;
			hCAVIStream->aviStreamHeader.rcFrame.bottom = 288;
			hCAVIStream->aviStreamHeader.rcFrame.right = 352*2;
			*/

			dwTotalFrames += hCAVIStream->SampleNum;
			dwStreams ++;
			pSFBITMAPINFOHEADER = (SFBitmapInfoHeader *)hCAVIStream->pAVIStreamFormat;
			dwWidth = pSFBITMAPINFOHEADER->biWidth;
			dwHeight = pSFBITMAPINFOHEADER->biHeight;
			g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIStream->AVIStreamHeaderPos,XOSFILE_SEEK_ORIGIN_BEGIN);
			g_pAVIFile_Write(hCAVIFile->hFile,(char*)&hCAVIStream->aviStreamHeader,sizeof(hCAVIStream->aviStreamHeader));
			g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);
		}
		else if (hCAVIStream->aviStreamHeader.fccType == streamtypeAUDIO) {
			hCAVIStream->aviStreamHeader.u32Scale = 1;
			hCAVIStream->aviStreamHeader.u32Length = hCAVIStream->SampleNum;
			hCAVIStream->aviStreamHeader.u32SuggestedBufferSize = 8000;
			hCAVIStream->aviStreamHeader.u32Quality = 0;

			//目前只能够存PCM编码的音频
			if (hCAVIStream->aviStreamHeader.fccHandler == 0){
				pSFWAVEFORMATEX = (SFWaveFormatEX *)hCAVIStream->pAVIStreamFormat;				
				hCAVIStream->aviStreamHeader.u32SampleSize = hCAVIStream->SampleLen / (pSFWAVEFORMATEX->nBlockAlign);
			}
			else{
				g_pAVIFile_Close(hCAVIFile->hFile);
				return -1;
			}

			dwTotalFrames += hCAVIStream->SampleNum;
			dwStreams ++;

			g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIStream->AVIStreamHeaderPos,XOSFILE_SEEK_ORIGIN_BEGIN);
			g_pAVIFile_Write(hCAVIFile->hFile,(char*)&hCAVIStream->aviStreamHeader,sizeof(hCAVIStream->aviStreamHeader));
			g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);
		}
		else {
			g_pAVIFile_Close(hCAVIFile->hFile);
			return -1;
		}

		continue;
	}
	//写MainAVIHeader
	hCAVIFile->mainAVIHeader.dwMicroSecPerFrame = 50000;
	hCAVIFile->mainAVIHeader.dwMaxBytesPerSec = 105672;
	hCAVIFile->mainAVIHeader.dwFlags = 0x00000810;
	hCAVIFile->mainAVIHeader.dwTotalFrames = dwTotalFrames;
	hCAVIFile->mainAVIHeader.dwStreams = dwStreams;
	hCAVIFile->mainAVIHeader.dwSuggestedBufferSize = 400*1024;
	hCAVIFile->mainAVIHeader.dwWidth = dwWidth;
	hCAVIFile->mainAVIHeader.dwHeight = dwHeight;
	hCAVIFile->mainAVIHeader.dwReserved2		 = dwHeight;
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->MainAVIHeaderPos,XOSFILE_SEEK_ORIGIN_BEGIN);
	g_pAVIFile_Write(hCAVIFile->hFile,(char*)&hCAVIFile->mainAVIHeader,sizeof(hCAVIFile->mainAVIHeader));
	g_pAVIFile_Seek(hCAVIFile->hFile,hCAVIFile->WritePos,XOSFILE_SEEK_ORIGIN_BEGIN);

	//关闭文件
	g_pAVIFile_Close(hCAVIFile->hFile);

	//清空内存
	hCAVIFile->bOpen = 0;
	hCAVIFile->bWrittenSample = 0;
	hCAVIFile->hFile = 0;
	for(i=0;i<hCAVIFile->MaxCAVIStreamNum;i++){
		memset(hCAVIFile->hCAVIStreams[i],0,sizeof(TCAVIStream));
	}
	hCAVIFile->WritePos = 0;
	hCAVIFile->RIFFLengthPos = 0;
	hCAVIFile->LISThdrlLengthPos = 0;
	hCAVIFile->LISTstrlLengthPos = 0;
	hCAVIFile->LISTmoviLengthPos = 0;
	hCAVIFile->MainAVIHeaderPos = 0;
	memset(&hCAVIFile->mainAVIHeader,0,sizeof(MainAVIHeader));
	memset(hCAVIFile->AVIIndexs,0,sizeof(sizeof(AVIINDEXENTRY)*hCAVIFile->MaxAVIIndexNum));
	hCAVIFile->AVIIndexNum = 0;

	return 0;
}

//关闭CAVIFile
int AVIFileWrite_Close(XOS_HANDLE Handle)
{
	unsigned int i;
	CAVIFILE_HANDLE hCAVIFile;
	hCAVIFile = (CAVIFILE_HANDLE)Handle;

	for (i=0;i<hCAVIFile->MaxCAVIStreamNum;i++){
		free(hCAVIFile->hCAVIStreams[i]);
	}
	free(hCAVIFile->hCAVIStreams);
	free(hCAVIFile->AVIIndexs);

	free(hCAVIFile);
	return 0;

}

/**************************************************************************************
*
*	AVI File Structure
*
**************************************************************************************/
/*
The AVI file is a RIFF file that consists of an AVI RIFF chunk with a registered FOURCC 
of AVI.The AVI RIFF chunk includes two mandatory list chunks with a FOURCC of hdrl 
and movi. The AVI RIFF chunk can also include an additional index chunk with a FOURCC 
idx1.The hdrl chunk is a header chunk that defines the format of the included data. 
The movi chunk contains the data, and the idx1 is an optional index to the data. 
Specifics concerning the hdrl, strl, movi, and idx1 list chunks follow.
The expanded form of the AVI file is illustrated in the RIFF syntax that follows: 


RIFF( 'AVI' LIST ( 'hdrl' 
		   'avih' ( Main AVI Header ) 
		   LIST ( 'strl'
		          'strh' ( Stream 1 Header ) 					
		          'strf' ( Stream 1 Format ) 					
		         ['strd' ( Stream 1 optional codec data ) ]
		        )
	 	   LIST ( 'strl'
		          'strh' ( Stream 2 Header ) 					
		          'strf' ( Stream 2 Format ) 					
		        [ 'strd' ( Stream 2 optional codec data )]
	 	        )
		   ...
 	        )
	LIST ( 'movi' 
		{ 
			'##dc' ( compressed DIB )
			| 
			LIST ( 'rec '
				'##dc' ( compressed DIB ) 
				'##db' ( uncompressed DIB ) 
				'##wb' ( audio data ) 
				'##pc' ( palette change ) 
				...
		 	)
	 	} 
		... 
	)
	[ 'idx1' ( AVI Index ) ]
)




hdrl List Chunk

The hdrl list chunk contains the avih chunk (the main AVI header) and 
a strl list chunk for each data stream contained in the file. 
The avih chunk provides general information about the file, including: 


Number of streams in the file (dwStreams) 
Width of the video image (dwWidth) 
Height of the video image (dwHeight) 
Frame duration (dwMicroSecPerFrame) 
Number of frames in the file (dwTotalFrames) 
Whether the file has an index (AVIF_HASINDEX bit of dwFlags). 



strl List Chunk

The stream list (strl) chunk contains information about the streams in the file. 
A strl list chunk contains a stream header (strh) chunk, a stream format (strf) chunk, 
and an optional stream data (strd) chunk. 
The strh includes general information about the stream data, such as whether 
it is audio or video (fccType set to auds or vids) 
and an indication of the maximum stream data chunk size (dwSuggestedBufferSize). 
The strf contains specific information about the stream data. 
This chunk is not a fixed size, and the data in it maps to different structure 
definitions depending on the data type and data compression. 


movi List Chunk

The movi list chunk contains the stream data in subchunks. 
The stream data subchunks can exist directly in the movi chunk 
or can be grouped into rec chunks. 
Grouping the subchunks into rec chunks is a means of defining the optimal read size 
for input/output (I/O) performance. The stream data should be read in rec chunks. 
The stream data subchunks have 4-character codes that identify the data 
as a compressed DIB (##dc), uncompressed DIB (##db), or audio data (##wb). 


idx1 Chunk

The idx1 (optional) chunk contains successions of AVIINDEXENTRY structures, 
one for each rec and stream data subchunk in the movi chunk. 
The AVINDEXENTRY structures identify each data chunk and its location in the file. 


AVI Data Structures

typedef struct {
    unsigned long dwMicroSecPerFrame;// 1/24 * 1000000
    unsigned long dwMaxBytesPerSec;
    unsigned long dwReserved1;
    unsigned long dwFlags;
    unsigned long dwTotalFrames;
    unsigned long dwInitialFrames;
    unsigned long dwStreams;
    unsigned long dwSuggestedBufferSize;
    unsigned long dwWidth;
    unsigned long dwHeight;
    unsigned long dwScale;
    unsigned long dwRate;
    unsigned long dwStart;
    unsigned long dwLength;
} MainAVIHeader;

#define AVIF_HASINDEX		0x00000010
#define AVIF_MUSTUSEINDEX	0x00000020
#define AVIF_ISINTERLEAVED	0x00000100
#define AVIF_WASCAPTUREFILE	0x00010000
#define AVIF_COPYRIGHTED	0x00020000

#define AVIF_KNOWN_FLAGS	0x00030130

#define Defined_MainAVIHeader_Size      (14*4)

typedef struct {
    unsigned long fccType;
    unsigned long fccHandler;
    unsigned long dwFlags;
    unsigned long dwReserved1;
    unsigned long dwInitialFrames;
    unsigned long dwScale;
    unsigned long dwRate;
    unsigned long dwStart;
    unsigned long dwLength;
    unsigned long dwSuggestedBufferSize;
    unsigned long dwQuality;
    unsigned long dwSampleSize;
    unsigned long dwReserved2;
    unsigned long dwReserved3;
} AVIStreamHeader;

#define AVISF_DISABLED		0x00000001
#define AVISF_VIDEO_PALCHANGES	0x00010000

#define AVISF_KNOWN_FLAGS	0x00010001

#define Defined_AVIStreamHeader_Size_old  (12*4)
#define Defined_AVIStreamHeader_Size      (14*4)

typedef struct {
    unsigned char bFirstEntry;
    unsigned char bNumEntries;
    unsigned short wFlags;
} AVIPALCHANGE;

typedef struct {
    unsigned long ckid;
    unsigned long dwFlags;
    unsigned long dwChunkOffset;
    unsigned long dwChunkLength;
} AVIINDEXENTRY;

#define AVIIF_LIST		0x00000001
#define AVIIF_KEYFRAME	0x00000010
#define AVIIF_FIRSTPART	0x00000020
#define AVIIF_LASTPART	0x00000040
#define AVIIF_MIDPART	(AVIIF_LASTPART | AVIFF_FIRSTPART)
#define AVIIF_NOTIME	0x00000100
#define AVIIF_COMPUSE	0x0fff0000

#define AVIIF_KNOWN_FLAGS 0x0fff0171

#define Defined_AVIINDEXENTRY_Size	(4*4)

*/
