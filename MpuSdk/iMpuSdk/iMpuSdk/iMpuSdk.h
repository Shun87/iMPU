//
//  iMpuSdk.h
//  iMpuSdk
//
//  Created by  on 12-11-26.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "Device.h"

@interface iMpuSdk : NSObject

- (NSInteger)startService:(NSString *)ip 
              port:(u_short)port 
               psd:(NSString *)psd 
            device:(Device *)device
            target:(UIView *)displayView;

//- (NSInteger)startService:(UIView *)displayView;
@end
