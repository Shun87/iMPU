/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */
/*
    File:       OSQueue.cpp

    Contains:   implements OSQueue class
                    

*/

#include "OSQueue.h"


OSQueue::OSQueue() : fLength(0)
{
    fSentinel.fNext = &fSentinel;
    fSentinel.fPrev = &fSentinel;
}

void OSQueue::EnQueue(OSQueueElem* elem)
{
    Assert(elem != NULL);
    if (elem->fQueue == this)
        return;
    Assert(elem->fQueue == NULL);
    elem->fNext = fSentinel.fNext;
    elem->fPrev = &fSentinel;
    elem->fQueue = this;
    fSentinel.fNext->fPrev = elem;
    fSentinel.fNext = elem;
    fLength++;
}

OSQueueElem* OSQueue::DeQueue()
{
    if (fLength > 0)
    {
        OSQueueElem* elem = fSentinel.fPrev;
        Assert(fSentinel.fPrev != &fSentinel);
        elem->fPrev->fNext = &fSentinel;
        fSentinel.fPrev = elem->fPrev;
        elem->fQueue = NULL;
        fLength--;
        return elem;
    }
    else
        return NULL;
}

void OSQueue::Remove(OSQueueElem* elem)
{
    Assert(elem != NULL);
    Assert(elem != &fSentinel);
    
    if (elem->fQueue == this)
    {
        elem->fNext->fPrev = elem->fPrev;
        elem->fPrev->fNext = elem->fNext;
        elem->fQueue = NULL;
        fLength--;
    }
}



void OSQueueIter::Next()
{
    if (fCurrentElemP == fQueueP->GetTail())
        fCurrentElemP = NULL;
    else
        fCurrentElemP = fCurrentElemP->Prev();
}

