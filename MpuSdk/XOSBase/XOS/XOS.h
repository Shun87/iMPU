/*  
 *  Copyright (c) 2005, ∞≤ª’¥¥ ¿ø∆ºº”–œﬁπ´Àæ
 *  All rights reserved.
 *  
 *  Œƒº˛√˚≥∆: XOS.h
 *  Œƒº˛±Í ∂: 
 *  ’™    “™: XOS÷ß≥÷Õ∑Œƒº˛, π”√XOS÷ß≥÷µƒ∞¸∫¨¥ÀÕ¨Œƒº˛º¥ø….
 *				XOS÷ß≥÷∞¸¿®»Áœ¬∑Ω√Ê.
 *					ª˘±æ¿‡–Õ
 *					ƒ⁄¥Ê∑÷≈‰
 *					∂‡œﬂ≥Ã÷ß≥÷
 *					Õ¯¬Á–≠“È’ª
 *  µ±«∞∞Ê±æ: 
 *  ◊˜    ’ﬂ: Tommy
 *  ÕÍ≥…»’∆⁄: 2008-10-14 15:26:18
 *  –ﬁ’˝±∏◊¢: 
 *  
 */

#ifndef __XOS_H__
#define __XOS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef WINCE
#define assert(b)
#define WIN32
#include "OS_WindowsCE.h"
#else
#include <assert.h>
#endif

/*
 * »Áπ˚“™◊‘∂®“Âprintf ‰≥ˆ,«Î∑≈ø™’‚∏ˆ∫Í,≤¢«“◊‘º∫ µœ÷xprintf∫Ø ˝,Ω”ø⁄”ÎprintfÕÍ»´Õ¨
 */

//ª˘±æ¿‡–Õ
#include "XOSType.h"
//ƒ⁄¥Ê∑÷≈‰
#include "XOSMemory.h"
//∂‡œﬂ≥Ã÷ß≥÷
#include "XOSThread.h"
//Õ¯¬Á–≠“È’ª
#include "XOSStack.h"
//TCSƒ£øÈ
#include "TCS.h"
//WDƒ£øÈ
#include "WD.h"

#ifndef _CREARO_NDK
//Œƒº˛≤Ÿ◊˜ƒ£øÈ,CREARO_NKD≤ª÷ß≥÷
#include "XOSFile.h"
#endif

#include "XOSLog.h"

/*  π¶ƒ‹√Ë ˆ: ªÒ»°…Ë±∏CPU π”√¬ 
 *  ≤Œ ˝Àµ√˜: char CPURate[7]:
 CPURate[0]: //”√ªßÃ¨µƒ’º”√ ±º‰∞Ÿ∑÷±»
 CPURate[1]: //µÕ”≈œ»º∂µƒ”√ªßƒ£ Ω’º”√ ±º‰∞Ÿ∑÷±»
 CPURate[2]: //ƒ⁄∫À’º”√ ±º‰∞Ÿ∑÷±»
 CPURate[3]: //ø’œ–µƒ¥¶¿Ì∆˜’º”√ ±º‰∞Ÿ∑÷±»(≥˝”≤≈ÃIOµ»¥˝ ±º‰“‘Õ‚∆‰À¸µ»¥˝ ±º‰)
 CPURate[4]: //”≤≈ÃIOµ»¥˝’º”√ ±º‰∞Ÿ∑÷±»
 CPURate[5]: //”≤÷–∂œ’º”√ ±º‰∞Ÿ∑÷±»
 CPURate[6]: //»Ì÷–∂œ’º”√ ±º‰∞Ÿ∑÷±»
 */
void XOS_GetCPURate(char CPURate[7]);

/*  π¶ƒ‹√Ë ˆ: ªÒ»°…Ë±∏ƒ⁄¥Ê π”√¬ 
 *	∑µªÿ÷µ:	∞Ÿ∑÷±»,0-100
 */
XOS_U8 XOS_GetMemRate();

/*  π¶ƒ‹√Ë ˆ: ªÒ»°œµÕ≥ ±º‰
 *  ≤Œ ˝Àµ√˜:
 *      struct tm *pst [OUT]: ±Í◊ºC ±º‰Ω·ππ,»°÷µ∑∂Œß»Áœ¬:
 *								tm_sec	Seconds after minute (0 ®C 59)
 *								tm_min	Minutes after hour (0 ®C 59)
 *								tm_hour	Hours after midnight (0 ®C 23)
 *								tm_mday	Day of month (1 ®C 31)
 *								tm_mon	Month (0 ®C 11; January = 0)
 *								tm_year	Year (current year minus 1900)
 *								tm_wday	Day of week (0 ®C 6; Sunday = 0)
 *								tm_yday	Day of year (0 ®C 365; January 1 = 0)
 */
void XOS_GetLocalTime(struct tm *pst);

/*  π¶ƒ‹√Ë ˆ: …Ë÷√œµÕ≥ ±º‰
 *  ≤Œ ˝Àµ√˜:
 *      struct tm *pst []:
 *	∑µªÿ÷µ:	…Ë÷√ «∑Ò≥…π¶,“ª∞„∑µªÿ≥…π¶
 *  ±∏◊¢: œµÕ≥‘⁄∆Ù∂Øµƒ ±∫Ú¥”RTCªÒ»° ±º‰∫Û,µ˜”√¥À∫Ø ˝…Ë÷√œµÕ≥ ±º‰
 *		  –ﬁ∏ƒœµÕ≥ ±º‰µƒ ±∫Ú,œ»–ﬁ∏ƒRTC ±º‰,»ª∫Û‘Ÿµ˜”√¥À∫Ø ˝–ﬁ∏ƒœµÕ≥ ±º‰
 *		  “ÚŒ™–ﬁ∏ƒRTC ±º‰≥…π¶¡À,øœ∂® «∫œ∑® ±º‰,À˘“‘’‚∏ˆ∫Ø ˝◊‹ «∑µªÿ≥…π¶
 */
XOS_BOOL XOS_SetLocalTime(struct tm *pst);

#endif //__XOS_H__
