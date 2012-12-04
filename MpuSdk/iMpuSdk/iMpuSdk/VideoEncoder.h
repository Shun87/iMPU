//
//  VideoEncoder.h
//  iMpuSdk
//
//  Created by  on 12-11-26.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#ifndef __VideoEncoder____H___
#define __VideoEncoder____H___

typedef void* x264_ecoder_handle;

x264_ecoder_handle x264_ecoder_init();
int x264_enocode(x264_ecoder_handle handle, unsigned char *pYUVData,
                 unsigned int length, unsigned char *outData, int *nLen, unsigned char *keyFrame);

#endif
