/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: AVIFileWrite.h
 *  文件标识: 
 *  摘    要: 
 *  
 *  当前版本: 
 *  作    者: jkt
 *  完成日期: 2007-6-21 18:27:22
 *  修正备注: 2009-7-16 21:00:00
 *  
 */

#ifndef __AVIFILEWRITE_H__
#define __AVIFILEWRITE_H__

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

#define AVIINDEX_FLAG_NULL			0x00
#define AVIINDEX_FLAG_LIST			0x01
#define AVIINDEX_FLAG_KEYFRAME		0x02
#define AVIINDEX_FLAG_FIRSTPART		0x03
#define AVIINDEX_FLAG_LASTPART		0x04
#define AVIINDEX_FLAG_MIDPART		0x05
#define AVIINDEX_FLAG_NOTIME		0x06
#define AVIINDEX_FLAG_COMPUSE		0x07
#define AVIINDEX_KNOWN_FLAGS		0x08

typedef struct _TCAVIStreamInfo{ 
    unsigned int fccType;				//媒体类型,比如streamtypeVIDEO
    unsigned int fccHandler;			//处理算法,比如DIVX,0表示PCM.对音频,目前只支持PCM	
    unsigned int dwRate;				//帧率,视频就是实际帧率,对PCM帧率等于采样率
    char  szName[64];					//流的名字,目前不支持
}TCAVIStreamInfo; 

typedef struct _SFBitmapInfoHeader{
	unsigned int   biSize; 
	unsigned int   biWidth; 
	unsigned int   biHeight; 
	unsigned short biPlanes; 
	unsigned short biBitCount; 
	unsigned int   biCompression;			//压缩算法,跟TCAVIStreamInfo中的fccHandler一样,可以不填
	unsigned int   biSizeImage; 
	unsigned int   biXPelsPerMeter; 
	unsigned int   biYPelsPerMeter; 
	unsigned int   biClrUsed; 
	unsigned int   biClrImportant; 
} SFBitmapInfoHeader; 

typedef struct _SFWaveFormatEX{
	unsigned short wFormatTag;
	unsigned short nChannels;
	unsigned int   nSamplesPerSec;
	unsigned int   nAvgBytesPerSec;
	unsigned short nBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short cbSize;
} SFWaveFormatEX;
 
#define EC_CAVISTREAM_WRITESAMPLE_NOVALIDINDEX			0x00010001
#define EC_CAVISTREAM_WRITESAMPLE_SAMPLENO				0x00010002

#ifdef __cplusplus
extern "C"{
#endif

/*
 *	初始化AVIFileWrite模块	
 *	参数:
 *		pHandle:		CAVIFile模块的句柄的地址.用来返回初始化好的句柄
 *		MaxIndexNum:	CAVIFile支持的最大的Index的数目.
 *						一般大小为:(流0的帧率(f/s)+流1的帧率+..)*时间(s)
 *		MaxStreamNum:	CAVIFile支持的最大的流的数目
 *	返回值：	
 *		0:				成功
 *		其他:			错误代码
 */	
int AVIFileWrite_Init(XOS_HANDLE *pHandle, int MaxIndexNum, int MaxStreamNum);

/*
 *	新建一个AVI文件
 *	参数:
 *		Handle:			CAVIFile模块的句柄.
 *		FileName:		打开的AVI文件的名字.可以支持带目录的名字
 *	返回值：	
 *		0:				成功
 *		其他:			错误代码
 */	
int AVIFileWrite_CreateFile(XOS_HANDLE Handle,char * FileName);

/*
 *	为新建的AVI文件新建一个流
 *	参数:
 *		Handle:				CAVIFile模块的句柄..
 *		pStream:			AVI流的句柄的地址,用来返回流的句柄.
 *		pAVIStreamInfo:		创建流时需要的流的信息,TCAVIStreamInfo见前面的定义
 *		pAVIStreamFormat:	创建流需要的格式的信息,根据不同的流为不同的结构,
 *							例如视频为SFBITMAPINFOHEADER
 *		AVIStreamFormatLen:	流的格式信息的长度
 *	返回值：	
 *		0:				成功
 *		其他:			错误代码
 *	说明:
 *		必须在创建好所有的流之后才能够写数据,并且开始写数据之后就不能再创建流
 */	
int AVIFileWrite_CreateStream(XOS_HANDLE Handle, XOS_HANDLE *pStream, TCAVIStreamInfo *pAVIStreamInfo,
								void *pAVIStreamFormat, int AVIStreamFormatLen);

/*	向AVI流中写数据帧
 *	参数:
 *		Handle:				AVI流的句柄
 *		Data:				要写的数据
 *		Len:				数据的长度
 *		SampleNo:			数据帧的序号
 *		Flags:				数据帧的标志,比如是否为I帧等等,详见AVIINDEX_FLAG_KEYFRAME等宏的定义
 *	返回值：	
 *		0:				成功
 *		其他:			错误代码
 *	说明:
 *		SampleNo必须是递增的,但是可以跳过某些帧
 */	
int AVIFileWrite_WriteSample(XOS_HANDLE Handle,char *Data,int Len, int SampleNo, unsigned int Flags);

/*	关闭AVI文件
 *	参数:
 *		Handle:	AVI流的句柄
 *	返回值：	
 *		0:		成功
 *		其他:	错误代码
 */	
int AVIFileWrite_CloseFile(XOS_HANDLE Handle);

/*
 *	关闭CAVIFile
 *	参数:
 *		Handle:	CAVIFile模块的句柄
 *	返回值：	
 *		0:				成功
 *		其他:			错误代码
 */	
int AVIFileWrite_Close(XOS_HANDLE Handle);

#ifdef __cplusplus
}
#endif

#endif //__AVIFILEWRITE_H__
