//
//  CmdPort.h
//  iMpuSdk
//
//  Created by  on 12-11-19.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Device.h"
#import "CmdDefs.h"

typedef void (^GatherDataBlock)(NSString *optID, int index, 
                                NSString *type, NSString *ip, u_short port, NSString *token);

/**
 *	@brief	专门处理接收命令并作出响应
 */
@interface CmdPort : NSObject

{
    dispatch_queue_t connectQueue;
    GatherDataBlock gatherBlock;
}
@property (nonatomic, copy)GatherDataBlock gatherBlock;
- (NSInteger)creat:(NSString *)ip 
              port:(u_short)port 
               psd:(NSString *)psd 
            device:(Device *)device
            block:(GatherDataBlock)block;
@end
