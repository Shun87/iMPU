/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: StreamParser.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-28 09:30:49
 *  修正备注: 
 *  
 */

#ifndef __STREAMPARSER_H__
#define __STREAMPARSER_H__
typedef struct _TStreamParser TStreamParser;
struct _TStreamParser  
{
//public:
	void (*Construct)(TStreamParser *pStreamParser);
	void (*Destroy)(TStreamParser *pStreamParser);
	XOS_BOOL (*Create)(TStreamParser *pStreamParser, const void *p, int iSize, XOS_BOOL bNBO);

	XOS_BOOL (*IsEnd)(TStreamParser *pStreamParser);
	char* (*GetCurPos)(TStreamParser *pStreamParser);
	int (*GetLeftLen)(TStreamParser *pStreamParser);
	XOS_BOOL (*SkipLength)(TStreamParser *pStreamParser, int iSize);
	XOS_BOOL (*SkipPAD2)(TStreamParser *pStreamParser);
	XOS_BOOL (*SkipPAD4)(TStreamParser *pStreamParser);
	XOS_BOOL (*SkipPAD8)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetCHAR)(TStreamParser *pStreamParser, XOS_S8 *pc);
	XOS_BOOL (*SkipCHAR)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetUCHAR)(TStreamParser *pStreamParser, XOS_U8 *puc);
	XOS_BOOL (*SkipUCHAR)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetSHORT)(TStreamParser *pStreamParser, XOS_S16 *ps);
	XOS_BOOL (*SkipSHORT)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetUSHORT)(TStreamParser *pStreamParser, XOS_U16 *pus);
	XOS_BOOL (*SkipUSHORT)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetINT)(TStreamParser *pStreamParser, XOS_S32 *pi);
	XOS_BOOL (*SkipINT)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetUINT)(TStreamParser *pStreamParser, XOS_U32 *pui);
	XOS_BOOL (*SkipUINT)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetIP)(TStreamParser *pStreamParser, XOS_U32 *pip);
	XOS_BOOL (*SkipIP)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetGUID)(TStreamParser *pStreamParser, TCGUID *pguid);
	TCGUID* (*SkipGUID)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetString)(TStreamParser *pStreamParser, char *lpsz, int len);
	char* (*SkipString)(TStreamParser *pStreamParser);
	XOS_BOOL (*GetBuffer)(TStreamParser *pStreamParser, char *p, int len);
	char* (*SkipBuffer)(TStreamParser *pStreamParser, int len);

//private:
	XOS_BOOL (*BeforeDecSize)(TStreamParser *pStreamParser, int iSize);
	XOS_BOOL (*DecSize)(TStreamParser *pStreamParser, int iSize);

	char *m_pBuf;
	int m_iOffset;
	int m_iSize;
	XOS_BOOL bNBO;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const TStreamParser STREAMPARSER;

#ifdef __cplusplus
}
#endif

#endif //__STREAMPARSER_H__
