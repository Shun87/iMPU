//
//  AudioPlay.h
//  iMpuSdk
//
//  Created by  on 12-12-4.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#ifndef iMpuSdk_AudioPlay_h
#define iMpuSdk_AudioPlay_h

#include <AudioToolbox/AudioToolbox.h>

#define kNumberBuffers 3

class AudioPlay {
    
public:
    AudioQueueBufferRef		inCompleteAQBuffer;
    BOOL					m_bQueueStart;
	BOOL					m_bFirstQueueStart; 
    AudioQueueRef			mQueue;
    BOOL                    m_bIARunning;
    AudioQueueBufferRef		mBuffers[kNumberBuffers];
    void InitQueue();
    OSStatus startQueue();
    void stopQueue();
protected:
};

#endif
