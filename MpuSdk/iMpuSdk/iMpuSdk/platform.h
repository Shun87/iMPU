/*
 *  platform.h
 *  NC
 *
 *  Created by crearo on 1/4/10.
 *  Copyright 2010 crearo. All rights reserved.
 *
 */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <pthread.h>
#include <vector>

#define CR_ASSERT assert

#ifndef WIN32
#define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory
#define SecureZeroMemory RtlSecureZeroMemory
#define CaptureStackBackTrace RtlCaptureStackBackTrace
#endif

#define FAR
#define NEAR 
#define BI_RGB 0
#define DIB_MARKER   ((WORD) ('M' << 8) | 'B')

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket		close
#define OPM_FD_SET(fd, set) FD_SET(fd, set)
#define INFINITE 0XFFFFFFFFL

typedef unsigned short WORD;
typedef void *LPVOID;
typedef unsigned long DWORD;
typedef pthread_mutex_t CRITICAL_SECTION;
typedef unsigned int UINT;

static void Sleep(unsigned long t)
{
	usleep(1000*t);
}

static int InitializeCriticalSection(CRITICAL_SECTION *pcs)
{
	// 原先的循环锁如何设置？ulSpinCount暂时不考虑
	return pthread_mutex_init(pcs, 0);
}

static int InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION *pcs,DWORD dwSpinCount)
{
	// 原先的循环锁如何设置？ulSpinCount暂时不考虑
	return pthread_mutex_init(pcs, 0);
}

static int DeleteCriticalSection(CRITICAL_SECTION *pcs)
{
	return pthread_mutex_destroy(pcs);
}

static int EnterCriticalSection(CRITICAL_SECTION *pcs)
{
	return pthread_mutex_lock(pcs);
}

static int LeaveCriticalSection(CRITICAL_SECTION *pcs)
{
	return pthread_mutex_unlock(pcs);
}

//////////////////////////////////////////////////////////////////////////////////////////


#endif /* __PLATFORM_H__*/