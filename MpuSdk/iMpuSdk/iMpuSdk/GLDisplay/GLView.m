//
//  GLView.m
//  PreviewController
//
//  Created by  on 12-3-23.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "GLView.h"
#import "ShaderUtilities.h"

@implementation GLView

// 可以根据os version来判断
//#ifdef __IPHONE_5_0
#define TextureFastUpload   0
//#endif
//

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (void)initRender
{
    _render = [[Render alloc] initWithLayer:(CAEAGLLayer *)self.layer];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.contentScaleFactor = [[UIScreen mainScreen] scale];
        CAEAGLLayer *eagllayer = (CAEAGLLayer *)self.layer;
        eagllayer.opaque = YES;
        eagllayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
        [self initRender];
        
    }
    return self;
}

- (void)layoutSubviews
{
    [_render resize];
}

- (void)dealloc
{
    [_render release];
    [super dealloc];
}

- (void)displayPixelBuffer:(CVImageBufferRef)pixelBuffer
{
    if (_render == nil)
    {
        [self initRender];
    }
    [_render displayPixelBuffer:pixelBuffer];
}

@end
