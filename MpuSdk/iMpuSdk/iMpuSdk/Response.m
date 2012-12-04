//
//  Response.m
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "Response.h"
#import "mxml.h"
#import "mymxml.h"

@implementation Response

+ (NSString *)responseWithRequestXml:(Request *)request
{
    mymxml_t *xml_t = mymxmlNew("1.0");
    NSAssert(xml_t != NULL, @"");
    
    mymxmlInsertNode(xml_t, "/Msg", NULL);
    mymxmlSetAttributeValue(xml_t, "/Msg", "Name", "CUCommonMsgRsp");
    
    mymxmlInsertNode(xml_t, "/Msg/Cmd", NULL);
    mymxmlSetAttributeValue(xml_t, "/Msg/Cmd", "Type", "CTL");
    mymxmlSetAttributeValue(xml_t, "/Msg/Cmd", "NUErrorCode", "0");
    
    mymxmlInsertNode(xml_t, "/Msg/Cmd/DstRes", NULL);
    mymxmlSetAttributeValue(xml_t, "/Msg/Cmd/DstRes", "Type", [request.resType UTF8String]);
    
    NSString *text = [NSString stringWithFormat:@"%d", request.cIndex];
    mymxmlSetAttributeValue(xml_t, "/Msg/Cmd/DstRes", "Idx", [text UTF8String]);
    mymxmlSetAttributeValue(xml_t, "/Msg/Cmd/DstRes", "OptID", [request.optID UTF8String]);
    
    int code = (request.supported ? 0 : 6146);
    text = [NSString stringWithFormat:@"%d", code];
    mymxmlSetAttributeValue(xml_t, "/Msg/Cmd/DstRes", "ErrorCode", [text UTF8String]);
    
    char buffer[1024];
    mymxmlSaveString(xml_t, buffer, 1024);
    mymxmlDelete(xml_t);
    
    return [NSString stringWithCString:buffer encoding:NSUTF8StringEncoding];
}
@end
