//
//  GLView.h
//  PreviewController
//
//  Created by  on 12-3-23.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/glext.h>

#import "Render.h"

@interface GLView : UIView
{
    Render *_render;
}

- (void)displayPixelBuffer:(CVImageBufferRef)pixelBuffer;

@end
