//
//  DataType.h
//  iMpuSdk
//
//  Created by  on 12-12-3.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#ifndef iMpuSdk_DataType_h
#define iMpuSdk_DataType_h

// 一些数据类型的定义
#include <vector>

typedef void * HANDLE;
typedef unsigned char  UCHAR;

enum IDCMFrameType
{
	FRAME_UNKNOWN = 0x00,							// 
	FRAME_VIDEO = 0x01,								// 
	FRAME_AUDIO	= 0x02,								// 
	DFRAME_DATA = 0x03,								// 
	FRAME_GPS = 0x04,								// GPS
};

#endif
