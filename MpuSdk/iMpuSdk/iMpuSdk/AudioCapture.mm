//
//  AudioDataPort.m
//  iMpuSdk
//
//  Created by  on 12-12-3.
//  Copyright (c) 2012年 Crearo. All rights reserved.
//

#import "AudioCapture.h"
#import "AQRecord.h"
#include "DataType.h"
#include "platform.h"
#include "CR_AudioDecShell.h"

#define MaxUnuseFrameNum		11						// 空闲帧队列最大长度7
#define MaxUseFrameNum			(MaxUnuseFrameNum-2)	// 有用帧队列最大长度

// 视频实时预览接收缓存 帧包信息结构
typedef struct _TFramePacket
{
    u_int len;				// 当前帧数据的长度
    char * data;			// 帧数据
}TFramePacket;

// 帧包列表结构
std::vector<TFramePacket *> g_listAudioUse;			// 使用的帧队列
std::vector<TFramePacket *> g_listAudioUnuse;		// 未使用的帧队列		
CRITICAL_SECTION g_cslistAudioUse;
CRITICAL_SECTION g_cslistAudioUnuse;

static BOOL receiveRunning = FALSE;
unsigned int audioDecoder = 0;
pthread_t receiveThread = NULL;

const u_int receive_size = 1024 * 15;
void *DecodeAudioFunc(LPVOID pDecodeParam);

@interface AudioCapture()
{
    AQRecord *aq;   // 音频采集模块
    
    pthread_mutex_t m_mutex;
    pthread_t m_threadHandle;
}

- (void)receiveAudio;
@end

const int newSize = 1024;
// 回调函数在子线程中调用的
static void audioDataCallBack(char *pData, int nLen, void *pThis)
{
    AudioCapture *recorder = (AudioCapture *)pThis;
    DC7_SendRawFrame(recorder.dc7, pData, nLen, FRAME_AUDIO, 0);
}

// 音频接收
void *RecvAudioFunc(LPVOID pRecvDataParam)
{
    AudioCapture *recorder = (AudioCapture *)pRecvDataParam;
	int Ret;
	InitializeCriticalSection(&g_cslistAudioUnuse);
	for (int i=0; i<MaxUnuseFrameNum; i++)
	{
		TFramePacket *pFP = new TFramePacket;
		pFP->data = NULL;
		pFP->len = 0;
		g_listAudioUnuse.push_back(pFP);
	}
	// 初始化使用列表信息
	InitializeCriticalSection(&g_cslistAudioUse);
	g_listAudioUse.clear();
	
	audioDecoder = AudioDecShell_Create((char *)"");
	
	pthread_t hDecodeAudioThread = NULL;
	pthread_create((pthread_t *)&hDecodeAudioThread, NULL, DecodeAudioFunc, 0);
	while(receiveRunning)
	{
		// 读取视频帧
		TFramePacket *pFPRead = NULL;
		EnterCriticalSection(&g_cslistAudioUnuse);
		if (g_listAudioUnuse.size() == 0)
		{
			Sleep(20);
			LeaveCriticalSection(&g_cslistAudioUnuse);
			continue;
		}
		pFPRead = g_listAudioUnuse[0];
		g_listAudioUnuse.erase(g_listAudioUnuse.begin());
		LeaveCriticalSection(&g_cslistAudioUnuse);
		
		pFPRead->len = receive_size;
		if (pFPRead->data == NULL)
		{
			pFPRead->data = new char[receive_size];
		}
		ZeroMemory(pFPRead->data, receive_size);

        int frameType = 0;
        int keyFrame = 2;
        int nRet = DC7_RecvRawFrame(recorder.dc7, (char *)pFPRead->data, &pFPRead->len, &frameType, &keyFrame);
        
        if (nRet != DC7_E_OK) 
        {
            // 读取失败，重新放回未使用链表
            EnterCriticalSection(&g_cslistAudioUnuse);
            g_listAudioUnuse.push_back(pFPRead);
            LeaveCriticalSection(&g_cslistAudioUnuse);
            
            //没有取到数据需要等一下
            if (Ret == DC7_E_WOULDBLOCK)
            {
                Sleep(10);
            }
            else 
            {
                receiveRunning = false;
                
#ifdef _DEBUG
                printf("DCM_ReadFrame error: 0x%08X\n", Ret);
#endif
            }
            
            continue;
        }
        else 
        {
//                // 只有在获取有效的音频侦成功的时候才能保存, 之前一接受到就塞是不对的.因为如果readFrame的状态是正在接受,那么此时是不能录像的.
//                if (g_pCRMedia != NULL)
//                {
//                    if (g_pCRMedia->IsRecording())
//                    {
//                        TMediaData *media = new TMediaData;
//                        media->pszData = new unsigned char[150*1024];
//                        memset(media->pszData, 0, 150*1024);
//                        memcpy(media->pszData, pFPRead->data, pFPRead->len);
//                        media->uiLen = pFPRead->len;
//                        g_pCRMedia->SaveMediaData(media, CRMedia::IMT_IA);
//                    }
//                }
            
            EnterCriticalSection(&g_cslistAudioUse);
            g_listAudioUse.push_back(pFPRead);
            LeaveCriticalSection(&g_cslistAudioUse);
        }
	}
	pthread_join(hDecodeAudioThread, NULL);
    
	// 释放资源
	DeleteCriticalSection(&g_cslistAudioUnuse);
	for (int i=0; i<g_listAudioUnuse.size(); i++)
	{
		TFramePacket *pFP = g_listAudioUnuse[i];
		delete  [] pFP->data;
		delete pFP;
	}
	g_listAudioUnuse.clear();
	DeleteCriticalSection(&g_cslistAudioUse);
	for (int i=0; i<g_listAudioUse.size(); i++)
	{
		TFramePacket *pFP = g_listAudioUse[i];
		delete [] pFP->data;
		delete pFP;
	}
	g_listAudioUse.clear();
	return NULL;
}

// 音频解码
void *DecodeAudioFunc(LPVOID pDecodeParam)
{
	while(receiveRunning)
	{
		TFramePacket * pFPPump = NULL;
		EnterCriticalSection(&g_cslistAudioUse);
		if (g_listAudioUse.size() == 0)
		{
			Sleep(10);
			LeaveCriticalSection(&g_cslistAudioUse);
			continue;
		}
		pFPPump = g_listAudioUse[0];
		g_listAudioUse.erase(g_listAudioUse.begin()+0);
		LeaveCriticalSection(&g_cslistAudioUse);
		if (pFPPump == NULL)
		{	
			assert(false);
			Sleep(10);
			continue;
		}
		unsigned char OutBuf[100*1024];
		AUDIODEC_FRAME_PARAM DecFrmParam;
		DecFrmParam.pData = (unsigned char *)pFPPump->data+12;
		DecFrmParam.DataLen = pFPPump->len-12;
		DecFrmParam.PCMLen = 100*1024;
		DecFrmParam.pPCM = OutBuf;
		unsigned short producerID = 0;
		int nRet = AudioDecShell_DecFrm(audioDecoder, &producerID, &DecFrmParam);
		if (0 == nRet)
		{
			//g_recvAudio.audioPlayProc(DecFrmParam.pPCM, DecFrmParam.PCMLen, g_recvAudio.pAudioPlayContext);
#ifdef _DEBUG
			//printf("AudioDecShell_DecFrm sucessfull**********************: 0x%08X\n", nRet);
#endif			
		}
        
		EnterCriticalSection(&g_cslistAudioUnuse);
		g_listAudioUnuse.push_back(pFPPump);
		LeaveCriticalSection(&g_cslistAudioUnuse);
	}
	return NULL;
}

@implementation AudioCapture

- (id)init
{
    if (self = [super init])
    {
        aq = new AQRecord();
        NSAssert(aq != NULL, @"Creat the Recorder failed");
        pthread_mutex_init(&m_mutex, 0);
        receiveRunning = NO;
    }

    return self;
}


- (void)setMediaType:(MediaType)type
{
    [super setMediaType:type];
    
    // 对讲 采集音频的同时还需要播放来自端口另一端的音频
    if (aq != NULL)
    {
        // 设置音频采集回调
        aq->SetAudioCallBack(audioDataCallBack, self);
        
        // 如果是对讲,还需要不停去接收音频
        if (type == MediaTypeTalk)
        {
            [self receiveAudio];
        }
    }
}

- (void)startRecord
{
    if (aq != NULL)
    {
        aq->StartRecord();
    }
}

- (void)receiveAudio
{
    receiveRunning = YES;
    pthread_create((pthread_t *)&receiveThread, NULL, RecvAudioFunc, self);
}

- (void)dealloc
{
    pthread_mutex_destroy(&m_mutex);
    delete aq;
    aq = NULL;
    [super dealloc];
}
@end
