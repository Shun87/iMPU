//
//  ViewController.m
//  PreviewController
//
//  Created by  on 12-3-23.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "ViewController.h"

//#include <stdint.h>
//#include <inttypes.h>
//#include "x264.h"
#import "iMpuSdk.h"

@implementation ViewController
@synthesize backView;

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle
NSString *filePath()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(
														 NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *nsFilePath = [documentsDirectory stringByAppendingPathComponent:@"test.dat"];
    return nsFilePath;
}

//typedef struct _x264_obj
//{
//    x264_picture_t pic;
//    x264_param_t param;
//    x264_t *pX264;
//}x264_obj;
//
//static x264_obj pObj;
//static unsigned long long GetTickCount64()
//{
//	struct timeval tv;
//	gettimeofday(&tv, 0);
//	return (unsigned long long)(tv.tv_sec*1000+tv.tv_usec/1000);
//}
//
//static unsigned long GetTickCount()
//{
//#ifdef _USE_TIMES_
//	// 还是计算CPU占用的时间，同boost::timer
//	struct tms tm;
//	return (unsigned long)times(&tm);
//#else	
//	unsigned long long ullTick = GetTickCount64();
//	return (unsigned long)(ullTick & 0xFFFFFFFF);
//#endif // _USE_TIMES_
//}

//static void x264_enocode()
//{
//    FILE *f;
//    const char *path = [filePath() UTF8String];
//    f = fopen(path, "wb");
//    if (!f)
//    {
//        printf("open file error\r\n");
//    }
//    
//    x264_param_default(&pObj.param);
//    x264_param_default_preset(&pObj.param, "superfast", "zerolatency");
//    x264_param_apply_profile(&pObj.param, "baseline");
//    pObj.param.b_cabac = 0;
//    pObj.param.rc.i_rc_method = X264_RC_ABR;
//    pObj.param.b_sliced_threads = 0;
//    
//    x264_picture_init(&pObj.pic);
//    pObj.pic.img.i_csp = X264_CSP_I420;
//    pObj.pic.img.i_plane = 3;
//    
//    pObj.param.i_width = 640;
//    pObj.param.i_height = 480;
//    pObj.param.rc.i_bitrate = 100;
//    pObj.param.i_fps_num = 25;
//    pObj.param.i_keyint_min = 30;
//    pObj.param.i_keyint_max = 30;
//
//    pObj.pX264 = x264_encoder_open(&pObj.param);
//    if (pObj.pX264 == NULL)
//    {
//        printf("x264_encoder_open failed\r\n");
//    }
//    
//    pObj.pic.img.i_stride[0] = 640;
//    pObj.pic.img.i_stride[1] = 320;
//    pObj.pic.img.i_stride[2] = 320;
//
//    int nSize = 640 * 480;
//    uint8_t *picture_buf = malloc((nSize * 3) / 2); /* size for YUV 420 */
//    pObj.pic.img.plane[0] = picture_buf;
//    pObj.pic.img.plane[1] = pObj.pic.img.plane[0] + nSize;
//    pObj.pic.img.plane[2] = pObj.pic.img.plane[1] + nSize / 4;
//    
//    
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
//        
//        x264_picture_t outPic;
//        x264_nal_t *nal;
//        int i_nal;
//        int rv;
//        pObj.pic.i_type = X264_TYPE_AUTO;
//        
//        rv = x264_encoder_encode(pObj.pX264, &nal, &i_nal, &pObj.pic, &outPic);
//        if (rv <= 0)
//        {
//            printf("encoder error\r\n");
//        }
//        
//        int nPos = 0;
//        uint8_t *outData = malloc(1024 * 1024);
//        uint8_t *pTemp = outData;
//        memset(outData, 0, 1024 * 1024);
//        for (int i=0; i<i_nal; i++)
//        {
//            if (nal[i].i_type == NAL_SEI)
//            {
//                continue;
//            }
//            
//            memcpy(pTemp+nPos, nal[i].p_payload, nal[i].i_payload);
//            nPos += nal[i].i_payload;
//        }
//
////        fwrite(&nPos, 1, 4, f);
////        fwrite(outData, 1, nPos, f);
//        
//        free(outData);
//    }
//    
//
//    free(picture_buf);
//    fclose(f);
    
//}

- (void)viewDidLoad
{
    [super viewDidLoad];

    iMpuSdk *s = [[iMpuSdk alloc] init];
    Device *pu = [[Device alloc] init];
    pu.puid = @"151355143040549015";

    [s startService:@"192.168.42.118" port:8866 psd:@"" device:pu target:self.view];
}

- (IBAction)btnSend:(id)sender
{
    //x264_enocode();
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)dealloc{
//    [glView release];
//    processor.delegate = nil;
//    [processor release];
    [super dealloc];
}

uint8_t yBuffer[640*480];
uint8_t uvBuffer[640*240];
//uint8_t vBuffer[320*240];
uint8_t yuvBuffer[640*720];

- (void)pixelBufferReadyForDisplay:(CVPixelBufferRef)pixelBuffer
{
//    if ([UIApplication sharedApplication].applicationState != UIApplicationStateBackground)
//    {
//        [glView displayPixelBuffer:pixelBuffer];
//    }
//    CVReturn sucess = CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//    if (sucess != kCVReturnSuccess)
//    {
//        return;
//    }
//    
//    size_t width = CVPixelBufferGetWidth(pixelBuffer);
//    size_t height = CVPixelBufferGetHeight(pixelBuffer);
//    memset(yBuffer, 0, width*height);
//    size_t byte = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//    void *base = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
//    if (byte == width)
//    {
//        memcpy(yBuffer, base, width*height);
//    }
//    
//    base = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
//    byte = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
//    memset(uvBuffer, 0, 640*240);
//    memcpy(uvBuffer, base, 640*240);
//    
//    memset(yuvBuffer, 0, 640*720);
//    uint8_t *pYUV = yuvBuffer;
//    memcpy(pYUV, yBuffer, 640*480);
//    pYUV += 640*480;
//    memcpy(pYUV, uvBuffer, 640*240);
//    
//    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown);
}

@end
