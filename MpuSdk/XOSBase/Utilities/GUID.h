/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: GUID.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: GUID�����Ͷ����Լ���صĺ���
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-09-02 16:28:11
 *  ������ע: 
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
ʹ�÷���:
		1.����ȫ�ֳ�ʼ������GUIDInit
		2.Ȼ�������������ʹ���书��,���Ⱥ�˳��֮��
 */

/*  ��������: ��ʼ��GUIDģ��
 *  ����˵��:
 *      int bDecoder [in]:	�Ƿ�Ϊ������,����Ϊ������
 *  ��ע: ȱʡΪ������,���ǽ�����
 */
void GUIDInit(int bDecoder);

/*  ��������: ��GUIDת�����ַ���,0x��ͷ
 *  ����˵��:
 *      const TCGUID *pGuid [IN]:	��Ҫת����GUIDָ��
 *  ����ֵ: ת�����GUID�ַ���
 *  ��ע: �ú�����֧�ֶ��߳�����,���Ƿ�����Ч���ַ���
 */
XOS_PSZ GUID2String(const TCGUID *pGuid);

/*  ��������: ��0x��ͷ���ַ���GUIDת��GUID��
 *  ����˵��:
 *      XOS_CPSZ pszGuid [IN]:	��Ҫת����GUID�ַ���
 *  ����ֵ: ת�����GUIDָ��
 *  ��ע: �ú�����֧�ֶ��߳�����,	
 */
TCGUID *String2GUID(XOS_CPSZ pszGuid);

/*  ��������: ������Դ���ͺͱ�źϳ���Դ��GUID
 *  ����˵��:
 *      TCGUID *pGuid [OUT]:	*pGuidΪ�ϳɵ�GUID
 *      XOS_CPSZ lpszType [IN]:	��Դ������
 *      int iNo [IN]:			��Դ�ı��
 *      const char *pMAC [IN]:	�豸��MAC��ַ
 *  ����ֵ: ������Ͳ�֧��,���ܷ���FALSE
 */
XOS_BOOL TypeAndNo2GUID(TCGUID *pGuid, XOS_CPSZ lpszType, int iNo, const char *pMAC);

/*  ��������: ��GUID����ȡ���ͺͱ��
 *  ����˵��:
 *      XOS_CPSZ szType [OUT]:		��ŵ�����ȡ�����������ַ���
 *      int iTypeLen [IN]:	szType����ĳ���,������Ȳ���,�᷵��FALSE
 *      int *piNo [IN]:		��ȡ�����ı��
 *      TCGUID *pGuid [IN]:		�������Ҫ��ȡ��GUID��ָ��.
 */
XOS_BOOL GUID2TypeAndIdx(char *szType, int iTypeLen, int *piNo, TCGUID *pGuid);

#ifdef __cplusplus
}
#endif

#endif //__GUID_H__
