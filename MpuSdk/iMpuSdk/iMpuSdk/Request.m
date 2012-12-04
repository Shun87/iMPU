//
//  Request.m
//  iMpuSdk
//
//  Created by  on 12-11-22.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "Request.h"
@interface Request()

@property (nonatomic, retain)NSXMLParser *parser;

@end

@implementation Request
@synthesize dstResDictionary, optID, parser, objSetDictionary, cIndex, resType, supported;
- (id)initWithRequest:(NSData *)data
{
    if (self = [super init])
    {
        self.dstResDictionary = [[[NSMutableDictionary alloc] init] autorelease];
        self.objSetDictionary = [[[NSMutableDictionary alloc] init] autorelease];
        self.parser = [[[NSXMLParser alloc] initWithData:data] autorelease];
        parser.delegate = (id<NSXMLParserDelegate>)self;
        [parser parse];
    }
    
    return self;
}

- (void)parser:(NSXMLParser *)parser 
            didStartElement:(NSString *)elementName 
            namespaceURI:(NSString *)namespaceURI 
            qualifiedName:(NSString *) qualifiedName 
            attributes:(NSDictionary *)attributeDict 
{
    if ([elementName isEqualToString:@"DstRes"])
    {
        self.optID = (NSString *)[attributeDict objectForKey:@"OptID"];
        self.resType = (NSString *)[attributeDict objectForKey:@"Type"];
        self.cIndex = [(NSString *)[attributeDict objectForKey:@"Type"] intValue];
        [self.dstResDictionary addEntriesFromDictionary:attributeDict];
    }
    else if ([elementName isEqualToString:@"Param"])
    {
        [self.dstResDictionary addEntriesFromDictionary:attributeDict];
    }
    else if ([elementName isEqualToString:@"Res"])
    {
        [self.objSetDictionary addEntriesFromDictionary:attributeDict];
    }
}

- (void)dealloc
{
    [optID release];
    [objSetDictionary release];
    [dstResDictionary release];
    [parser release];
    [resType release];
    
    [super dealloc];
}
@end
