/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: AVIFileRead.h
 *  文件标识: 
 *  摘    要: 	
 *  
 *  当前版本: 
 *  作    者: jkt
 *  完成日期: 2007-6-21 18:27:22
 *  修正备注: 
			  tanhs
			  2009-7-15 16:27:00
			  重新封装了函数
 *  
 */

#ifndef _AVIFILEREAD_H__
#define _AVIFILEREAD_H__

#ifndef mmioFOURCC
    #define mmioFOURCC( ch0, ch1, ch2, ch3 ) \
	( (unsigned char)(ch0) | ( (unsigned char)(ch1) << 8 ) |	\
	( (unsigned char)(ch2) << 16 ) | ( (unsigned char)(ch3) << 24 ) )
#endif

#ifndef streamtypeVIDEO
#define streamtypeVIDEO		mmioFOURCC('v', 'i', 'd', 's')
#define streamtypeAUDIO		mmioFOURCC('a', 'u', 'd', 's')
#define streamtypeMIDI		mmioFOURCC('m', 'i', 'd', 's')
#define streamtypeTEXT		mmioFOURCC('t', 'x', 't', 's')
#endif

#ifndef indextype00db
#define indextype00db		mmioFOURCC('0', '0', 'd', 'b')	//未压缩
#define indextype00dc		mmioFOURCC('0', '0', 'd', 'c')	//压缩视频
#define indextype02dc		mmioFOURCC('0', '2', 'd', 'c')	//压缩视频
#define indextype01wb		mmioFOURCC('0', '1', 'w', 'b')	//音频indexID
#define indextype03wb		mmioFOURCC('0', '3', 'w', 'b')	//音频indexID
#endif

#ifndef encodertypeNULL
#define encodertypeNULL		mmioFOURCC('\0', '\0', '\0', '\0')
#define encodertypeDIVX		mmioFOURCC('D', 'I', 'V', 'X')
#define encodertypeH264		mmioFOURCC('H', '2', '6', '4')
#define encodertypeS264		mmioFOURCC('S', '2', '6', '4')
#define encodertypeF264		mmioFOURCC('F', '2', '6', '4')
#define encodertypeC264		mmioFOURCC('C', '2', '6', '4')
#endif

//查找帧的类型
#define AVIFILEREAD_FINDVIDEO		1	//单独找视频
#define AVIFILEREAD_FINDAUDIO		2	//单独找音频
#define AVIFILEREAD_FINDALL			3	//单独找音视频

//////////////////////////////////////////////////////////////////////////
//结构体定义
typedef struct CHUNK
{
	XOS_HANDLE	pFHandle;
	char	szType[8];		// 块类型(RIFF,LIST)
	XOS_U32	u32Size;		// 块大小
	XOS_U32	u32Position;	// 块所在文件中的位置（块数据）	
	char 	szName[8];		// 块名字(AVI_)	
}CHUNK;

//AVI文件各主要块的信息hdrl movi idx1
typedef struct _AVIMacroFormat_
{
	CHUNK	stFileChunk;		//表示文件的chunk
	CHUNK	stHdrlChunk;		//hdrl 用于描述AVI文件中各个流的格式信息
	CHUNK	stMoviChunk;		//媒体数据块
	CHUNK	stIndexChunk;		//索引块
} AVIMacroFormat, *pAVIMacroFormat;

typedef struct _RIFF_FindData_t_
{
	CHUNK	RootChunk;			// 表示在这个块的结构中查找
	CHUNK	LastFindChunk;		// 最后一次查找到的那个块
} RIFF_FindData_t;

//用于记录AVI文件的全局信息(avih块)
typedef struct _AVIMainHeader_
{
    //XOS_U32 u32Fcc;					//必须为'avih'
    //XOS_U32 u32CB;					//本数据结构的大小,不包括最初的8个字节(fcc和cb两个域)
	XOS_U32 u32MicroSecPerFrame;		//视频帧间隔时间(以毫秒为单位)
	XOS_U32 u32MaxBytesPerSec;			//这个AVI文件的最大数据率
	XOS_U32 u32PaddingGranularity;		//数据填充的粒度,记录块的长度需为此值的倍数,通常是2048
	XOS_U32 u32Flages;					//AVI文件的全局标记,比如是否含有索引块等,
										//AVI文件的特殊属性,如是否包含索引块,音视频数据是否交叉存储
	XOS_U32 u32TotalFrame;				//文件中的总桢数
	XOS_U32 u32InitialFrames;			//为交互格式指定初始帧数(非交互格式应该指定为0),说明在开始播放前需要多少桢
	XOS_U32 u32Streams;					//本文件包含的流的个数
	XOS_U32 u32SuggestedBufferSize;		//建议使用的缓冲区的大小(应能容纳最大的块)
										//通常为存储一桢图像以及同步声音所需要的数据之和
	XOS_U32 u32Width;					//图像宽
	XOS_U32 u32Height;					//图像高
	XOS_U32 u32Reserved[4];				//保留值
}AVIMainHeader, *pAVIMainHeader;

//说明这个流的头信息strh数据流头
typedef struct _AVIStreamHeader
{
	//XOS_U32 u32Fcc;				//必须为'strh' 
	//XOS_U32 u32CB;				//本数据结构的大小,不包括最初的8个字节(fcc和cb两个域)
	XOS_U32	fccType;				//4字节,表示数据流的种类 'vids'表示视频数据流,'auds'音频数据流
									//'mids'(MIDI流)、'txts'(文字流)
	XOS_U32	fccHandler;				//4字节,指定流的处理者,对于音视频来说就是解码器
	XOS_U32	u32Flags;				//数据流属性
	XOS_U16	u16Priority;			//此数据流的播放优先级
	XOS_U16	u16Language;			//音频的语言代号
	XOS_U32	u32InitalFrames;		//为交互格式指定初始帧数,说明在开始播放前需要帧数
	XOS_U32	u32Scale;				//这个流使用的时间尺度,视频每帧的大小或者音频的采样大小
	XOS_U32	u32Rate;				//u32Scale/u32Rate = 每秒的采样数
	XOS_U32	u32Start;				//数据流开始播放的位置，以u32Scale为单位(流的开始时间)
	XOS_U32	u32Length;				//流的长度,以u32Scale为单位
	XOS_U32	u32SuggestedBufferSize;	//读取这个流数据建议使用的缓存大小
	XOS_U32	u32Quality;				//流数据的质量指标(0~10,000)解压缩质量参数，值越大，质量越好
	XOS_U32	u32SampleSize;			//音频的采样大小
	struct
	{
		XOS_U16 left;
		XOS_U16 right;
		XOS_U16 top;
		XOS_U16 bottom;
	}Rect;							//指定这个流(视频流或文字流)在视频主窗口中的显示位置
									//视频主窗口由AVIMAINHEADER结构中的dwWidth和dwHeight决定
}AVIStreamHeader, *pAVIStreamHeader;

typedef struct _MyAVIStream_
{ 
	AVIStreamHeader stAVIStreamHeader;	//"strh"信息
	CHUNK	stStrlChunk;	//"strl"块
	CHUNK	stMoviChunk;	//"movi"块
	CHUNK	stIndexChunk;	//"index"块
	char    *pIndexDataBuffer;		//全部索引的缓冲区
	
} MyAVIStream, *pMyAVIStream;

typedef XOS_HANDLE (*CAVIFile_Open)(XOS_CPSZ cpszFileName, XOS_BOOL bReadOnly);
typedef XOS_BOOL (*CAVIFile_Seek)(XOS_HANDLE hFile, int iOffset, int iOrigin);
typedef int (*CAVIFile_Write)(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen);
typedef int (*CAVIFile_Read)(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen);
typedef XOS_BOOL (*CAVIFile_Eof)(XOS_HANDLE hFile);
typedef void (*CAVIFile_Close)(XOS_HANDLE hFile);

#ifdef __cplusplus
extern "C"{
#endif

//////////////////////////////////////////////////////////////////////////
//注意在操作AVI文件时必须先调用下面的函数指定文件操作类型,
//如果是读AVI文件,指定XOSFile.h的函数,如果是FAT32录像指定CFS.h里的函数
//具体使用见Test/AVIReadTest.c
//文件操作函数指定
void AVIFile_RegisterOpen(CAVIFile_Open pAVIFileOpen);
void AVIFile_RegisterSeek(CAVIFile_Seek pAVIFileSeek);
void AVIFile_RegisterRead(CAVIFile_Read pAVIFileRead);
void AVIFile_RegisterWrite(CAVIFile_Write pAVIFileWrite);
void AVIFile_RegisterEof(CAVIFile_Eof pAVIFileEof);
void AVIFile_RegisterClose(CAVIFile_Close pAVIFileClose);

//////////////////////////////////////////////////////////////////////////
//操作AVI文件对外接口
/*
 *	功能描述: 新建一个AVI文件
 *	参数:
 *		pstAVIMacroFormat: 保存所读取的文件的结构.
 *		pszFileName: 文件路径
 */	
XOS_BOOL AVIFileRead_Open(pAVIMacroFormat pstAVIMacroFormat, XOS_CPSZ pszFileName);

/*
 *	功能描述: 读取avi文件的基本信息('avih'块头信息)
 *	参数:
 *		pstAVIMacroFormat: 保存所读取的文件的结构.
 *		pstAVIMainHeader: 缓存,用于填充文件信息
 */	
XOS_BOOL AVIFileRead_MainInfo(pAVIMacroFormat pstAVIMacroFormat, pAVIMainHeader pstAVIMainHeader);

/*
 *	功能描述: 获取avi文件的流信息
 *	参数:
 *		pstAVIMacroFormat:	保存所读取的文件的结构.
 *		pstAVIStream:		流指针
 *		*piTotalIndex:		获取的流在指定流类型的所有流中的序号,从0开始计数
 */	
XOS_BOOL AVIFileRead_StreamInfo(pAVIMacroFormat pstAVIMacroFormat, pMyAVIStream pstAVIStream, int *piTotalIndex);

/*
 *	功能描述:	获取媒体帧数据
 *	参数:
 *		pstAVIStream [OUT]:			保存所读取的文件的结构.
 *		*piIndexNum [INT OUT]:		帧编号,从0开始计数,小于*piTotalIndex,返回实际读到的帧号
 *		*pFrameBuffer [OUT]:		帧缓存
 *		*pulLen [INT OUT]:			转入为帧Buffer大小,转出为实际读到的帧长度
 *		*piFlags [OUT]: 0:			表示非I帧 1:表示I帧 -1:出错
 *      *piFindFrameType [IN OUT]:	找帧的类型,查看上面宏定义,返回实际读到的帧类型
 *		XOS_BOOL bFindIFrame [IN]:	0:找所有帧 1:仅仅找视频I帧(注意:此时*piFindFrameType必须为视频)
 *  返回值: -1表示出错 0:正常 1:到达文件结尾
 */	
int AVIFileRead_Frame(pMyAVIStream pstAVIStream, unsigned long *pulIndexNum, char *pFrameBuffer, 
					  unsigned long *pulLen, XOS_BOOL *pbIFrame, int *piFindFrameType, XOS_BOOL bFindIFrame);

/*
 *	功能描述: 关闭所有资源和释放分配的内存.
 *	参数:
 *		pstAVIMacroFormat:	保存所读取的文件的结构.
 */
void AVIFileRead_Release(pAVIMacroFormat pstAVIMacroFormat, pMyAVIStream pstAVIStream);

#ifdef __cplusplus
}
#endif

/* The following is a short description of the AVI file format.  Please
 * see the accompanying documentation for a full explanation.
 *
 * An AVI file is the following RIFF form:
 *
 *	RIFF('AVI' 
 *	      LIST('hdrl'
 *		    avih(<MainAVIHeader>)
 *                  LIST ('strl'
 *                      strh(<Stream header>)
 *                      strf(<Stream format>)
 *                      ... additional header data
 *            LIST('movi'	 
 *      	  { LIST('rec' 
 *      		      SubChunk...
 *      		   )
 *      	      | SubChunk } ....	    
 *            )
 *            [ <AVIIndex> ]
 *      )
 *
 *	The main file header specifies how many streams are present.  For
 *	each one, there must be a stream header chunk and a stream format
 *	chunk, enlosed in a 'strl' LIST chunk.  The 'strf' chunk contains
 *	type-specific format information; for a video stream, this should
 *	be a BITMAPINFO structure, including palette.  For an audio stream,
 *	this should be a WAVEFORMAT (or PCMWAVEFORMAT) structure.
 *
 *	The actual data is contained in subchunks within the 'movi' LIST 
 *	chunk.  The first two characters of each data chunk are the
 *	stream number with which that data is associated.
 *
 *	Some defined chunk types:
 *           Video Streams:
 *                  ##db:	RGB DIB bits
 *                  ##dc:	RLE8 compressed DIB bits
 *                  ##pc:	Palette Change
 *
 *           Audio Streams:
 *                  ##wb:	waveform audio bytes
 *
 * The grouping into LIST 'rec' chunks implies only that the contents of
 *   the chunk should be read into memory at the same time.  This
 *   grouping is used for files specifically intended to be played from 
 *   CD-ROM.
 *
 * The index chunk at the end of the file should contain one entry for 
 *   each data chunk in the file.
 *       
 * Limitations for the current software:
 *	Only one video stream and one audio stream are allowed.
 *	The streams must start at the beginning of the file.
 *
 * 
 * To register codec types please obtain a copy of the Multimedia
 * Developer Registration Kit from:
 *
 *  Microsoft Corporation
 *  Multimedia Systems Group
 *  Product Marketing
 *  One Microsoft Way
 *  Redmond, WA 98052-6399
 *
 */

#endif
