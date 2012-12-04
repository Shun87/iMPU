/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOS.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2009-3-30 20:56:09
 *  ������ע: 
 *  
 */

#include "../XOS.h"

//���������ʵ����XOSThread_XXX.c��
//void XOS_GetCPURate(char CPURate[7]);

XOS_U8 XOS_GetMemRate()
{
#ifdef WIN32
	return 40;
#elif _CREARO_NDK
	return 40;
#else //LINUX
	FILE *fp;
	int Free;
	int Total;
	char text[201];
	char MemFree[21];
	char MemTotal[21];
	XOS_U8 MemUseRate;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL)
	{
		return 100;
	}
	memset(text, 0, sizeof(text));
	while (fgets(text, sizeof(text)-1, fp))
	{
		if (strstr(text, "MemTotal:"))
		{
			sscanf(text, "%s %d", MemTotal, &Total);
		}
		if (strstr(text, "MemFree:"))
		{
			sscanf(text, "%s %d", MemFree, &Free);
		}
	}
	fclose(fp);
	
	MemUseRate = 100 - (100*Free)/Total;
	return MemUseRate;
#endif //WIN32	
}

void XOS_GetLocalTime(struct tm *pst)
{
	time_t tt;
	tt = time(NULL);
	*pst = *localtime(&tt);
}


//									1	2	3	4	5	6	7	8	9  10  11  12		
static const int sc_MaxDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int IsLeapYear(int iYear)
{
	return ((((iYear % 4) == 0) && ((iYear % 100) != 0)) || 
		    (((iYear % 4) == 0) && ((iYear % 400) == 0)));
}

static int GetMaxDayByMonth(int iYear, int iMonth)
{
	int iMaxDay;
	iMaxDay = sc_MaxDays[iMonth];
	if ((iMonth == 1) && IsLeapYear(iYear)) {
		iMaxDay ++;
	}
	return iMaxDay;
}
XOS_BOOL XOS_SetLocalTime(struct tm *pst)
{
#ifdef WIN32
	printf("XOS_SetLocalTime %02d-%02d-%02d %02d:%02d:%02d %d.\r\n",
				pst->tm_year+1900-2000, pst->tm_mon+1, pst->tm_mday,	
				pst->tm_hour, pst->tm_min, pst->tm_sec, pst->tm_wday);
	return XOS_TRUE;
#elif _CREARO_NDK
	printf("XOS_SetLocalTime %02d-%02d-%02d %02d:%02d:%02d %d.\r\n",
				pst->tm_year+1900-2000, pst->tm_mon+1, pst->tm_mday,	
				pst->tm_hour, pst->tm_min, pst->tm_sec, pst->tm_wday);
	return XOS_TRUE;
#else //LINUX
	char szCommand[128] = "";
	char fCommand[] = "date -s %02d%02d%02d%02d%04d.%02d";

	printf("XOS_SetLocalTime %04d-%02d-%02d %02d:%02d:%02d %d.\r\n",
				pst->tm_year+1900, pst->tm_mon+1, pst->tm_mday,	
				pst->tm_hour, pst->tm_min, pst->tm_sec, pst->tm_wday);
	//�Ϸ��Լ��
	if ((pst->tm_year > 199) ||
		(pst->tm_year < 100) ||
		(pst->tm_mon > 11) ||
		(pst->tm_mon < 0) ||
		(pst->tm_mday > GetMaxDayByMonth(pst->tm_year, pst->tm_mon)) ||
		(pst->tm_mday < 1) ||
		(pst->tm_wday > 6) ||
		(pst->tm_wday < 0) ||
		(pst->tm_hour > 23) ||
		(pst->tm_hour < 0) ||
		(pst->tm_min > 59) ||
		(pst->tm_min < 0) ||
		(pst->tm_sec > 59) ||
		(pst->tm_sec < 0))
	{
		printf("XOS_SetLocalTime invalid time\r\n");
		return XOS_FALSE;
	}
	
	//����ϵͳʱ��,��ʽ	date -s 112318062006.56	
	sprintf(szCommand, fCommand, 
			pst->tm_mon+1, pst->tm_mday, pst->tm_hour, pst->tm_min, 
			pst->tm_year+1900, pst->tm_sec);
	printf("XOS_SetLocalTime %s\r\n", szCommand);
	system(szCommand);
	return XOS_TRUE;
#endif //WIN32	
}
