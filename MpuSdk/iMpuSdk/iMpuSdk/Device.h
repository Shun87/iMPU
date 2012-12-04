//
//  PUInfo.h
//  iMpuSdk
//
//  Created by  on 12-11-19.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface Device : NSObject {
@private
    NSString *puid;
    u_short port;
    NSString *epid;
    NSString *ip;
    NSString *produceID;
    NSString *deviceID;
    NSString *deviceType;
    NSString *deviceModel;
    NSString *hardwareVersion;
    NSString *softwareVerion;
}
@property (nonatomic, copy)NSString *epid;
@property (nonatomic, assign)ushort port;
@property (nonatomic, copy)NSString *ip;
@property (nonatomic, copy)NSString *puid;
@property (nonatomic, copy)NSString *produceID;
@property (nonatomic, copy)NSString *deviceID;
@property (nonatomic, copy)NSString *deviceType;
@property (nonatomic, copy)NSString *deviceModel;
@property (nonatomic, copy)NSString *hardwareVersion;
@property (nonatomic, copy)NSString *softwareVerion;
@end
