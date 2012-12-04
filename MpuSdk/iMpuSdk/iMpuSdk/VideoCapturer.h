//
//  VideoCapturer.h
//  PreviewController
//
//  Created by  on 12-3-23.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CMBufferQueue.h>
#import <AVFoundation/AVFoundation.h>
#import "Capturer.h"

@interface VideoCapturer : Capturer<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    AVCaptureSession *captureSession;
    AVCaptureConnection *videoConnection;
    
    CMVideoDimensions videoDimensions;
    CMBufferQueueRef previewBufferQueue;
    
}

- (void)setupAndStartCaptureSession;
- (void) showError:(NSError*)error;
@end


