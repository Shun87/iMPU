//
//  Response.h
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Request.h"

/**
 *	@brief	网络请求的响应
 */
@interface Response : NSObject

// 由请求得到响应报文
+ (NSString *)responseWithRequestXml:(Request *)request;

@end
