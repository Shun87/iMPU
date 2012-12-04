//
//  VideoEncoder.c
//  iMpuSdk
//
//  Created by  on 12-11-26.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#include "VideoEncoder.h"
#include <stdint.h>
#include <inttypes.h>
#include "x264.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _x264_obj
{
    x264_picture_t pic;
    x264_param_t param;
    x264_t *pX264;
}x264_obj;

x264_ecoder_handle x264_ecoder_init()
{
    x264_obj *pObj = (x264_obj *)malloc(sizeof(x264_obj));
    x264_param_default(&pObj->param);
    x264_param_default_preset(&pObj->param, "superfast", "zerolatency");
    x264_param_apply_profile(&pObj->param, "baseline");
    pObj->param.b_cabac = 0;
    pObj->param.rc.i_rc_method = X264_RC_ABR;
    pObj->param.b_sliced_threads = 0;
    
    x264_picture_init(&pObj->pic);
    pObj->pic.img.i_csp = X264_CSP_I420;
    pObj->pic.img.i_plane = 3;
    
    pObj->param.i_width = 1280;
    pObj->param.i_height = 720;
    pObj->param.rc.i_bitrate = 1200; // 码率在往上就没效果了.
    pObj->param.i_fps_num = 25;
    pObj->param.i_keyint_min = 30;
    pObj->param.i_keyint_max = 30;
    
    pObj->pX264 = x264_encoder_open(&pObj->param);
    if (pObj->pX264 == 0)
    {
        printf("x264_encoder_open failed\r\n");
    }
    
    pObj->pic.img.i_stride[0] = 1280;
    pObj->pic.img.i_stride[1] = 640;
    pObj->pic.img.i_stride[2] = 640;
    
    return pObj;
}

int x264_enocode(x264_ecoder_handle handle, unsigned char *pYUVData,
                 unsigned int length, unsigned char *outData, int *nLen, unsigned char *keyFrame)
{
    x264_obj *pObj = (x264_obj *)handle;
    unsigned int yPlaneSize = length * 2 / 3;
    pObj->pic.img.plane[0] = pYUVData;
    pObj->pic.img.plane[1] = pYUVData + yPlaneSize;
    pObj->pic.img.plane[2] = pObj->pic.img.plane[1] + yPlaneSize / 4;
    
//    /* encode 1 second of video */
//    for(int i=0; i<25; i++) {
//   
//        fflush(stdout);
//        /* Y */
//        for(int y=0; y<pObj.param.i_height; y++) {
//            for(int x=0; x<pObj.param.i_width; x++) {
//                
//                pObj.pic.img.plane[0][y * 640 + x] = x + y + i * 3;
//            }
//        }
//        
//        /* Cb and Cr */
//        for(int y=0; y<pObj.param.i_height/2; y++) {
//            for(int x=0; x<pObj.param.i_width/2; x++) {
//                pObj.pic.img.plane[1][y * 320 + x] = 128 + y + i * 2;
//                pObj.pic.img.plane[2][y * 320 + x] = 64 + x + i * 5;
//            }
//        }
        
    x264_picture_t outPic;
    x264_nal_t *nal;
    int i_nal;
    int rv;
    pObj->pic.i_type = X264_TYPE_AUTO;
    
    rv = x264_encoder_encode(pObj->pX264, &nal, &i_nal, &pObj->pic, &outPic);
    if (rv <= 0)
    {
        *nLen = 0;
        return rv;
    }

    int nPos = 0;
    uint8_t *pTemp = outData;
    for (int i=0; i<i_nal; i++)
    {
        if (nal[i].i_type == NAL_SEI)
        {
            continue;
        }
        
        memcpy(pTemp+nPos, nal[i].p_payload, nal[i].i_payload);
        nPos += nal[i].i_payload;
    }
    
    if (outPic.b_keyframe)
    {
        // 判断是否是关键帧
        *keyFrame = outPic.b_keyframe;
    }
    *nLen = nPos;
    
    return rv;
}