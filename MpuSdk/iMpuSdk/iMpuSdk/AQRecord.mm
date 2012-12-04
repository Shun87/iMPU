//
//  AQRecord.mm
//  iMobileMonitor_Device
//
//  Created by  on 12-6-20.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#include "AQRecord.h"
#include <stdio.h>
#include "AudioEncoder.h"

typedef struct
{
    unsigned char algID;
    unsigned char rsv[3];
    unsigned char data[48];
}AUDIO_ALG_DATA;

typedef struct
{
    unsigned int uiUTC;
    unsigned int uiTimeStamp;
    unsigned char ucKeyFrm;
    unsigned char ucRsv[3];
    
    unsigned short BlockAlign;
	unsigned char Channels;
	unsigned char BitsPerSample;
	unsigned short SamplesPerSec;
	unsigned short AlgFrmNum;
	unsigned short ProducerID;
	unsigned short PCMLen;
	unsigned int Rsv1;
    AUDIO_ALG_DATA algData[9];
}AUDIO_DATA_HEAD;

// ____________________________________________________________________________________
// AudioQueue callback function, called when an input buffers has been filled.
void AQRecord::AudioInputBufferHandler(void *                               inUserData,
                                       AudioQueueRef						inAQ,
                                       AudioQueueBufferRef					inBuffer,
                                       const AudioTimeStamp *				inStartTime,
                                       UInt32								inNumPackets,
                                       const AudioStreamPacketDescription*	inPacketDesc)
{
    AQRecord *aqr = (AQRecord *)inUserData;
    
    SInt16 *pPCM = (SInt16 *)inBuffer->mAudioData;
    aqr->EncodePCMData(pPCM, inBuffer->mAudioDataByteSize);
    
    if (aqr->IsRunning())
    {
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}

void *AQRecord::ThreadFunc(void *inUserData)
{
    AQRecord *aqr = (AQRecord *)inUserData;

    aqr->BuildAudioFrame();

    return 0;
}

AQRecord::AQRecord() 
: m_bRecording(false)
, m_hEncoder(NULL)
,m_nRemainLen(0)
,m_pszRemain(NULL)
,m_bBuilding(true)
,m_bTalk(true)
{    
    m_hEncoder = EncoderCreat(ADPCM);
    assert(m_hEncoder != NULL);
    
    m_pszRemain = new short[1024];
    assert(m_pszRemain != NULL);
    
    pthread_mutex_init(&m_mutex, 0);
}

AQRecord::~AQRecord()
{
    pthread_mutex_destroy(&m_mutex);
    
    if (m_pszRemain != NULL)
    {
        delete [] m_pszRemain;
        m_pszRemain = NULL;
    }
    
    if (m_hEncoder != NULL)
    {
        EncoderDelete(m_hEncoder);
        m_hEncoder = NULL;
    }
}

void AQRecord::StartRecord()
{
    // 
    InitAudioQueue();
    m_bBuilding = true;
    m_bRecording = true;
    AudioQueueStart(m_queue, NULL);
    
    pthread_create((pthread_t *)&m_threadHandle, NULL, &ThreadFunc, this);
}

void AQRecord::StopRecord()
{
    m_bBuilding = false;
    pthread_join(m_threadHandle, NULL);
    m_threadHandle = NULL;
    m_bRecording = false;
    AudioQueueStop(m_queue, true);
    AudioQueueDispose(m_queue, true);
    
    for (int i=0; i<m_vctEncodedData.size(); i++)
    {
        UCHAR *pPCM = m_vctEncodedData[i];
        delete [] pPCM;
    }
    m_vctEncodedData.clear();
    
    m_nRemainLen = 0;
}

void AQRecord::EncodePCMData(short pszData[], int nPCMLen)
{
    int nTotal = nPCMLen + m_nRemainLen;
    int nCount = nTotal / 160;
    int nRemain = nTotal % 160;
    short pszTotalData[nTotal];
    short *pPCM = pszTotalData;
    memset(pPCM, 0, nTotal);
//    
    if (m_nRemainLen > 0)
    {
        memcpy(pPCM, m_pszRemain, m_nRemainLen);
        pPCM += m_nRemainLen;
    }
    memcpy(pPCM, pszData, nPCMLen);
//    
    short *pTemp = pszTotalData;
    pthread_mutex_lock(&m_mutex);
    for (int i=0; i<nCount; i++)
    {
        int pMaxLen = HIS_ADPCM_FRM_LEN;
        UCHAR *pStream = new UCHAR[pMaxLen];
        Encode(m_hEncoder, pTemp, HIS_ADPCM_SAMPLE_NUM, pStream, &pMaxLen);
        
        // adpcm 帧
        m_vctEncodedData.push_back(pStream);
        
        pTemp += 80;
        
        if (i == nCount - 1)
        {
            CopyRemainData(pTemp, nRemain);
        }
    }
//
    pthread_mutex_unlock(&m_mutex);
}

void AQRecord::BuildAudioFrame()
{
    while (m_bBuilding)
    {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        pthread_mutex_lock(&m_mutex);
        int nSize = m_vctEncodedData.size();
        if (nSize >= 9)
        {
            AUDIO_DATA_HEAD audio;
            time_t time = (time_t)[[NSDate date] timeIntervalSince1970];
            audio.uiUTC = time / 1000;
            audio.uiTimeStamp = time;
            audio.ucKeyFrm = 1;
            audio.ucRsv[0] = 0;
            audio.ucRsv[1] = 0;
            audio.ucRsv[2] = 0;
            audio.BlockAlign = 52;
            audio.Channels = 1;
            audio.BitsPerSample = 16;
            audio.SamplesPerSec = 80;
            audio.AlgFrmNum = 9;
            audio.ProducerID = 1;
            audio.PCMLen = 160;
            audio.Rsv1 = 0;
            for (int i=0; i<9; i++)
            {
                audio.algData[i].algID = 5;
                audio.algData[i].rsv[0] = 0;
                audio.algData[i].rsv[1] = 0;
                audio.algData[i].rsv[2] = 0;
                memset(audio.algData[i].data, 0, 48);
                
                UCHAR *pEncoded = m_vctEncodedData[i];
                memcpy(audio.algData[i].data, pEncoded, 48);
            }
                    
            for (int i=0; i<9; i++)
            {
                UCHAR *pPCM = m_vctEncodedData[i];
                delete [] pPCM;
            }
            
            m_vctEncodedData.erase(m_vctEncodedData.begin(), m_vctEncodedData.begin()+9);
            pthread_mutex_unlock(&m_mutex);

            if (m_DataCallBack != NULL)
            {
                m_DataCallBack((char *)&audio, 496, m_pContext);
            }
        }
        else
        {
            pthread_mutex_unlock(&m_mutex);
        }
        
        [pool release];
    }
}

void AQRecord::InitAudioQueue()
{
    AudioStreamBasicDescription audioFormat;
    memset(&audioFormat, 0, sizeof(audioFormat));
    audioFormat.mSampleRate = 8000.0;
    audioFormat.mChannelsPerFrame = 1;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags =kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    audioFormat.mBitsPerChannel = 16;
    audioFormat.mBytesPerPacket = audioFormat.mBytesPerFrame = (audioFormat.mBitsPerChannel / 8) * audioFormat.mChannelsPerFrame;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mReserved = 0;
    AudioQueueNewInput(&audioFormat,
                       AudioInputBufferHandler, 
                       this, NULL, NULL, 0, &m_queue);
    
    const int bufferSize = 2048;
    for (int k=0; k<3; k++)
    {
        AudioQueueAllocateBufferWithPacketDescriptions(m_queue, bufferSize, 0, &m_buffers[k]);
        AudioQueueEnqueueBuffer(m_queue, m_buffers[k], 0, NULL);
    }
    
    AudioQueueSetParameter(m_queue, kAudioQueueParam_Volume, 1.0);
}

void AQRecord::CopyRemainData(short *pszData, int nUnEncodeLen)
{
    // 拷贝上一次编码剩下的数据
    m_nRemainLen = nUnEncodeLen;
    memset(m_pszRemain, 0, 500);
    memcpy(m_pszRemain, pszData, nUnEncodeLen);
}
