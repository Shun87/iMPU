//
//  AQRecord.h
//  iMobileMonitor_Device
//
//  Created by  on 12-6-20.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//
#include <Foundation/Foundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include "DataType.h"

// 得到声音数据后的回调函数 注意:此回调函数在子线程中完成的

typedef void(*DATA_RECORD_CALLBACK)(char *pData, int nLen, void *pContext);

// 采集声音
class AQRecord
{
public:
    AQRecord();
    ~AQRecord();
    
    void StartRecord();
    void StopRecord();
    BOOL IsRunning() const { return m_bRecording; }
    void CopyRemainData(short *pszData, int nUnEncodeLen);

    void SetRecrodMode(bool bTalk) { m_bTalk = bTalk; }
    void SetAudioCallBack(DATA_RECORD_CALLBACK function, void *pContext)
    {
        m_DataCallBack = function; m_pContext = pContext;
    }
private:
    BOOL m_bRecording;
    BOOL m_bBuilding;
    AudioQueueRef				m_queue;
    AudioQueueBufferRef			m_buffers[3];
    short *m_pszRemain;
    int m_nRemainLen;
    HANDLE m_hEncoder;
    std::vector<UCHAR *> m_vctEncodedData;
    pthread_mutex_t m_mutex;
    pthread_t m_threadHandle;
    bool m_bTalk;   // 是否是对讲
    typedef struct
    {
        UCHAR *pStream;
        unsigned int uiTimeStamp;
    }pstream_info;

    void InitAudioQueue();
    
    void EncodePCMData(short pszData[], int nPCMLen);
    
    void BuildAudioFrame();
    
    static void AudioInputBufferHandler(void *                              inUserData,
                                        AudioQueueRef						inAQ,
                                        AudioQueueBufferRef					inBuffer,
                                        const AudioTimeStamp *				inStartTime,
                                        UInt32								inNumPackets,
                                        const AudioStreamPacketDescription*	inPacketDesc);
    static void *ThreadFunc(void *inUserData);
    
    DATA_RECORD_CALLBACK m_DataCallBack;
    void *m_pContext;

};