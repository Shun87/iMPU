//
//  Request.h
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *	@brief	对网络命令请求的封装
 */
@interface Request : NSObject

{
    NSXMLParser *parser;
    NSString *optID;
    NSString *resType;
    u_char cIndex;
    BOOL supported;
    NSMutableDictionary *dstResDictionary;
    NSMutableDictionary *objSetDictionary;
}
@property (nonatomic, assign)BOOL supported;
@property (nonatomic, copy)NSString *optID;
@property (nonatomic, copy)NSString *resType;
@property (nonatomic, assign)u_char cIndex;
@property (nonatomic, retain)NSMutableDictionary *dstResDictionary;
@property (nonatomic, retain)NSMutableDictionary *objSetDictionary;
- (id)initWithRequest:(NSData *)data;
@end
