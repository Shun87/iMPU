//
//  DataPipe.m
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "DataPipe.h"
#include "VideoEncoder.h"
#import "AudioCapture.h"
#import "GpsCapture.h"
#import "AudioPlay.h"

@interface DataPipe()
{
    Capturer *videoCapturer;
    x264_ecoder_handle *encoder;
    BOOL start;
}
@property (nonatomic, assign)BOOL start;
@property (nonatomic, retain)Capturer *videoCapturer;
@end

@implementation DataPipe
@synthesize displayView;
@synthesize videoCapturer;
@synthesize start;

- (id)init
{
    if (self  = [super init])
    {
        cmdPort = [[CmdPort alloc] init];
        capture = [[MediaDataCapture alloc] init];
        start = NO;
        portArray = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (NSInteger)creatCmdPort:(NSString *)ip 
                     port:(u_short)port 
                      psd:(NSString *)psd 
                   device:(Device *)device
{
    // CmdPort 完成平台的连接 上报和请求的处理
    return [cmdPort creat:ip port:port psd:psd device:device 
             block:^(NSString *optID, int index, NSString *type, 
              NSString *dcIp, u_short dcPort, NSString *dcToken)
            {
                // 主线程处理
                dispatch_async(dispatch_get_main_queue(), ^{
                    
                    // 分析得到的操作ID
                    if ([optID isEqualToString:CTL_COMMONRES_StartStream_PushMode])
                    {
                        if ([type isEqualToString:@"IV"])
                        {
                            capture = [[MediaDataCapture alloc] init];
                            capture.mediaType = MediaTypeIV;
                        }
                        else if ([type isEqualToString:@"GPS"])
                        {
                            capture = [[GpsCapture alloc] init];
                            capture.mediaType = MediaTypeGps;
                        }
                        else if ([type isEqualToString:@"IA"])
                        {
                            capture = [[AudioCapture alloc] init];
                            capture.mediaType = MediaTypeIA;
                        }
                    }
                    else if ([optID isEqualToString:CTL_OA_StartCall_PushMode])
                    {
                        // 播放声音
                        NSAssert([type isEqualToString:@"OA"], @"type is not OA");
                        
                        capture.mediaType = MediaTypeCall;
                        
                    }
                    else if ([optID isEqualToString:CTL_OA_StartTalk_PushMode])
                    {
                        // 对讲
                        NSAssert([type isEqualToString:@"OA"], @"type is not OA");
                        capture = [[AudioCapture alloc] init];
                        
                        capture.mediaType = MediaTypeTalk;
                    }
                    
                    // 创建采集端口
                    [capture creatWithIp:dcIp port:dcPort token:dcToken];
                    
                    if (!self.start)
                    {
                        [self startCapture];
                        self.start = YES;
                    }
                });
            }];
}

- (void)startCapture
{
    // 必须在主线程中
    if (capture.mediaType == MediaTypeIV)
    {
        encoder = x264_ecoder_init();
        glView = [[GLView alloc] initWithFrame:self.displayView.bounds];
        [self.displayView addSubview:glView];
        
        self.videoCapturer = [[[VideoCapturer alloc] init] autorelease];
        self.videoCapturer.delegate = self;
        [self.videoCapturer startCapture];
    }
    else if (capture.mediaType == MediaTypeTalk
             || capture.mediaType == MediaTypeIA)
    {
        AudioCapture *recorder = (AudioCapture *)capture;
        [recorder startRecord];
    }
}

- (void)closePorts
{
    
}

- (void)pixelBufferReadyForDisplay:(CVPixelBufferRef)pixelBuffer
{
    // 取到一帧的图像数据 先抽取Y.U.V三个分量

    [glView displayPixelBuffer:pixelBuffer];
    
    int nSize = 1280 * 720;
    uint8_t *picture_buf = malloc((nSize * 3) / 2); /* size for YUV 420 */
    uint8_t *pBase = picture_buf;
    memset(picture_buf, 0, (nSize * 3) / 2);

    uint8_t uBuffer[640*360];
    uint8_t vBuffer[640*360];
    CVReturn sucess = CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    if (sucess != kCVReturnSuccess)
    {
        return;
    }

    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    unsigned char *yData = (unsigned char *)malloc(width * height);
    assert(yData != NULL);
    memset(yData, 0, width*height);
    size_t byte = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    void *base = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    if (byte == width)
    {
        memcpy(yData, base, width*height);
        memcpy(picture_buf, base, width*height);
        picture_buf +=  width*height;
    }
    free(yData);

    width = width/2;
    height = height/2;
    base = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    byte = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    size_t uByte = byte/2;
    
    memset(vBuffer, 0, uByte*height);
    memset(uBuffer, 0, uByte*height);
    uint8_t *uTemp = uBuffer;
    uint8_t *vTemp = vBuffer;
    for (int i=0; i<height; i++)
    {
        for (int i=0; i<byte/2; i++)
        {
            *uTemp++ = *(unsigned char *)base++;
            *vTemp++ = *(unsigned char *)base++;
        }
    }
    
    memcpy(picture_buf, uBuffer, uByte*height);
    picture_buf += uByte*height;
    memcpy(picture_buf, vBuffer, uByte*height);

    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    if (encoder != NULL)
    {
        uint8_t encodeData[1024*150];
        int nLen = 0;
        u_char keyFrame = 0;
        int nRet = x264_enocode(encoder, pBase, nSize * 3 / 2, encodeData, &nLen, &keyFrame);
        if (nRet < 0)
        {
            printf("x264_enocode error\r\n");
        }
        else
        {
            printf("x264_enocode sucess len = %d = keyFrame = %d\r\n", nLen, keyFrame);
        }
        
        [((VideoDataPort *)capture) sendData:encodeData len:nLen keyFrame:keyFrame];
    }
    
    free(pBase);
}

- (void)dealloc
{
    [capture release];
    [cmdPort release];
    [glView release];
    [videoCapturer release];
    [portArray release];
    [super dealloc];
}

@end
