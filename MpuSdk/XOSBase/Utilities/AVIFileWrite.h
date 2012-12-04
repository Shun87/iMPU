/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: AVIFileWrite.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: 
 *  
 *  ��ǰ�汾: 
 *  ��    ��: jkt
 *  �������: 2007-6-21 18:27:22
 *  ������ע: 2009-7-16 21:00:00
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
    unsigned int fccType;				//ý������,����streamtypeVIDEO
    unsigned int fccHandler;			//�����㷨,����DIVX,0��ʾPCM.����Ƶ,Ŀǰֻ֧��PCM	
    unsigned int dwRate;				//֡��,��Ƶ����ʵ��֡��,��PCM֡�ʵ��ڲ�����
    char  szName[64];					//��������,Ŀǰ��֧��
}TCAVIStreamInfo; 

typedef struct _SFBitmapInfoHeader{
	unsigned int   biSize; 
	unsigned int   biWidth; 
	unsigned int   biHeight; 
	unsigned short biPlanes; 
	unsigned short biBitCount; 
	unsigned int   biCompression;			//ѹ���㷨,��TCAVIStreamInfo�е�fccHandlerһ��,���Բ���
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
 *	��ʼ��AVIFileWriteģ��	
 *	����:
 *		pHandle:		CAVIFileģ��ľ���ĵ�ַ.�������س�ʼ���õľ��
 *		MaxIndexNum:	CAVIFile֧�ֵ�����Index����Ŀ.
 *						һ���СΪ:(��0��֡��(f/s)+��1��֡��+..)*ʱ��(s)
 *		MaxStreamNum:	CAVIFile֧�ֵ�����������Ŀ
 *	����ֵ��	
 *		0:				�ɹ�
 *		����:			�������
 */	
int AVIFileWrite_Init(XOS_HANDLE *pHandle, int MaxIndexNum, int MaxStreamNum);

/*
 *	�½�һ��AVI�ļ�
 *	����:
 *		Handle:			CAVIFileģ��ľ��.
 *		FileName:		�򿪵�AVI�ļ�������.����֧�ִ�Ŀ¼������
 *	����ֵ��	
 *		0:				�ɹ�
 *		����:			�������
 */	
int AVIFileWrite_CreateFile(XOS_HANDLE Handle,char * FileName);

/*
 *	Ϊ�½���AVI�ļ��½�һ����
 *	����:
 *		Handle:				CAVIFileģ��ľ��..
 *		pStream:			AVI���ľ���ĵ�ַ,�����������ľ��.
 *		pAVIStreamInfo:		������ʱ��Ҫ��������Ϣ,TCAVIStreamInfo��ǰ��Ķ���
 *		pAVIStreamFormat:	��������Ҫ�ĸ�ʽ����Ϣ,���ݲ�ͬ����Ϊ��ͬ�Ľṹ,
 *							������ƵΪSFBITMAPINFOHEADER
 *		AVIStreamFormatLen:	���ĸ�ʽ��Ϣ�ĳ���
 *	����ֵ��	
 *		0:				�ɹ�
 *		����:			�������
 *	˵��:
 *		�����ڴ��������е���֮����ܹ�д����,���ҿ�ʼд����֮��Ͳ����ٴ�����
 */	
int AVIFileWrite_CreateStream(XOS_HANDLE Handle, XOS_HANDLE *pStream, TCAVIStreamInfo *pAVIStreamInfo,
								void *pAVIStreamFormat, int AVIStreamFormatLen);

/*	��AVI����д����֡
 *	����:
 *		Handle:				AVI���ľ��
 *		Data:				Ҫд������
 *		Len:				���ݵĳ���
 *		SampleNo:			����֡�����
 *		Flags:				����֡�ı�־,�����Ƿ�ΪI֡�ȵ�,���AVIINDEX_FLAG_KEYFRAME�Ⱥ�Ķ���
 *	����ֵ��	
 *		0:				�ɹ�
 *		����:			�������
 *	˵��:
 *		SampleNo�����ǵ�����,���ǿ�������ĳЩ֡
 */	
int AVIFileWrite_WriteSample(XOS_HANDLE Handle,char *Data,int Len, int SampleNo, unsigned int Flags);

/*	�ر�AVI�ļ�
 *	����:
 *		Handle:	AVI���ľ��
 *	����ֵ��	
 *		0:		�ɹ�
 *		����:	�������
 */	
int AVIFileWrite_CloseFile(XOS_HANDLE Handle);

/*
 *	�ر�CAVIFile
 *	����:
 *		Handle:	CAVIFileģ��ľ��
 *	����ֵ��	
 *		0:				�ɹ�
 *		����:			�������
 */	
int AVIFileWrite_Close(XOS_HANDLE Handle);

#ifdef __cplusplus
}
#endif

#endif //__AVIFILEWRITE_H__
