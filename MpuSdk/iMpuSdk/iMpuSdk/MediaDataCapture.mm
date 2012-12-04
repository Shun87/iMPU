//
//  MediaDataPort.m
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "MediaDataCapture.h"

typedef struct
{
    StorageFrameHead head;
    u_short width;
    u_short height;
    u_short produceID;
    u_char frameRate;
    u_char reserved[9];
    u_char algID;
    u_char ctlFlg;
    u_short rsv;
    u_char privateData[8];
    u_char data[1024*400];
}VIDEO_DATA_HEAD;

@interface MediaDataCapture()
{
    NSString *ip;
    u_short port;
    NSString *token;
    
}
@property (nonatomic, copy)NSString *ip;
@property (nonatomic, assign)u_short port;
@property (nonatomic, copy)NSString *token;
@end

@implementation MediaDataCapture
@synthesize ip, port, token;
@synthesize mediaType, dc7;

- (void)dealloc
{
    dispatch_release(sendDataQueue);
    [ip release];
    [token release];
    [super dealloc];
}

- (void)setMediaType:(MediaType)aMediaType
{
    mediaType = aMediaType;
}

- (void)creatWithIp:(NSString *)aIP port:(u_short)aPort token:(NSString *)aToken
{
    self.token = aToken;
    self.ip = aIP;
    self.port = aPort;
    sendDataQueue = dispatch_queue_create("DC7SendDataQueue", NULL);
    dc7 = DC7_Create([ip UTF8String], port, [token UTF8String], NULL, NULL, NULL);
}
@end

@implementation VideoDataPort

- (void)sendData:(unsigned char *)pData len:(int)len keyFrame:(u_char)keyFrame
{
    //dispatch_async(sendDataQueue, ^{
    
    VIDEO_DATA_HEAD video;
    memset(&video, 0, sizeof(VIDEO_DATA_HEAD));
    time_t time = (time_t)[[NSDate date] timeIntervalSince1970];
    video.head.uiUTC = time / 1000;
    video.head.uiTimeStamp = time;
    video.head.ucKeyFrm = keyFrame;
    video.width = 640;
    video.height = 480;
    video.produceID = 1;
    video.frameRate = 25;
    video.algID = 6;
    memset(video.data, 0, 1024*400);
    memcpy(video.data, pData, len);
    DC7_SendRawFrame(dc7, (char *)&video, len+40, 1, 0);
}

@end






