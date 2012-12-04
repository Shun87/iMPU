//
//  AudioPlay.cpp
//  iMpuSdk
//
//  Created by  on 12-12-4.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#include "AudioPlay.h"

#include "platform.h"

#define kNumberBuffers				3
#define kNumberIAFrame              10

struct AudioData
{
    char *pBuffer;
    UINT uiLen;
};

static std::vector<AudioData *>			vctUseList;
static std::vector<AudioData *>			vctUnuseList;
static CRITICAL_SECTION						useMutext;
static CRITICAL_SECTION						unUseMutext;

void AQOutCallBack(void *						inUserData,
				   AudioQueueRef				inAQueue,
				   AudioQueueBufferRef			inCompleteAQBuffer)
{
	AudioPlay *This = (AudioPlay *)inUserData;
	This->inCompleteAQBuffer = inCompleteAQBuffer;
    
	EnterCriticalSection(&useMutext);
	if (This->m_bQueueStart)
	{
#ifdef _DEBUG
        printf("AQOutCallBack>>>>>>>>>>>>>>>>>>>>vctUseList size = %lu\r\n", vctUseList.size());
#endif
		if (vctUseList.size() > 0)
		{
			AudioData *pData = vctUseList[0];
            memset(inCompleteAQBuffer->mAudioData, 0, inCompleteAQBuffer->mAudioDataBytesCapacity);
            
			memcpy(inCompleteAQBuffer->mAudioData, pData->pBuffer, pData->uiLen);
			inCompleteAQBuffer->mAudioDataByteSize = pData->uiLen;
			AudioQueueEnqueueBuffer(This->mQueue, inCompleteAQBuffer, 0, NULL);
			vctUseList.erase(vctUseList.begin());
			
			EnterCriticalSection(&unUseMutext);
			vctUnuseList.push_back(pData);
			LeaveCriticalSection(&unUseMutext);
		}
		else
		{	
            This->m_bQueueStart = FALSE;
            AudioQueueStop(inAQueue, FALSE); 
		}
	}
	LeaveCriticalSection(&useMutext);
}

void IsRunningProc (void *              inUserData,
                    AudioQueueRef           inAQ,
                    AudioQueuePropertyID    inID)
{
    AudioPlay *This = (AudioPlay *)inUserData;
    UInt32 size = sizeof(This->m_bIARunning);
	OSStatus result = AudioQueueGetProperty (inAQ, kAudioQueueProperty_IsRunning, &This->m_bIARunning, &size);
	
	if ((result == noErr) && (!This->m_bIARunning))
    {
        printf("AudioQueue stop run m_bQeueStart = %d\r\n", This->m_bQueueStart);
    }
}

void *audioPlayCallBack(unsigned char *pBuff, unsigned int uiLen, void *pContent)
{
	AudioPlay *This = (AudioPlay *)pContent;
    
	// 试用内存池
	if (This->m_bFirstQueueStart && !This->m_bQueueStart)
	{
		InitializeCriticalSection(&useMutext);
		InitializeCriticalSection(&unUseMutext);
		
		// 申请内存池
		for (int k=0; k<13; k++)
		{
			AudioData *pData = new AudioData;
			pData->uiLen = 0;
			pData->pBuffer = nil;
			vctUnuseList.push_back(pData);
		}
		vctUseList.clear();
		This->m_bFirstQueueStart = FALSE;
	}
	
	EnterCriticalSection(&unUseMutext);
	AudioData *pData = NULL;
	if (vctUnuseList.size() > 0)
	{
		pData = vctUnuseList[0];
		vctUnuseList.erase(vctUnuseList.begin());
	}
	LeaveCriticalSection(&unUseMutext);
	
	if (pData != NULL)
	{
		if (pData->pBuffer == NULL)
		{
			pData->pBuffer = new char[2880];
		}
		memset(pData->pBuffer, 0, 2880);
		memcpy(pData->pBuffer, pBuff, uiLen);
		pData->uiLen = uiLen;
		EnterCriticalSection(&useMutext);
		vctUseList.push_back(pData);
		LeaveCriticalSection(&useMutext);
		
		// audio queue中没有预存数据,AQOutCallBack不会被触发,所以m_bQueueStart为FALSE时
		// 无须锁
		if (!This->m_bQueueStart)
		{
            // 缓冲kNumberIAFrame帧, 防止AudioQueue频繁的stop和start
            int ns = vctUseList.size();
			if (vctUseList.size() == kNumberIAFrame)
			{
				// 预存到audio queue
				for (int k=0; k<kNumberBuffers; k++)
				{
					pData = vctUseList[k];
                    memset(This->mBuffers[k]->mAudioData, 0, This->mBuffers[k]->mAudioDataBytesCapacity);
					memcpy(This->mBuffers[k]->mAudioData, pData->pBuffer, pData->uiLen);
					This->mBuffers[k]->mAudioDataByteSize = pData->uiLen;
					AudioQueueEnqueueBuffer(This->mQueue, This->mBuffers[k], 0, NULL);
				}
                
				// 回收
				for (int k=0; k<kNumberBuffers; k++)
				{
					pData = vctUseList[k];
					vctUnuseList.push_back(pData);				
				}
				vctUseList.erase(vctUseList.begin(), vctUseList.begin()+3);
				
				// 数据已经准备好,可以启动audio queue了
                OSStatus status = AudioQueueStart(This->mQueue, NULL);
                if (status == noErr)
                {
                    This->m_bQueueStart = TRUE;
                }
			}
		}
	}
	
	return 0;
}

void AudioPlay::InitQueue()
{    
    AudioStreamBasicDescription mAudioFormat;
    memset(&mAudioFormat, 0, sizeof(mAudioFormat));
    mAudioFormat.mSampleRate = 8000;
    mAudioFormat.mChannelsPerFrame = 1;
    mAudioFormat.mFormatID = kAudioFormatLinearPCM;
    mAudioFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    mAudioFormat.mBitsPerChannel = 16;
    mAudioFormat.mBytesPerPacket = mAudioFormat.mBytesPerFrame = (mAudioFormat.mBitsPerChannel / 8) * mAudioFormat.mChannelsPerFrame;
    mAudioFormat.mFramesPerPacket = 1;
    
    AudioQueueNewOutput(&mAudioFormat, AQOutCallBack, this, 
                        CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &mQueue);
    const int bufferSize = 2048;
    for (int k=0; k<kNumberBuffers; k++)
    {
        AudioQueueAllocateBufferWithPacketDescriptions(mQueue, bufferSize, 0, &mBuffers[k]);
    }
    AudioQueueAddPropertyListener(mQueue, kAudioQueueProperty_IsRunning, IsRunningProc, this);		
    AudioQueueSetParameter(mQueue, kAudioQueueParam_Volume, 1.0);
    
    for (int i=0; i<3; i++)
    {
        AQOutCallBack(this, mQueue, mBuffers[i]);
    }
    AudioQueueStart(mQueue, NULL);
}

OSStatus AudioPlay::startQueue()
{
	OSStatus status = AudioQueueStart(this->mQueue, NULL);
	if (status == noErr)
	{
		this->m_bQueueStart = TRUE;
	}
	return status;
}

void AudioPlay::stopQueue()
{
	OSStatus status = AudioQueueStop(this->mQueue, TRUE);
	if (status == noErr)
	{
		this->m_bQueueStart = FALSE;
		this->m_bFirstQueueStart = TRUE;
	}
	
	if (this->mQueue)
	{
		AudioQueueDispose(this->mQueue, TRUE);
	}
	
	// 释放资源
	DeleteCriticalSection(&useMutext);
	for (int i=0; i<vctUseList.size(); i++)
	{
		AudioData *pData = vctUseList[i];
		delete  [] pData->pBuffer;
		delete pData;
	}
	vctUseList.clear();
	
	DeleteCriticalSection(&unUseMutext);
	for (int i=0; i<vctUnuseList.size(); i++)
	{
		AudioData *pData = vctUnuseList[i];
		delete [] pData->pBuffer;
		delete pData;
	}
	vctUnuseList.clear();
}
