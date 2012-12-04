//
//  AudioEncoder.h
//  NDMLib_Device
//
//  Created by  on 12-6-21.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#define HIS_ADPCM_SAMPLE_NUM	80			//一个ADPCM帧的采样点数
#define HIS_ADPCM_FRM_LEN		48			//一个ADPCM帧的字节数

typedef enum
{
    ADPCM,
    AMR,
}ENCODERID;

typedef struct
{
    char *pszData;
    unsigned int uiLen;
    ENCODERID encoderID;
}AudioDataParam;

HANDLE EncoderCreat(ENCODERID encoderID);

void EncoderDelete(HANDLE hEncoder);

int Encode(HANDLE hEncoder, short *pPCM, int SampleNum, unsigned char *pStream, int *pMaxStreamLen);

