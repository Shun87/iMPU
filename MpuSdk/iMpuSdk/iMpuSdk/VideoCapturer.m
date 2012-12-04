//
//  VideoCapturer.m
//  PreviewController
//
//  Created by  on 12-3-23.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "VideoCapturer.h"

@implementation VideoCapturer

- (id)init
{
    if (self = [super init])
    {
        
    }
    
    return self;
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput 
        didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
       fromConnection:(AVCaptureConnection *)connection
{
    CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
   // CMTime timestamp = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
    
    if (videoDimensions.width == 0 && videoDimensions.height == 0)
    {
        videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
    }
  
    OSStatus err = CMBufferQueueEnqueue(previewBufferQueue, sampleBuffer);
    if ( !err ) {        
        dispatch_async(dispatch_get_main_queue(), ^{
            CMSampleBufferRef sbuf = (CMSampleBufferRef)CMBufferQueueDequeueAndRetain(previewBufferQueue);
            if (sbuf) {
                CVImageBufferRef pixBuf = CMSampleBufferGetImageBuffer(sbuf);
                [self.delegate pixelBufferReadyForDisplay:pixBuf];
                CFRelease(sbuf);
            }
        });
    }
}

- (AVCaptureDevice *)videoDeviceWithPosition:(AVCaptureDevicePosition)position 
{
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in devices)
        if ([device position] == position)
            return device;
    
    return nil;
}

- (void)setupCaptureSession
{
    /*
	 * Create video connection
	 */
    captureSession = [[AVCaptureSession alloc] init];
    AVCaptureDeviceInput *videoIn = [[AVCaptureDeviceInput alloc] initWithDevice:
                                     [self videoDeviceWithPosition:AVCaptureDevicePositionBack] error:nil];
    if ([captureSession canAddInput:videoIn])
        [captureSession addInput:videoIn];
	[videoIn release];
    
	AVCaptureVideoDataOutput *videoOut = [[AVCaptureVideoDataOutput alloc] init];
	/*
     RosyWriter prefers to discard late video frames early in the capture pipeline, since its
     processing can take longer than real-time on some platforms (such as iPhone 3GS).
     Clients whose image processing is faster than real-time should consider setting AVCaptureVideoDataOutput's
     alwaysDiscardsLateVideoFrames property to NO. 
	 */
	[videoOut setAlwaysDiscardsLateVideoFrames:YES];
	[videoOut setVideoSettings:[NSDictionary dictionaryWithObject:
                                [NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] 
                                                           forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
	dispatch_queue_t videoCaptureQueue = dispatch_queue_create("Video Capture Queue", DISPATCH_QUEUE_SERIAL);
	[videoOut setSampleBufferDelegate:self queue:videoCaptureQueue];
	dispatch_release(videoCaptureQueue);
	if ([captureSession canAddOutput:videoOut])
		[captureSession addOutput:videoOut];
	//videoConnection = [videoOut connectionWithMediaType:AVMediaTypeVideo];
	//self.videoOrientation = [videoConnection videoOrientation];
	[videoOut release];
}

- (void)setupAndStartCaptureSession
{
    // Create a shallow queue for buffers going to the display for preview.
	OSStatus err = CMBufferQueueCreate(kCFAllocatorDefault, 1, CMBufferQueueGetCallbacksForUnsortedSampleBuffers(), &previewBufferQueue);
	if (err)
		[self showError:[NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil]];

    if (!captureSession)
    {
        [self setupCaptureSession];
    }
    
    if (!captureSession.isRunning)
    {
        [captureSession startRunning];
    }
}

#pragma mark Error Handling

- (void)showError:(NSError *)error
{
    CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, ^(void) {
//        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:[error localizedDescription]
//                                                            message:[error localizedFailureReason]
//                                                           delegate:nil
//                                                  cancelButtonTitle:@"OK"
//                                                  otherButtonTitles:nil];
//        [alertView show];
//        [alertView release];
    });
}

- (void)startCapture
{
    [self setupAndStartCaptureSession];
}

- (void)stopCature
{
    
}
@end
