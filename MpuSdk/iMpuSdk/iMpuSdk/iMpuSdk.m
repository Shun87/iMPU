//
//  iMpuSdk.m
//  iMpuSdk
//
//  Created by  on 12-11-26.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "iMpuSdk.h"
#import "DataPipe.h"
#import "GLView.h"
@interface iMpuSdk()
{
    DataPipe *pipe;
}
@property (nonatomic, retain)DataPipe *pipe;

@end

@implementation iMpuSdk
@synthesize pipe;

- (NSInteger)startService:(NSString *)ip 
              port:(u_short)port 
               psd:(NSString *)psd 
            device:(Device *)device
            target:(UIView *)displayView
{
    self.pipe = [[[DataPipe alloc] init] autorelease];
    self.pipe.displayView = displayView;
    return [pipe creatCmdPort:ip port:8866 psd:psd device:device];
}

- (NSInteger)startService:(UIView *)displayView
{
//    self.glView = [[[GLView alloc] initWithFrame:displayView.bounds] autorelease];
//    self.glView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin 
//                                    | UIViewAutoresizingFlexibleTopMargin
//                                    | UIViewAutoresizingFlexibleWidth
//                                    | UIViewAutoresizingFlexibleHeight;
//    [displayView addSubview:self.glView];
    
    
    return 0;
}

- (void)dealloc
{
    [pipe release];
    [super dealloc];
}
@end
