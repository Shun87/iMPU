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
    File:       StrPtrLen.cpp

    Contains:   Implementation of class defined in StrPtrLen.h.  
                    

    

*/


#include <ctype.h>
#include "StrPtrLen.h"
#include "MyAssert.h"

#include <stdio.h>
#include "OSMemory.h"

#define qtss_printf printf

UInt8       StrPtrLen::sCaseInsensitiveMask[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, //0-9 
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, //10-19 
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, //20-29
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, //30-39 
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, //40-49
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59, //50-59
    60, 61, 62, 63, 64, 97, 98, 99, 100, 101, //60-69 //stop on every character except a letter
    102, 103, 104, 105, 106, 107, 108, 109, 110, 111, //70-79
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, //80-89
    122, 91, 92, 93, 94, 95, 96, 97, 98, 99, //90-99
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, //100-109
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, //110-119
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129 //120-129
};

UInt8 StrPtrLen::sNonPrintChars[] =
{
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, //0-9     // stop
    0, 1, 1, 0, 1, 1, 1, 1, 1, 1, //10-19    //'\r' & '\n' are not stop conditions
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //20-29
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, //30-39   
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //40-49
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //50-59
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //60-69  
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //70-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80-89
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //110-119
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //120-129
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //130-139
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //140-149
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //150-159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160-169
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, //170-179
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //180-189
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //190-199
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //200-209
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //210-219
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //220-229
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //230-239
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //240-249
    1, 1, 1, 1, 1, 1             //250-255
};

char* StrPtrLen::GetAsCString() const
{
    // convert to a "NEW'd" zero terminated char array
    // caler is reponsible for the newly allocated memory
    char *theString = NEW char[Len+1];
    
    if ( Ptr && Len > 0 )
        ::memcpy( theString, Ptr, Len );
    
    theString[Len] = 0;
    
    return theString;
}


Bool16 StrPtrLen::Equal(const StrPtrLen &compare) const
{
    if (NULL == compare.Ptr && NULL == Ptr )
        return true;
        
        if ((NULL == compare.Ptr) || (NULL == Ptr))
        return false;

    if ((compare.Len == Len) && (memcmp(compare.Ptr, Ptr, Len) == 0))
        return true;
    else
        return false;
}

Bool16 StrPtrLen::Equal(const char* compare) const
{   
    if (NULL == compare && NULL == Ptr )
        return true;
        
    if ((NULL == compare) || (NULL == Ptr))
        return false;
        
    if ((::strlen(compare) == Len) && (memcmp(compare, Ptr, Len) == 0))
        return true;
    else
        return false;
}



Bool16 StrPtrLen::NumEqualIgnoreCase(const char* compare, const UInt32 len) const
{
    // compare thru the first "len: bytes
    Assert(compare != NULL);
    
    if (len <= Len)
    {
        for (UInt32 x = 0; x < len; x++)
            if (sCaseInsensitiveMask[Ptr[x]] != sCaseInsensitiveMask[compare[x]])
                return false;
        return true;
    }
    return false;
}

Bool16 StrPtrLen::EqualIgnoreCase(const char* compare, const UInt32 len) const
{
    Assert(compare != NULL);
    if (len == Len)
    {
        for (UInt32 x = 0; x < len; x++)
            if (sCaseInsensitiveMask[Ptr[x]] != sCaseInsensitiveMask[compare[x]])
                return false;
        return true;
    }
    return false;
}

UInt32 StrPtrLen::RemoveWhitespace()
{
    if (Ptr == NULL || Len == 0)
        return 0;

    char *EndPtr = Ptr + Len; // one past last char
    char *destPtr = Ptr;
    char *srcPtr = Ptr;
    
    Len = 0;
    while (srcPtr < EndPtr)
    {
        
        if (*srcPtr != ' ' && *srcPtr != '\t')
        {    
            if (srcPtr != destPtr)
               *destPtr = *srcPtr;

             destPtr++;
             Len ++;
        }
        srcPtr ++;
    }

    return Len;
}

UInt32 StrPtrLen::TrimLeadingWhitespace()
{
    if (Ptr == NULL || Len == 0)
        return 0;

    char *EndPtr = Ptr + Len; //one past last char

    while (Ptr < EndPtr)
    {
        if (*Ptr != ' ' && *Ptr != '\t')
            break;
        
        Ptr += 1;
        Len -= 1; 
    }

    return Len;
}

UInt32 StrPtrLen::TrimTrailingWhitespace()
{
    if (Ptr == NULL || Len == 0)
        return 0;

    char *theCharPtr = Ptr + (Len - 1); // last char

    while (theCharPtr >= Ptr)
    {
        if (*theCharPtr != ' ' && *theCharPtr != '\t')
            break;
        
        theCharPtr -= 1;
        Len -= 1; 
    }
    
    return Len;
}

void StrPtrLen::PrintStr()
{
    char *thestr = GetAsCString();
    
    UInt32 i = 0;
    for (; i < Len; i ++) 
    { 
       if (StrPtrLen::sNonPrintChars[Ptr[i]]) 
       {   thestr[i] = 0;
           break;
       }
       
    } 
       
    if (thestr != NULL)
    {   
        qtss_printf(thestr);
        delete thestr;
    }   
}

void StrPtrLen::PrintStr(char *appendStr)
{
    StrPtrLen::PrintStr();
    if (appendStr != NULL)
        qtss_printf(appendStr);
}

void StrPtrLen::PrintStrEOL(char* stopStr, char *appendStr)
{
           
 
    char *thestr = GetAsCString();
    
    SInt32 i = 0;
    for (; i < (SInt32) Len; i ++) 
    { 
       if (StrPtrLen::sNonPrintChars[Ptr[i]]) 
       {   thestr[i] = 0;
           break;
       }
       
    } 

    for (i = 0; thestr[i] != 0 ; i ++) 
    { 
       if (thestr[i] == '%' && thestr[i+1] != '%' ) 
       {   thestr[i] = '$';
       }       
    } 

    SInt32 stopLen = 0;
    if (stopStr != NULL)
        stopLen = ::strlen(stopStr);

    if (stopLen > 0 && stopLen <= i)
    {
        char* stopPtr = ::strstr(thestr, stopStr);
        if (stopPtr != NULL)
        {   stopPtr +=  stopLen;
           *stopPtr = 0;
           i = stopPtr - thestr;
        }
    }

    char * theStrLine = thestr;
    char * nextLine = NULL;
    char * theChar = NULL;
    static char *cr="\\r";
    static char *lf="\\n\n";
    SInt32 tempLen = i;
    for (i = 0; i < tempLen; i ++) 
    {   
        if (theStrLine[i] == '\r')
        {   theChar = cr;
            theStrLine[i] = 0;
            nextLine = &theStrLine[i+1];
        }
        else if (theStrLine[i] == '\n')
        {   theChar = lf;
            theStrLine[i] = 0;
            nextLine = &theStrLine[i+1];
        }
        
        if (nextLine != NULL)
        { 
            qtss_printf(theStrLine);
            qtss_printf(theChar);
            
            theStrLine = nextLine;
            nextLine = NULL;  
            tempLen -= (i+1);
            i = -1;
        }
    }
    qtss_printf(theStrLine);
    delete thestr;

    if (appendStr != NULL)
        qtss_printf(appendStr);
   
}


