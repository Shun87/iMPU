//
//  Render.m
//  PreviewController
//
//  Created by  on 12-3-24.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "Render.h"
#import "ShaderUtilities.h"

#define STRINGIFY(A)  #A

#include "process.vsh"
#include "process.fsh"

@implementation Render

enum
{
    ATTRIB_VERTEX,
    ATTRIB_TEXTUREPOSITION,
    NUM_ATTRIBUTES
};

enum {
    UNIFORM_Y,
    UNIFORM_U,
    UNIFORM_V,
    NUM_UNIFORMS
};

// attributes
GLint attribLocation[NUM_ATTRIBUTES] = {
    ATTRIB_VERTEX, ATTRIB_TEXTUREPOSITION,  
};

GLint uniformLocation[NUM_UNIFORMS] = {
    UNIFORM_Y, UNIFORM_U,UNIFORM_V,
};

- (id)initWithLayer:(CAEAGLLayer *)layer
{
    if (self = [super init])
    {
        _layer = layer;
        
        glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!glContext || ![EAGLContext setCurrentContext:glContext]) {
            NSLog(@"Problem with OpenGL context.");
        }
        
        [self initializeBuffers];
    }
    
    return self;
}

- (void)resize
{
    
}

- (BOOL)initializeBuffers
{
    BOOL success = YES;
    
    glDisable(GL_DEPTH_TEST);
    
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    glGenRenderbuffers(1, &colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [glContext renderbufferStorage:GL_RENDERBUFFER 
                      fromDrawable:_layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderBufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderBufferHeight);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, colorRenderBuffer);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"Failure with framebuffer generation");
		success = NO;
	}
    
    // 字符串数组
    GLchar *attribName[NUM_ATTRIBUTES] = {
        "position", "textureCoordinate",  
    };
    
    GLchar *uniformName[NUM_UNIFORMS] = {
        "SamplerY", "SamplerU", "SamplerV"
    };
    
    glueCreateProgram(vertexShader, fragmentShader,
                      NUM_ATTRIBUTES, (const GLchar **)&attribName, attribLocation,
                      NUM_UNIFORMS, (const GLchar **)&uniformName, uniformLocation, &program);
    if (!program)
    {
        success = NO;
    }
    
    return success;
}

- (void)renderWithSquareVertices:(const GLfloat*)squareVertices textureVertices:(const GLfloat*)textureVertices
{
    // Use shader program.
    glUseProgram(program);
    
    // Update attribute values.
	glVertexAttribPointer(attribLocation[ATTRIB_VERTEX], 2, GL_FLOAT, 0, 0, squareVertices);
	glEnableVertexAttribArray(attribLocation[ATTRIB_VERTEX]);
	glVertexAttribPointer(attribLocation[ATTRIB_TEXTUREPOSITION], 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(attribLocation[ATTRIB_TEXTUREPOSITION]);
    
    // Update uniform values if there are any
    
    // 0 ==> GL_TEXTURE0
    // 1 ==> GL_TEXTURE1
	glUniform1i(uniformLocation[UNIFORM_Y], 0);	
    glUniform1i(uniformLocation[UNIFORM_U], 1);
    glUniform1i(uniformLocation[UNIFORM_V], 2);
    
    // 
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Present
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [glContext presentRenderbuffer:GL_RENDERBUFFER];
}
//ipad 1280 720
// iphone 640 480
uint8_t uBuffer[640*360];
uint8_t vBuffer[640*360];
- (void)uploadTexture:(GLuint *)texture pixelBuffer:(CVImageBufferRef)pixelBuffer plane:(NSInteger)index
{
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    GLuint type = GL_LUMINANCE;
    if (index != 0)
    {
        type = GL_LUMINANCE;
        width = width/2;
        height = height/2;
    }
    
    if (*texture == 0)
    {
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, 0);
    }
    
    glBindTexture(GL_TEXTURE_2D, *texture);
    if (index == 0)
    {
        unsigned char *yData = (unsigned char *)malloc(width * height);
        assert(yData != NULL);
        memset(yData, 0, width*height);
        size_t byte = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
        void *base = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
        if (byte == width)
        {
            memcpy(yData, base, width*height);
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, type, GL_UNSIGNED_BYTE, yData);
        free(yData);
    }
    else if(index == 1)
    {
        void *base = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        size_t byte = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        size_t uByte = byte/2;
        
        memset(vBuffer, 0, uByte*height);
        memset(uBuffer, 0, uByte*height);
        uint8_t *uTemp = uBuffer;
        uint8_t *vTemp = vBuffer;
        for (int i=0; i<height; i++)
        {
            for (int i=0; i<byte/2; i++)
            {
                *uTemp++ = *(unsigned char *)base++;
                *vTemp++ = *(unsigned char *)base++;
            }
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, type, GL_UNSIGNED_BYTE, uBuffer);
    }
    else
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, type, GL_UNSIGNED_BYTE, vBuffer);
    }
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

- (void)displayPixelBuffer:(CVImageBufferRef)pixelBuffer
{   
    // Retrieving the base address for a PixelBuffer requires that the buffer base address be locked
    // via a successful call to CVPixelBufferLockBaseAddress.
    CVReturn sucess = CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    if (sucess != kCVReturnSuccess)
    {
        return;
    }
    
    [self uploadTexture:&yTexture pixelBuffer:pixelBuffer plane:0];
    [self uploadTexture:&uTexture pixelBuffer:pixelBuffer plane:1];
    [self uploadTexture:&vTexture pixelBuffer:pixelBuffer plane:2];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTexture);
//    
    
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    // Set the view port to the entire view
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
	
    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    static const GLfloat textureVertices3[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,
    };
    
    // Draw the texture on the screen with OpenGL ES 2
    [self renderWithSquareVertices:squareVertices textureVertices:textureVertices3];
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
//Your code doesn't look to0 bad. I can see two mistakes and one potential problem:

//The uvPixelCount is incorrect. The YUV 420 format means that there is color information for each 2 by 2 pixel block.
// So the correct count is:
//
//uvPixelCount = (width / 2) * (height / 2);
//You write something about yPixelCount / 4, but I cannot see that in your code.
//
//The UV information is interleaved, i.e. the second plane alternatingly contains a U and a V value.

// Or put differently: there's a U value on all even byte addresses and a V value on all odd byte addresses.
// If you really need to separate the U and V information, memcpy won't do.
//
//There can be some extra bytes after each pixel row. 
// You should use CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0) to get the number of bytes between two rows. 
//As a consequence, a single memcpy won't do. 
//Instead you need to copy each pixel row separately to get rid of the extra bytes between the rows.
//
//All these things only explain part of the resulting image. 
//The remaining parts are probably due to differences between your code and what the receiving peer expect.
////You did't write anything about that? Does the peer really need separated U and V values? 
//Does it you 4:2:0 compression as well? Does it you video range instead of full range as well?



//Turning the biplanar image format into a planar is simply: just copy the two planes into a single memory block. 
//The planar format is just the concatenation of the luma and the chroma plane. 
//But I'm still not sure what target format you need: YUV420 is simply not a precise description. 
//And while I've given you a lot of information about image formats, I've the impression you in fact need a video format. Please post as much details about the target format as possible
// "420v" (fourcc: NV12) is a bi-planar format while 420p is planar
// The VideoRange or FullRange suffix simply indicates whether the bytes are returned between 16 - 235 for Y 
//and 16 - 240 for UV or full 0 - 255 for each component.


//I believe the default colorspace used by an AVCaptureVideoDataOutput instance is the YUV 4:2:0 planar colorspace
//(except on the iPhone 3G, where it's YUV 4:2:2 interleaved). 
//This means that there are two planes of image data contained within the video frame, 
//with the Y plane coming first. For every pixel in your resulting image, there is one byte for the Y value at that pixel.


//A row in the image might be longer than the width of the image (due to rounding). 
//That's why there are separate functions for getting the width and the number of bytes per row. 
//You don't have this problem at the moment. But that might change with the next version of iOS. So your code should be:
//#pragma mark -
//#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate Methods
//#if !(TARGET_IPHONE_SIMULATOR)
//- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;
//{
//    // get image buffer reference
//    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
//    
//    // extract needed informations from image buffer
//    CVPixelBufferLockBaseAddress(imageBuffer, 0);
//    size_t bufferSize = CVPixelBufferGetDataSize(imageBuffer);
//    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
//    CGSize resolution = CGSizeMake(CVPixelBufferGetWidth(imageBuffer), CVPixelBufferGetHeight(imageBuffer));
//    
//    // variables for grayscaleBuffer 
//    void *grayscaleBuffer = 0;
//    size_t grayscaleBufferSize = 0;
//    
//    // the pixelFormat differs between iPhone 3G and later models
//    OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);
//    
//    if (pixelFormat == '2vuy') { // iPhone 3G
//        // kCVPixelFormatType_422YpCbCr8     = '2vuy',    
//        /* Component Y'CbCr 8-bit 4:2:2, ordered Cb Y'0 Cr Y'1 */
//        
//        // copy every second byte (luminance bytes form Y-channel) to new buffer
//        grayscaleBufferSize = bufferSize/2;
//        grayscaleBuffer = malloc(grayscaleBufferSize);
//        if (grayscaleBuffer == NULL) {
//            NSLog(@"ERROR in %@:%@:%d: couldn't allocate memory for grayscaleBuffer!", NSStringFromClass([self class]), NSStringFromSelector(_cmd), __LINE__);
//            return nil; }
//        memset(grayscaleBuffer, 0, grayscaleBufferSize);
//        void *sourceMemPos = baseAddress + 1;
//        void *destinationMemPos = grayscaleBuffer;
//        void *destinationEnd = grayscaleBuffer + grayscaleBufferSize;
//        while (destinationMemPos <= destinationEnd) {
//            memcpy(destinationMemPos, sourceMemPos, 1);
//            destinationMemPos += 1;
//            sourceMemPos += 2;
//        }       
//    }
//    
//    if (pixelFormat == '420v' || pixelFormat == '420f') {
//        // kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange = '420v', 
//        // kCVPixelFormatType_420YpCbCr8BiPlanarFullRange  = '420f',
//        // Bi-Planar Component Y'CbCr 8-bit 4:2:0, video-range (luma=[16,235] chroma=[16,240]).  
//        // Bi-Planar Component Y'CbCr 8-bit 4:2:0, full-range (luma=[0,255] chroma=[1,255]).
//        // baseAddress points to a big-endian CVPlanarPixelBufferInfo_YCbCrBiPlanar struct
//        // i.e.: Y-channel in this format is in the first third of the buffer!
//        int bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);
//        grayscaleBufferSize = resolution.height * bytesPerRow ;
//        grayscaleBuffer = malloc(grayscaleBufferSize);
//        if (grayscaleBuffer == NULL) {
//            NSLog(@"ERROR in %@:%@:%d: couldn't allocate memory for grayscaleBuffer!", NSStringFromClass([self class]), NSStringFromSelector(_cmd), __LINE__);
//            return nil; }
//        memset(grayscaleBuffer, 0, grayscaleBufferSize);
//        memcpy (grayscaleBuffer, baseAddress, grayscaleBufferSize); 
//    }
//    
//    // do whatever you want with the grayscale buffer
//    ...
//    
//    // clean-up
//    free(grayscaleBuffer);
//}
//#endif
@end
