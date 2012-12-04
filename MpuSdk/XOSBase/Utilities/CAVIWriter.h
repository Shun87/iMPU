	/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: CAVIWriter.h
 *  文件标识: 
 *  摘    要: 写AVI文件模块,与vfw接口写出的文件基本一致
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2009-11-6 10:42:54
 *  修正备注: 
 *		1.音频视频流创建的顺序无关
 *		2.可以在写入帧数据之后,再创建新的流
 *		3.支持双音轨
 *		4.支持仅仅存视频或者音频,而且支持仅仅存双音轨音频.
 *		5.支持为每个流取一个自定义的字符串名称
 *		6.支持写入私有数据
 */

#ifndef __CAVIWRITER_H__
#define __CAVIWRITER_H__

//错误码定义
#define CAVIWRITER_E_OK					0
#define CAVIWRITER_E_UNKNOWN			-1			//未定义错误
#define CAVIWRITER_E_NOVALIDINDEX		-2			//没有足够的Index
#define CAVIWRITER_E_SAMPLENO			-3			//样本号错误,不连续
#define CAVIWRITER_E_MEMORY				-4			//内存不足
#define CAVIWRITER_E_STATUS				-5			//状态错误
#define CAVIWRITER_E_FILE_OPEN			-6			//打开文件错误
#define CAVIWRITER_E_STREAMCOUNT		-7			//流数目错误,
													//不能够超过CAVIWRITER_MAX_STREAM_NUM
#define CAVIWRITER_E_SAMPLELEN			-8			//样本长度错误,音频帧长不可变
#define CAVIWRITER_E_FCCTYPE			-9			//错误的媒体类型
#define CAVIWRITER_E_OVERFLOW			-10			//空间不足

//支持的最大流数目
#define	CAVIWRITER_MAX_STREAM_NUM		4		
//支持的私有数据长度
#define	CAVIWRITER_PRIVATEDATA_LEN		1024

//媒体类型宏定义
#define CAVIWRITER_mmioFOURCC( ch0, ch1, ch2, ch3 ) \
	( (unsigned char)(ch0) | ( (unsigned char)(ch1) << 8 ) |	\
	( (unsigned char)(ch2) << 16 ) | ( (unsigned char)(ch3) << 24 ) )
#define CAVIWRITER_streamtypeVIDEO	CAVIWRITER_mmioFOURCC('v', 'i', 'd', 's')
#define CAVIWRITER_streamtypeAUDIO	CAVIWRITER_mmioFOURCC('a', 'u', 'd', 's')
#define CAVIWRITER_streamtypeMIDI	CAVIWRITER_mmioFOURCC('m', 'i', 'd', 's')
#define CAVIWRITER_streamtypeTEXT	CAVIWRITER_mmioFOURCC('t', 'x', 't', 's')

//句柄定义
typedef void* CAVIWRITER_HANDLE;
typedef void* CAVIWRITER_STREAMHANDLE;

//索引类型定义
#define CAVIWRITER_AVIINDEX_FLAG_NULL			0x00	//非I帧
#define CAVIWRITER_AVIINDEX_FLAG_KEYFRAME		0x10	//I帧

typedef struct _TCAVIWriterStreamInfo { 
	unsigned int fccType;			//媒体类型,比如streamtypeVIDEO
	unsigned int fccHandler;		//处理算法,比如DIVX,音频填0
	unsigned int dwRate;			//帧率,dwRate/dwScale就是实际的帧率,音频和视频都有效
    unsigned int dwScale;			//帧率的除因子,dwRate/dwScale就是实际的帧率
									//一般这个值取1,如果实际帧率出现小数的时候,可以变大,比如1000
	char  szName[64];				//流的名字,多音轨的时候可以用来区分音轨
} TCAVIWriterStreamInfo; 

typedef struct _TAVIWriterBMPInfoHeader {
	unsigned int   biSize;			//这个结构的大小,sizeof(TAVIWriterBMPInfoHeader)
	unsigned int   biWidth;			//图像的实际像素宽度
	unsigned int   biHeight;		//图像的实际像素高度
	unsigned short biPlanes;		//调色板个数,一般为1
	unsigned short biBitCount;		//颜色位数,一般为24
	unsigned int   biCompression;	//压缩算法,跟TCAVIWriterStreamInfo中的fccHandler一样,可不填
	unsigned int   biSizeImage;		//图像的大小,等于biWidth*biHeight*biBitCount/8
	unsigned int   biXPelsPerMeter; //含义不明,填0
	unsigned int   biYPelsPerMeter;	//含义不明,填0
	unsigned int   biClrUsed;		//含义不明,填0
	unsigned int   biClrImportant;	//含义不明,填0	
} TAVIWriterBMPInfoHeader; 

typedef struct _TAVIWriterWaveFormatEx {
	unsigned short wFormatTag;		//音频的编码算法
	unsigned short nChannels;		//声道数,一般填1
	unsigned int   nSamplesPerSec;	//采样率,一般填8000
	unsigned int   nAvgBytesPerSec;	//平均字节率.填nBlockAlign*dwRate
	unsigned short nBlockAlign;		//每帧数据的长度
	unsigned short wBitsPerSample;	//采样位宽,一般填16
	unsigned short cbSize;			//扩展数据长度,填0
} TAVIWriterWaveFormatEx;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 初始化CAVIWriter模块
 *  参数说明:
 *      CAVIWRITER_HANDLE *phWriter [OUT]:		模块句柄的地址.用来返回初始化好的句柄
 *      int iMaxIndexNum [INT]:					支持的最大的Index的数目.
 *												一般大小为:(流0的帧率(f/s)+流1的帧率+..)*时间(s)
 *  返回值: 错误码,见前面的宏定义
 */
int CAVIWriter_Init(CAVIWRITER_HANDLE *phWriter, int iMaxIndexNum);

/*  功能描述: 关闭CAVIWriter模块
 *  参数说明:
 *      CAVIWRITER_HANDLE hWriter [IN]:			模块句柄
 */
void CAVIWriter_Close(CAVIWRITER_HANDLE hWriter);

/*  功能描述:新建一个AVI文件	
 *  参数说明:
 *      CAVIWRITER_HANDLE hWriter [IN]:			模块句柄
 *      const char *cpszFileName [IN]:			文件名
 *  返回值: 错误码,见前面的宏定义
 */
int CAVIWriter_CreateFile(CAVIWRITER_HANDLE hWriter, const char *cpszFileName);

/*  功能描述: 关闭AVI文件
 *  参数说明:
 *      CAVIWRITER_HANDLE hWriter [IN]:			模块句柄
 *  返回值: 错误码,见前面的宏定义	
 */
int CAVIWriter_CloseFile(CAVIWRITER_HANDLE hWriter);

/*  功能描述: 设置私有数据
 *  参数说明:
 *      CAVIWRITER_HANDLE hWriter [IN]:			模块句柄
 *      char binData[CAVIWRITER_PRIVATEDATA_LEN] [IN]:	私有数据缓冲,最多CAVIWRITER_PRIVATEDATA_LEN
 */
void CAVIWriter_SetPrivateData(CAVIWRITER_HANDLE hWriter, char binData[CAVIWRITER_PRIVATEDATA_LEN]);

/*  功能描述: 为新建的AVI文件新建一个流
 *  参数说明:
 *      CAVIWRITER_HANDLE hWriter [IN]:			模块句柄
 *      CAVIWRITER_STREAMHANDLE *hStream [OUT]:	创建的流句柄
 *      const TCAVIWriterStreamInfo *pInfo [IN]:流信息
 *      const void *pFormat [IN]:				创建流需要的格式的信息,根据不同的流为不同的结构,
 *												例如视频为TAVIWriterBMPInfoHeader
 *      int iFormatLen [IN]:					流的格式信息的长度
 *  返回值: 错误码,见前面的宏定义
 */
int CAVIWriter_CreateStream(CAVIWRITER_HANDLE hWriter, CAVIWRITER_STREAMHANDLE *phStream, 
							const TCAVIWriterStreamInfo *pInfo, const void *pFormat, int iFormatLen);

/*  功能描述: 向AVI流中写数据帧
 *  参数说明:
 *      CAVIWRITER_STREAMHANDLE hStream [IN]:	流句柄
 *      const char *pSample [IN]:				数据帧缓冲
 *      int iSampleLen [IN]:					数据帧长度
 *      int iSampleNo [IN]:						数据帧序号	
 *      unsigned int uiFlags [IN]:				数据帧的标志,比如是否为I帧等等,
 *												详见CAVIWRITER_AVIINDEX_FLAG_KEYFRAME等宏的定义
 *  返回值: 错误码,见前面的宏定义,特殊返回值以及处理如下:
 *				CAVIWRITER_E_NOVALIDINDEX	索引已经使用完,需要切换文件
 *				CAVIWRITER_E_SAMPLENO		帧序号不连续,需要上层确保序号连续
 */
int CAVIWriter_WriteSample(CAVIWRITER_STREAMHANDLE hStream, const char *pSample, 
						   int iSampleLen, int iSampleNo, unsigned int uiFlags);

#ifdef __cplusplus
}
#endif

#endif //__CAVIWRITER_H__
