//
//  MediaDataPort.h
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DC7.h"

typedef struct
{
    u_int uiUTC;
    u_int uiTimeStamp;
    u_char ucKeyFrm;
    u_char ucRsv[3];    //12
}StorageFrameHead;

typedef enum 
{
    MediaTypeTalk,
    MediaTypeCall,
    MediaTypeIA,
    MediaTypeIV,
    MediaTypeGps,
}MediaType;

@interface MediaDataCapture : NSObject
{
    dispatch_queue_t sendDataQueue;
    
    DC7_HANDLE  dc7;
}
@property (nonatomic, assign)DC7_HANDLE  dc7;
@property (nonatomic, assign)MediaType mediaType;

- (void)setMediaType:(MediaType)aMediaType;
- (void)creatWithIp:(NSString *)aIP port:(u_short)aPort token:(NSString *)aToken;

@end

@interface VideoDataPort : MediaDataCapture {
@private
    u_char *plane[3];
}

- (void)sendData:(unsigned char *)pData len:(int)len keyFrame:(u_char)keyFrame;
@end




