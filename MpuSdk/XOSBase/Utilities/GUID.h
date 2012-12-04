/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: GUID.h
 *  文件标识: 
 *  摘    要: GUID的类型定义以及相关的函数
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-09-02 16:28:11
 *  修正备注: 
 *  
 */

#ifndef __GUID_H__
#define __GUID_H__

#ifndef __TCGUID__
#define __TCGUID__

	typedef struct _TCGUID {
		union {
			XOS_U8 GUID[16];
			struct {
				XOS_U8 PlatFormType;
				XOS_U8 ResourceType;
				XOS_U16 SubResourceType;
				XOS_U32 ID;
				XOS_U16 Reserved;
				XOS_U8 MAC[6];
			} sGUID;
		} uGUID;
	} TCGUID;
	
	#define GUID_PLATFORMTYPE_STATION				1
	#define GUID_PLATFORMTYPE_CES					2
	#define GUID_PLATFORMTYPE_USER					10
	#define GUID_PLATFORMTYPE_CONFIGER				11
	#define GUID_PLATFORMTYPE_RESOURCEUSER			12
	#define GUID_PLATFORMTYPE_REPLAYER				13
	
	#define GUID_RESOURCETYPE_SELF					0
	#define GUID_RESOURCETYPE_INPUTVIDEO			1
	#define GUID_RESOURCETYPE_INPUTAUDIO			2
	#define GUID_RESOURCETYPE_OUTPUTVIDEO			3
	#define GUID_RESOURCETYPE_OUTPUTAUDIO			4
	#define GUID_RESOURCETYPE_SERIALPORT			5
	#define GUID_RESOURCETYPE_INPUTDIGITALLINE		6
	#define GUID_RESOURCETYPE_OUTPUTDIGITALLINE		7
	#define GUID_RESOURCETYPE_DISPLAYER				8
	#define GUID_RESOURCETYPE_SPEAKER				9
	#define GUID_RESOURCETYPE_STORAGER				10
	#define GUID_RESOURCETYPE_ARCHIVER				11
	#define GUID_RESOURCETYPE_PLATFORMMANAGER		12
	#define GUID_RESOURCETYPE_DEPOSITER				21
	
	#define GUID_SUBRESOURCETYPE_BASE				0
	#define GUID_SUBRESOURCETYPE_SERIALPORT_PTZ		1
	
	#define GUID_PLATFORMTYPE(p) (((TCGUID *)(p))->uGUID.sGUID.PlatFormType)
	#define GUID_RESOURCETYPE(p) (((TCGUID *)(p))->uGUID.sGUID.ResourceType)
	#define GUID_SUBRESOURCETYPE(p) (((TCGUID *)(p))->uGUID.sGUID.SubResourceType)
	#define GUID_ID(p) (((TCGUID *)(p))->uGUID.sGUID.ID)
	#define GUID_MAC(p) (((TCGUID *)(p))->uGUID.sGUID.MAC)

#endif	//#ifndef __TCGUID__ 

#ifdef __cplusplus
extern "C" {
#endif

/*
使用方法:
		1.首先全局初始化函数GUIDInit
		2.然后调用其他函数使用其功能,无先后顺序之分
 */

/*  功能描述: 初始化GUID模块
 *  参数说明:
 *      int bDecoder [in]:	是否为编码器,否则为解码器
 *  备注: 缺省为编码器,而非解码器
 */
void GUIDInit(int bDecoder);

/*  功能描述: 将GUID转换成字符串,0x打头
 *  参数说明:
 *      const TCGUID *pGuid [IN]:	需要转换的GUID指针
 *  返回值: 转换后的GUID字符串
 *  备注: 该函数不支持多线程重入,总是返回有效的字符串
 */
XOS_PSZ GUID2String(const TCGUID *pGuid);

/*  功能描述: 将0x打头的字符串GUID转成GUID型
 *  参数说明:
 *      XOS_CPSZ pszGuid [IN]:	需要转换的GUID字符串
 *  返回值: 转换后的GUID指针
 *  备注: 该函数不支持多线程重入,	
 */
TCGUID *String2GUID(XOS_CPSZ pszGuid);

/*  功能描述: 根据资源类型和编号合成资源的GUID
 *  参数说明:
 *      TCGUID *pGuid [OUT]:	*pGuid为合成的GUID
 *      XOS_CPSZ lpszType [IN]:	资源的类型
 *      int iNo [IN]:			资源的编号
 *      const char *pMAC [IN]:	设备的MAC地址
 *  返回值: 如果类型不支持,可能返回FALSE
 */
XOS_BOOL TypeAndNo2GUID(TCGUID *pGuid, XOS_CPSZ lpszType, int iNo, const char *pMAC);

/*  功能描述: 从GUID中提取类型和编号
 *  参数说明:
 *      XOS_CPSZ szType [OUT]:		存放的是提取出来的类型字符串
 *      int iTypeLen [IN]:	szType缓冲的长度,如果长度不够,会返回FALSE
 *      int *piNo [IN]:		提取出来的编号
 *      TCGUID *pGuid [IN]:		输入的需要提取的GUID的指针.
 */
XOS_BOOL GUID2TypeAndIdx(char *szType, int iTypeLen, int *piNo, TCGUID *pGuid);

#ifdef __cplusplus
}
#endif

#endif //__GUID_H__
