	/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: CAVIWriter.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: дAVI�ļ�ģ��,��vfw�ӿ�д�����ļ�����һ��
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2009-11-6 10:42:54
 *  ������ע: 
 *		1.��Ƶ��Ƶ��������˳���޹�
 *		2.������д��֡����֮��,�ٴ����µ���
 *		3.֧��˫����
 *		4.֧�ֽ�������Ƶ������Ƶ,����֧�ֽ�����˫������Ƶ.
 *		5.֧��Ϊÿ����ȡһ���Զ�����ַ�������
 *		6.֧��д��˽������
 */

#ifndef __CAVIWRITER_H__
#define __CAVIWRITER_H__

//�����붨��
#define CAVIWRITER_E_OK					0
#define CAVIWRITER_E_UNKNOWN			-1			//δ�������
#define CAVIWRITER_E_NOVALIDINDEX		-2			//û���㹻��Index
#define CAVIWRITER_E_SAMPLENO			-3			//�����Ŵ���,������
#define CAVIWRITER_E_MEMORY				-4			//�ڴ治��
#define CAVIWRITER_E_STATUS				-5			//״̬����
#define CAVIWRITER_E_FILE_OPEN			-6			//���ļ�����
#define CAVIWRITER_E_STREAMCOUNT		-7			//����Ŀ����,
													//���ܹ�����CAVIWRITER_MAX_STREAM_NUM
#define CAVIWRITER_E_SAMPLELEN			-8			//�������ȴ���,��Ƶ֡�����ɱ�
#define CAVIWRITER_E_FCCTYPE			-9			//�����ý������
#define CAVIWRITER_E_OVERFLOW			-10			//�ռ䲻��

//֧�ֵ��������Ŀ
#define	CAVIWRITER_MAX_STREAM_NUM		4		
//֧�ֵ�˽�����ݳ���
#define	CAVIWRITER_PRIVATEDATA_LEN		1024

//ý�����ͺ궨��
#define CAVIWRITER_mmioFOURCC( ch0, ch1, ch2, ch3 ) \
	( (unsigned char)(ch0) | ( (unsigned char)(ch1) << 8 ) |	\
	( (unsigned char)(ch2) << 16 ) | ( (unsigned char)(ch3) << 24 ) )
#define CAVIWRITER_streamtypeVIDEO	CAVIWRITER_mmioFOURCC('v', 'i', 'd', 's')
#define CAVIWRITER_streamtypeAUDIO	CAVIWRITER_mmioFOURCC('a', 'u', 'd', 's')
#define CAVIWRITER_streamtypeMIDI	CAVIWRITER_mmioFOURCC('m', 'i', 'd', 's')
#define CAVIWRITER_streamtypeTEXT	CAVIWRITER_mmioFOURCC('t', 'x', 't', 's')

//�������
typedef void* CAVIWRITER_HANDLE;
typedef void* CAVIWRITER_STREAMHANDLE;

//�������Ͷ���
#define CAVIWRITER_AVIINDEX_FLAG_NULL			0x00	//��I֡
#define CAVIWRITER_AVIINDEX_FLAG_KEYFRAME		0x10	//I֡

typedef struct _TCAVIWriterStreamInfo { 
	unsigned int fccType;			//ý������,����streamtypeVIDEO
	unsigned int fccHandler;		//�����㷨,����DIVX,��Ƶ��0
	unsigned int dwRate;			//֡��,dwRate/dwScale����ʵ�ʵ�֡��,��Ƶ����Ƶ����Ч
    unsigned int dwScale;			//֡�ʵĳ�����,dwRate/dwScale����ʵ�ʵ�֡��
									//һ�����ֵȡ1,���ʵ��֡�ʳ���С����ʱ��,���Ա��,����1000
	char  szName[64];				//��������,�������ʱ�����������������
} TCAVIWriterStreamInfo; 

typedef struct _TAVIWriterBMPInfoHeader {
	unsigned int   biSize;			//����ṹ�Ĵ�С,sizeof(TAVIWriterBMPInfoHeader)
	unsigned int   biWidth;			//ͼ���ʵ�����ؿ��
	unsigned int   biHeight;		//ͼ���ʵ�����ظ߶�
	unsigned short biPlanes;		//��ɫ�����,һ��Ϊ1
	unsigned short biBitCount;		//��ɫλ��,һ��Ϊ24
	unsigned int   biCompression;	//ѹ���㷨,��TCAVIWriterStreamInfo�е�fccHandlerһ��,�ɲ���
	unsigned int   biSizeImage;		//ͼ��Ĵ�С,����biWidth*biHeight*biBitCount/8
	unsigned int   biXPelsPerMeter; //���岻��,��0
	unsigned int   biYPelsPerMeter;	//���岻��,��0
	unsigned int   biClrUsed;		//���岻��,��0
	unsigned int   biClrImportant;	//���岻��,��0	
} TAVIWriterBMPInfoHeader; 

typedef struct _TAVIWriterWaveFormatEx {
	unsigned short wFormatTag;		//��Ƶ�ı����㷨
	unsigned short nChannels;		//������,һ����1
	unsigned int   nSamplesPerSec;	//������,һ����8000
	unsigned int   nAvgBytesPerSec;	//ƽ���ֽ���.��nBlockAlign*dwRate
	unsigned short nBlockAlign;		//ÿ֡���ݵĳ���
	unsigned short wBitsPerSample;	//����λ��,һ����16
	unsigned short cbSize;			//��չ���ݳ���,��0
} TAVIWriterWaveFormatEx;

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: ��ʼ��CAVIWriterģ��
 *  ����˵��:
 *      CAVIWRITER_HANDLE *phWriter [OUT]:		ģ�����ĵ�ַ.�������س�ʼ���õľ��
 *      int iMaxIndexNum [INT]:					֧�ֵ�����Index����Ŀ.
 *												һ���СΪ:(��0��֡��(f/s)+��1��֡��+..)*ʱ��(s)
 *  ����ֵ: ������,��ǰ��ĺ궨��
 */
int CAVIWriter_Init(CAVIWRITER_HANDLE *phWriter, int iMaxIndexNum);

/*  ��������: �ر�CAVIWriterģ��
 *  ����˵��:
 *      CAVIWRITER_HANDLE hWriter [IN]:			ģ����
 */
void CAVIWriter_Close(CAVIWRITER_HANDLE hWriter);

/*  ��������:�½�һ��AVI�ļ�	
 *  ����˵��:
 *      CAVIWRITER_HANDLE hWriter [IN]:			ģ����
 *      const char *cpszFileName [IN]:			�ļ���
 *  ����ֵ: ������,��ǰ��ĺ궨��
 */
int CAVIWriter_CreateFile(CAVIWRITER_HANDLE hWriter, const char *cpszFileName);

/*  ��������: �ر�AVI�ļ�
 *  ����˵��:
 *      CAVIWRITER_HANDLE hWriter [IN]:			ģ����
 *  ����ֵ: ������,��ǰ��ĺ궨��	
 */
int CAVIWriter_CloseFile(CAVIWRITER_HANDLE hWriter);

/*  ��������: ����˽������
 *  ����˵��:
 *      CAVIWRITER_HANDLE hWriter [IN]:			ģ����
 *      char binData[CAVIWRITER_PRIVATEDATA_LEN] [IN]:	˽�����ݻ���,���CAVIWRITER_PRIVATEDATA_LEN
 */
void CAVIWriter_SetPrivateData(CAVIWRITER_HANDLE hWriter, char binData[CAVIWRITER_PRIVATEDATA_LEN]);

/*  ��������: Ϊ�½���AVI�ļ��½�һ����
 *  ����˵��:
 *      CAVIWRITER_HANDLE hWriter [IN]:			ģ����
 *      CAVIWRITER_STREAMHANDLE *hStream [OUT]:	�����������
 *      const TCAVIWriterStreamInfo *pInfo [IN]:����Ϣ
 *      const void *pFormat [IN]:				��������Ҫ�ĸ�ʽ����Ϣ,���ݲ�ͬ����Ϊ��ͬ�Ľṹ,
 *												������ƵΪTAVIWriterBMPInfoHeader
 *      int iFormatLen [IN]:					���ĸ�ʽ��Ϣ�ĳ���
 *  ����ֵ: ������,��ǰ��ĺ궨��
 */
int CAVIWriter_CreateStream(CAVIWRITER_HANDLE hWriter, CAVIWRITER_STREAMHANDLE *phStream, 
							const TCAVIWriterStreamInfo *pInfo, const void *pFormat, int iFormatLen);

/*  ��������: ��AVI����д����֡
 *  ����˵��:
 *      CAVIWRITER_STREAMHANDLE hStream [IN]:	�����
 *      const char *pSample [IN]:				����֡����
 *      int iSampleLen [IN]:					����֡����
 *      int iSampleNo [IN]:						����֡���	
 *      unsigned int uiFlags [IN]:				����֡�ı�־,�����Ƿ�ΪI֡�ȵ�,
 *												���CAVIWRITER_AVIINDEX_FLAG_KEYFRAME�Ⱥ�Ķ���
 *  ����ֵ: ������,��ǰ��ĺ궨��,���ⷵ��ֵ�Լ���������:
 *				CAVIWRITER_E_NOVALIDINDEX	�����Ѿ�ʹ����,��Ҫ�л��ļ�
 *				CAVIWRITER_E_SAMPLENO		֡��Ų�����,��Ҫ�ϲ�ȷ���������
 */
int CAVIWriter_WriteSample(CAVIWRITER_STREAMHANDLE hStream, const char *pSample, 
						   int iSampleLen, int iSampleNo, unsigned int uiFlags);

#ifdef __cplusplus
}
#endif

#endif //__CAVIWRITER_H__
