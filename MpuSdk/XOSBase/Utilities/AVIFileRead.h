/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: AVIFileRead.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: 	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: jkt
 *  �������: 2007-6-21 18:27:22
 *  ������ע: 
			  tanhs
			  2009-7-15 16:27:00
			  ���·�װ�˺���
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
#define indextype00db		mmioFOURCC('0', '0', 'd', 'b')	//δѹ��
#define indextype00dc		mmioFOURCC('0', '0', 'd', 'c')	//ѹ����Ƶ
#define indextype02dc		mmioFOURCC('0', '2', 'd', 'c')	//ѹ����Ƶ
#define indextype01wb		mmioFOURCC('0', '1', 'w', 'b')	//��ƵindexID
#define indextype03wb		mmioFOURCC('0', '3', 'w', 'b')	//��ƵindexID
#endif

#ifndef encodertypeNULL
#define encodertypeNULL		mmioFOURCC('\0', '\0', '\0', '\0')
#define encodertypeDIVX		mmioFOURCC('D', 'I', 'V', 'X')
#define encodertypeH264		mmioFOURCC('H', '2', '6', '4')
#define encodertypeS264		mmioFOURCC('S', '2', '6', '4')
#define encodertypeF264		mmioFOURCC('F', '2', '6', '4')
#define encodertypeC264		mmioFOURCC('C', '2', '6', '4')
#endif

//����֡������
#define AVIFILEREAD_FINDVIDEO		1	//��������Ƶ
#define AVIFILEREAD_FINDAUDIO		2	//��������Ƶ
#define AVIFILEREAD_FINDALL			3	//����������Ƶ

//////////////////////////////////////////////////////////////////////////
//�ṹ�嶨��
typedef struct CHUNK
{
	XOS_HANDLE	pFHandle;
	char	szType[8];		// ������(RIFF,LIST)
	XOS_U32	u32Size;		// ���С
	XOS_U32	u32Position;	// �������ļ��е�λ�ã������ݣ�	
	char 	szName[8];		// ������(AVI_)	
}CHUNK;

//AVI�ļ�����Ҫ�����Ϣhdrl movi idx1
typedef struct _AVIMacroFormat_
{
	CHUNK	stFileChunk;		//��ʾ�ļ���chunk
	CHUNK	stHdrlChunk;		//hdrl ��������AVI�ļ��и������ĸ�ʽ��Ϣ
	CHUNK	stMoviChunk;		//ý�����ݿ�
	CHUNK	stIndexChunk;		//������
} AVIMacroFormat, *pAVIMacroFormat;

typedef struct _RIFF_FindData_t_
{
	CHUNK	RootChunk;			// ��ʾ�������Ľṹ�в���
	CHUNK	LastFindChunk;		// ���һ�β��ҵ����Ǹ���
} RIFF_FindData_t;

//���ڼ�¼AVI�ļ���ȫ����Ϣ(avih��)
typedef struct _AVIMainHeader_
{
    //XOS_U32 u32Fcc;					//����Ϊ'avih'
    //XOS_U32 u32CB;					//�����ݽṹ�Ĵ�С,�����������8���ֽ�(fcc��cb������)
	XOS_U32 u32MicroSecPerFrame;		//��Ƶ֡���ʱ��(�Ժ���Ϊ��λ)
	XOS_U32 u32MaxBytesPerSec;			//���AVI�ļ������������
	XOS_U32 u32PaddingGranularity;		//������������,��¼��ĳ�����Ϊ��ֵ�ı���,ͨ����2048
	XOS_U32 u32Flages;					//AVI�ļ���ȫ�ֱ��,�����Ƿ����������,
										//AVI�ļ�����������,���Ƿ����������,����Ƶ�����Ƿ񽻲�洢
	XOS_U32 u32TotalFrame;				//�ļ��е�������
	XOS_U32 u32InitialFrames;			//Ϊ������ʽָ����ʼ֡��(�ǽ�����ʽӦ��ָ��Ϊ0),˵���ڿ�ʼ����ǰ��Ҫ������
	XOS_U32 u32Streams;					//���ļ����������ĸ���
	XOS_U32 u32SuggestedBufferSize;		//����ʹ�õĻ������Ĵ�С(Ӧ���������Ŀ�)
										//ͨ��Ϊ�洢һ��ͼ���Լ�ͬ����������Ҫ������֮��
	XOS_U32 u32Width;					//ͼ���
	XOS_U32 u32Height;					//ͼ���
	XOS_U32 u32Reserved[4];				//����ֵ
}AVIMainHeader, *pAVIMainHeader;

//˵���������ͷ��Ϣstrh������ͷ
typedef struct _AVIStreamHeader
{
	//XOS_U32 u32Fcc;				//����Ϊ'strh' 
	//XOS_U32 u32CB;				//�����ݽṹ�Ĵ�С,�����������8���ֽ�(fcc��cb������)
	XOS_U32	fccType;				//4�ֽ�,��ʾ������������ 'vids'��ʾ��Ƶ������,'auds'��Ƶ������
									//'mids'(MIDI��)��'txts'(������)
	XOS_U32	fccHandler;				//4�ֽ�,ָ�����Ĵ�����,��������Ƶ��˵���ǽ�����
	XOS_U32	u32Flags;				//����������
	XOS_U16	u16Priority;			//���������Ĳ������ȼ�
	XOS_U16	u16Language;			//��Ƶ�����Դ���
	XOS_U32	u32InitalFrames;		//Ϊ������ʽָ����ʼ֡��,˵���ڿ�ʼ����ǰ��Ҫ֡��
	XOS_U32	u32Scale;				//�����ʹ�õ�ʱ��߶�,��Ƶÿ֡�Ĵ�С������Ƶ�Ĳ�����С
	XOS_U32	u32Rate;				//u32Scale/u32Rate = ÿ��Ĳ�����
	XOS_U32	u32Start;				//��������ʼ���ŵ�λ�ã���u32ScaleΪ��λ(���Ŀ�ʼʱ��)
	XOS_U32	u32Length;				//���ĳ���,��u32ScaleΪ��λ
	XOS_U32	u32SuggestedBufferSize;	//��ȡ��������ݽ���ʹ�õĻ����С
	XOS_U32	u32Quality;				//�����ݵ�����ָ��(0~10,000)��ѹ������������ֵԽ������Խ��
	XOS_U32	u32SampleSize;			//��Ƶ�Ĳ�����С
	struct
	{
		XOS_U16 left;
		XOS_U16 right;
		XOS_U16 top;
		XOS_U16 bottom;
	}Rect;							//ָ�������(��Ƶ����������)����Ƶ�������е���ʾλ��
									//��Ƶ��������AVIMAINHEADER�ṹ�е�dwWidth��dwHeight����
}AVIStreamHeader, *pAVIStreamHeader;

typedef struct _MyAVIStream_
{ 
	AVIStreamHeader stAVIStreamHeader;	//"strh"��Ϣ
	CHUNK	stStrlChunk;	//"strl"��
	CHUNK	stMoviChunk;	//"movi"��
	CHUNK	stIndexChunk;	//"index"��
	char    *pIndexDataBuffer;		//ȫ�������Ļ�����
	
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
//ע���ڲ���AVI�ļ�ʱ�����ȵ�������ĺ���ָ���ļ���������,
//����Ƕ�AVI�ļ�,ָ��XOSFile.h�ĺ���,�����FAT32¼��ָ��CFS.h��ĺ���
//����ʹ�ü�Test/AVIReadTest.c
//�ļ���������ָ��
void AVIFile_RegisterOpen(CAVIFile_Open pAVIFileOpen);
void AVIFile_RegisterSeek(CAVIFile_Seek pAVIFileSeek);
void AVIFile_RegisterRead(CAVIFile_Read pAVIFileRead);
void AVIFile_RegisterWrite(CAVIFile_Write pAVIFileWrite);
void AVIFile_RegisterEof(CAVIFile_Eof pAVIFileEof);
void AVIFile_RegisterClose(CAVIFile_Close pAVIFileClose);

//////////////////////////////////////////////////////////////////////////
//����AVI�ļ�����ӿ�
/*
 *	��������: �½�һ��AVI�ļ�
 *	����:
 *		pstAVIMacroFormat: ��������ȡ���ļ��Ľṹ.
 *		pszFileName: �ļ�·��
 */	
XOS_BOOL AVIFileRead_Open(pAVIMacroFormat pstAVIMacroFormat, XOS_CPSZ pszFileName);

/*
 *	��������: ��ȡavi�ļ��Ļ�����Ϣ('avih'��ͷ��Ϣ)
 *	����:
 *		pstAVIMacroFormat: ��������ȡ���ļ��Ľṹ.
 *		pstAVIMainHeader: ����,��������ļ���Ϣ
 */	
XOS_BOOL AVIFileRead_MainInfo(pAVIMacroFormat pstAVIMacroFormat, pAVIMainHeader pstAVIMainHeader);

/*
 *	��������: ��ȡavi�ļ�������Ϣ
 *	����:
 *		pstAVIMacroFormat:	��������ȡ���ļ��Ľṹ.
 *		pstAVIStream:		��ָ��
 *		*piTotalIndex:		��ȡ������ָ�������͵��������е����,��0��ʼ����
 */	
XOS_BOOL AVIFileRead_StreamInfo(pAVIMacroFormat pstAVIMacroFormat, pMyAVIStream pstAVIStream, int *piTotalIndex);

/*
 *	��������:	��ȡý��֡����
 *	����:
 *		pstAVIStream [OUT]:			��������ȡ���ļ��Ľṹ.
 *		*piIndexNum [INT OUT]:		֡���,��0��ʼ����,С��*piTotalIndex,����ʵ�ʶ�����֡��
 *		*pFrameBuffer [OUT]:		֡����
 *		*pulLen [INT OUT]:			ת��Ϊ֡Buffer��С,ת��Ϊʵ�ʶ�����֡����
 *		*piFlags [OUT]: 0:			��ʾ��I֡ 1:��ʾI֡ -1:����
 *      *piFindFrameType [IN OUT]:	��֡������,�鿴����궨��,����ʵ�ʶ�����֡����
 *		XOS_BOOL bFindIFrame [IN]:	0:������֡ 1:��������ƵI֡(ע��:��ʱ*piFindFrameType����Ϊ��Ƶ)
 *  ����ֵ: -1��ʾ���� 0:���� 1:�����ļ���β
 */	
int AVIFileRead_Frame(pMyAVIStream pstAVIStream, unsigned long *pulIndexNum, char *pFrameBuffer, 
					  unsigned long *pulLen, XOS_BOOL *pbIFrame, int *piFindFrameType, XOS_BOOL bFindIFrame);

/*
 *	��������: �ر�������Դ���ͷŷ�����ڴ�.
 *	����:
 *		pstAVIMacroFormat:	��������ȡ���ļ��Ľṹ.
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
