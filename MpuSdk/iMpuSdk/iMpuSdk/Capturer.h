//
//  Capturer.h
//  iMpuSdk
//
//  Created by  on 12-11-26.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>

@protocol CapturerDelegate;

@interface Capturer : NSObject
{
    id<CapturerDelegate> delegate;
}
@property (readwrite, assign) id<CapturerDelegate> delegate;

- (void)startCapture;
- (void)stopCature;

@end

@protocol CapturerDelegate <NSObject>
@required
- (void)pixelBufferReadyForDisplay:(CVPixelBufferRef)pixelBuffer;	// This method is always called on the main thread.
@end