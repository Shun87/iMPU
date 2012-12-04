//
//  PUInfo.m
//  iMpuSdk
//
//  Created by  on 12-11-19.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "Device.h"

@implementation Device
@synthesize  produceID, deviceID, puid, ip, port, epid,
deviceType, deviceModel, hardwareVersion, softwareVerion;

- (id)init
{
    if (self  = [super init])
    {
        self.produceID = @"00002";
        self.hardwareVersion = @"iPhone";
        self.softwareVerion = @"v7.1.12.1120";
        self.deviceID = @"00001-4556678734";
        self.deviceType = @"WENC";
        self.deviceModel = @"CR600M";
        self.ip = @"192.168.43.15";
        self.port = 8886;
        self.epid = @"system";
    }
    
    return self;
}

- (void)dealloc
{
    [produceID release];
    [deviceID release];
    [deviceType release];
    [deviceModel release];
    [hardwareVersion release];
    [softwareVerion release];
    [puid release];
    [ip release];
    [epid release];
    [super release];
}
@end
