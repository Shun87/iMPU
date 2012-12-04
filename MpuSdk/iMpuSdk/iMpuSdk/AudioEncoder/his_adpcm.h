#ifndef _HIS_ADPCM_H_
#define _HIS_ADPCM_H_

#define HIS_ADPCM_OK			0
#define HIS_ADPCM_E_UNKNOWN		-1
#define HIS_ADPCM_E_HANDLE		-2
#define HIS_ADPCM_E_SAMPLE_NUM	-3
#define HIS_ADPCM_E_STREAM_FMT	-4	//码流格式错误
#define HIS_ADPCM_E_BUF_LEN		-5	//缓冲区长度太短

#define HIS_ADPCM_SAMPLE_NUM	80			//一个ADPCM帧的采样点数
#define HIS_ADPCM_FRM_LEN		48			//一个ADPCM帧的字节数

typedef void* HIS_ADPCM_HANDLE;

int HIS_ADPCM_Create(HIS_ADPCM_HANDLE *phHIS);
void HIS_ADPCM_Delete(HIS_ADPCM_HANDLE hHIS);

//pPCM输入PCM数据。
//SampleNum是采样点的个数
//pStream是码流输出缓冲区.码流从HIS的私有头开始。
//*pMaxStreamLen传入是pStream的长度,要足够大(至少大于HIS_ADPCM_FRM_LEN), 输出是码流长度,应该就是HIS_ADPCM_FRM_LEN
//返回值HIS_ADPCM_OK是成功,负值是失败
int HIS_ADPCM_Encode(HIS_ADPCM_HANDLE hHIS, short *pPCM, int SampleNum, unsigned char *pStream, int *pMaxStreamLen);

//pPCM输出PCM数据。
//pMaxSampleNums输入是pPCM可以容纳的采样点的个数，要足够大（至少大于HIS_ADPCM_SAMPLE_NUM），输出是解码后采样点的个数，应该就是HIS_ADPCM_SAMPLE_NUM
//pStream是码流输入缓冲区.码流从HIS的私有头开始。
//StreamLen是码流长度,应该就是HIS_ADPCM_FRM_LEN
//返回值HIS_ADPCM_OK是成功,负值是失败
int HIS_ADPCM_Decode(HIS_ADPCM_HANDLE hHIS, short *pPCM, int *pMaxSampleNum, unsigned char *pStream, int StreamLen);

#endif