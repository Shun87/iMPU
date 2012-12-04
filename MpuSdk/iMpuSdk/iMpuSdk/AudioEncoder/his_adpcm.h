#ifndef _HIS_ADPCM_H_
#define _HIS_ADPCM_H_

#define HIS_ADPCM_OK			0
#define HIS_ADPCM_E_UNKNOWN		-1
#define HIS_ADPCM_E_HANDLE		-2
#define HIS_ADPCM_E_SAMPLE_NUM	-3
#define HIS_ADPCM_E_STREAM_FMT	-4	//������ʽ����
#define HIS_ADPCM_E_BUF_LEN		-5	//����������̫��

#define HIS_ADPCM_SAMPLE_NUM	80			//һ��ADPCM֡�Ĳ�������
#define HIS_ADPCM_FRM_LEN		48			//һ��ADPCM֡���ֽ���

typedef void* HIS_ADPCM_HANDLE;

int HIS_ADPCM_Create(HIS_ADPCM_HANDLE *phHIS);
void HIS_ADPCM_Delete(HIS_ADPCM_HANDLE hHIS);

//pPCM����PCM���ݡ�
//SampleNum�ǲ�����ĸ���
//pStream���������������.������HIS��˽��ͷ��ʼ��
//*pMaxStreamLen������pStream�ĳ���,Ҫ�㹻��(���ٴ���HIS_ADPCM_FRM_LEN), �������������,Ӧ�þ���HIS_ADPCM_FRM_LEN
//����ֵHIS_ADPCM_OK�ǳɹ�,��ֵ��ʧ��
int HIS_ADPCM_Encode(HIS_ADPCM_HANDLE hHIS, short *pPCM, int SampleNum, unsigned char *pStream, int *pMaxStreamLen);

//pPCM���PCM���ݡ�
//pMaxSampleNums������pPCM�������ɵĲ�����ĸ�����Ҫ�㹻�����ٴ���HIS_ADPCM_SAMPLE_NUM��������ǽ���������ĸ�����Ӧ�þ���HIS_ADPCM_SAMPLE_NUM
//pStream���������뻺����.������HIS��˽��ͷ��ʼ��
//StreamLen����������,Ӧ�þ���HIS_ADPCM_FRM_LEN
//����ֵHIS_ADPCM_OK�ǳɹ�,��ֵ��ʧ��
int HIS_ADPCM_Decode(HIS_ADPCM_HANDLE hHIS, short *pPCM, int *pMaxSampleNum, unsigned char *pStream, int StreamLen);

#endif