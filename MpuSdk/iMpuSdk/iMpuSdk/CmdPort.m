//
//  CmdPort.m
//  iMpuSdk
//
//  Created by  on 12-11-19.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "CmdPort.h"
#include "NSS7.h"
#include <CommonCrypto/CommonDigest.h>
#import "XOS.h"
#import "Request.h"
#import "Response.h"


typedef enum _ConnectStatus
{
    Break = 0x01,
	Connecting,
    Reporting,
    RecvRequest,
} ConnectStatus;

@interface CmdPort()
{
    ConnectStatus status;
    NSS7_HANDLE NSS7;
}
- (NSInteger)getConnectStatus;
- (NSInteger)sendReport;
- (NSInteger)receiveRequest;
- (NSInteger)sendResponse:(const char *)request len:(u_int)len  transID:(u_int)transID;
@end

@implementation CmdPort
@synthesize gatherBlock;

- (id)init
{
    if (self  = [super init])
    {
        status = Break;
        NSS7 = 0;
    }
    
    return self;
}

- (NSInteger)creat:(NSString *)ip 
              port:(u_short)port 
               psd:(NSString *)psd 
            device:(Device *)device
             block:(GatherDataBlock)block
{
    self.gatherBlock = block;
    
    if (NSS7_DNS2IPInit() != NSS7_E_OK)
    {
        return NSS7_E_UNKNOW;
    }
    
    if (ip == nil)
    {
        return NSS7_E_PARAM;
    }
    
    if (port == 0)
    {
        return NSS7_E_PARAM;
    } 
    
    u_char ucPsdHash[NSS7_MAXLEN_HASHPSW] = {0};
    CC_MD5_CTX ctx;
    CC_MD5_Init(&ctx);
    CC_MD5_Update(&ctx, [psd UTF8String], strlen([psd UTF8String]));
    CC_MD5_Final(ucPsdHash, &ctx);
    
    unsigned char szProducePsd[64] = "DSoighLLQW9XZCzmn36CQ435i36J3E7Q3K7n38F3a53ep2S11m23n5d2a67k2fH";
    // 不支持直连 
    NSS7 = NSS7_Create((char *)[ip UTF8String], 
                                   port, 
                                   ucPsdHash,
                                   (char *)[device.puid UTF8String],
                                   XOS_AddrToU32("0.0.0.0"),
                                   XOS_AddrToU32("255.255.255.255"),
                                   0,
                                   (char *)[device.produceID UTF8String],
                                   szProducePsd,
                                   1,
                                   "212124324",
                                   "WENC",
                                   "iPhone",
                                   "HV2",
                                   "SV2",
                                   "",
                                   0, "", "", 0, "", "", 0, 0);
    
    if (NSS7 != NULL)
    {
        status = Connecting;
    }
    else
    {
        NSLog(@"NCS7_Create error");
        return NSS7_E_MALLOC;
    }

    connectQueue = dispatch_queue_create("NCS7_GetConnectStatusQueue", NULL);
    dispatch_async(connectQueue, ^{
    int nRet = NSS7_E_OK;
    while (1) 
    {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        switch (status)
        {
            case Connecting:
            {
                nRet = [self getConnectStatus];
                if (nRet == NSS7_E_OK)
                {
                    status = Reporting;
                }
                else if (nRet < 0)
                {
                    status = Break;
                    if (NSS7 != NULL)
                    {
                        NSS7_Close(NSS7);
                        NSS7 = NULL;
                    }
                }
            }
            break;
            case Reporting:
            {
                nRet = [self sendReport];
                if (nRet == NSS7_E_OK)
                {
                    status = RecvRequest;
                }
                else
                {
                    NSS7_Close(NSS7);
                }
            }break;
            case RecvRequest:
            {
                [self receiveRequest];
            }
            default:
                break;
        }
        
        [pool drain];
    }});
    
    return NSS7_E_OK;
}

- (NSInteger)receiveRequest
{
    // 推送数据
    NSInteger nRet = NSS7_Run(NSS7);
    if (nRet < NSS7_E_OK)
    {
        NSLog(@"NSS7_Run error");
        return nRet;
    }
    
    char *pMsgBody = NULL;
    int nMsgBodyLen = 0;
    u_int uiTransID = 0;
    nRet = NSS7_RecvRequest(NSS7, &pMsgBody, &nMsgBodyLen, &uiTransID);
    if (nRet == NSS7_E_OK)
    {
        [self sendResponse:pMsgBody len:nMsgBodyLen transID:uiTransID];
        
    }
    
    if (pMsgBody != NULL)
    {
        NSS7_Free(NSS7, pMsgBody);
    }
    
    return nRet;
}

- (NSInteger)sendResponse:(const char *)request len:(u_int)len  transID:(u_int)transID
{
    const int RESPONSE_UNSPPORT_CTRL  = 6146;
    const int RESPONSE_SUCCESS        = 0;
    
    int nErrorCode = 0;
    NSData *data = [NSData dataWithBytes:request length:len];
    Request *requestData = [[[Request alloc] initWithRequest:data] autorelease];
    if ([requestData.optID isEqualToString: CTL_COMMONRES_StartStream_PushMode])
    {
        nErrorCode = RESPONSE_SUCCESS;
        requestData.supported  = YES;
    }
    else if ([requestData.optID isEqualToString: CTL_OA_StartCall_PushMode]
             || [requestData.optID isEqualToString: CTL_OA_StartTalk_PushMode])
    {
        nErrorCode = RESPONSE_SUCCESS;
        requestData.supported = YES;
    }
    else
    {
        nErrorCode = RESPONSE_UNSPPORT_CTRL;
        requestData.supported = NO;
    }
    
    
#ifdef DEBUG
    NSLog(@"%s\r\n", request);
#endif
    
    NSString *responseString = [Response responseWithRequestXml:requestData];
    
    int nRet = NSS7_SendResponse(NSS7, (char *)
                             [responseString UTF8String], 
                             [responseString length], transID);
    if (nRet == NSS7_E_OK)
    {
        NSString *ip = [requestData.dstResDictionary objectForKey:@"IP"];
        NSString *port = [requestData.dstResDictionary objectForKey:@"Port"];
        NSString *token = [requestData.dstResDictionary objectForKey:@"Token"];
        if (ip != nil && port != nil && token != nil)
        {
            gatherBlock(requestData.optID, requestData.cIndex, requestData.resType, 
                        ip, [port intValue], token);
            
        }
    }
    
    return nRet;
}

- (NSInteger)getConnectStatus
{
    int nRet = NSS7_GetConnectStatus(NSS7);
    if (nRet == NSS7_E_OK)
    {
        NSLog(@"NSS7_GetConnectStatus success");
    }
    else if (nRet < 0)
    {
        NSLog(@"NSS7_GetConnectStatus error");
    }

    return nRet;
}

- (NSInteger)sendReport
{
    NSString *filPath = [[NSBundle mainBundle] pathForResource:@"report" ofType:@"xml"];
    NSError *error = nil;
    NSString *content = [NSString stringWithContentsOfFile:filPath 
                                                  encoding:NSUTF8StringEncoding 
                                                     error:&error];
    return NSS7_SendReport(NSS7, (char *)[content UTF8String], strlen([content UTF8String]));
}

- (void)dealloc
{
    [gatherBlock release];
    [super dealloc];
}
@end
