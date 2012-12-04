//
//  AudioEncoder.cpp
//  NDMLib_Device
//
//  Created by  on 12-6-21.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//


#include "DataType.h"
#include "his_adpcm.h"
#include "AudioEncoder.h"

HANDLE EncoderCreat(ENCODERID encoderID)
{
    HANDLE hEncoder = NULL;

    if (encoderID == ADPCM)
    {
        HIS_ADPCM_Create(&hEncoder);
    }
    else
    {
        printf("The coder id is not support");
    }
    
    return hEncoder;
}

void EncoderDelete(HANDLE hEncoder)
{
    HIS_ADPCM_Delete(hEncoder);
}

int Encode(HANDLE hEncoder, short *pPCM, int SampleNum, unsigned char *pStream, int *pMaxStreamLen)
{
    int nRet = HIS_ADPCM_Encode(hEncoder, pPCM, SampleNum, pStream, pMaxStreamLen);
    return nRet;
}
