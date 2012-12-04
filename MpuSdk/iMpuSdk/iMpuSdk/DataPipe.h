//
//  DataPipe.h
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CmdPort.h"
#import "MediaDataCapture.h"

#import "GLView.h"

#import "VideoCapturer.h"

/**
 *	@brief	数据管道
 */
@interface DataPipe : NSObject<CapturerDelegate>
{
    CmdPort *cmdPort;
    MediaDataCapture *capture;
    UIView *displayView;
    
    GLView *glView;
    
    MediaType mediaType;
    
    NSMutableArray *portArray;
}
@property (nonatomic, assign)UIView *displayView;

// 创建命令通道
- (NSInteger)creatCmdPort:(NSString *)ip 
                     port:(u_short)port 
                      psd:(NSString *)psd 
                   device:(Device *)device;
- (void)closePorts;

- (void)startCapture;
@end
