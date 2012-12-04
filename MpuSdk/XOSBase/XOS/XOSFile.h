/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: XOSFile.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: ��װ�˶Դ����ļ���Ŀ¼���ʽӿ�
 *            ע��ֻ�Դ����ļ�,�������Ƶ��ļ�Ҳ���ʺ���Щ����
 *			  Linuxϵͳ�е������ļ���ô��?�����Ҵ����ļ���ʱ��?
 *			  ��·������?
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2007-6-29 15:23:19
 *  ������ע: 
 *  
 */

#ifndef __XOSFILE_H__
#define __XOSFILE_H__

#define XOSFILE_SEEK_ORIGIN_BEGIN	0
#define XOSFILE_SEEK_ORIGIN_CUR		1
#define XOSFILE_SEEK_ORIGIN_END		2

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* �ļ���д����                                                         */
/************************************************************************/

/*  ��������: ��ȡ�ļ���С
 *  ����˵��:
 *      XOS_CPSZ lpszFileName [IN]:	�ļ���
 *  ����ֵ: �ļ��ֽ���,-1��ʾû������ļ�
 */
int XOSFile_Length(XOS_CPSZ cpszFileName);

/*  ��������: ���ļ�
 *  ����˵��:
 *      XOS_CPSZ cpszFileName [IN]:	�ļ���
 *		XOS_BOOL bReadOnly [IN]: �Ƿ�ֻ��
 *  ����ֵ: �ļ������NULLL:��ʧ��
 */
XOS_HANDLE XOSFile_Open(XOS_CPSZ cpszFileName, XOS_BOOL bReadOnly);

/*  ��������: ������д���ļ�����
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
 *		void *pData [IN]: ָ����д������ݵ�ַ
 *		XOS_U32 uiLen [IN]: �ܹ�д����ַ���
 *  ����ֵ: ����ʵ��д����ֽ���Ŀ
 */
int XOSFile_Write(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen);

/*  ��������: ���ļ�����ȡ����
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
 *		void *pData [OUT]: ����Ŷ�ȡ���������ݿռ�
 *		XOS_U32 uiLen [IN]: ��ȡ���ַ���
 *  ����ֵ: ����ʵ�ʶ�ȡ�����ֽ���Ŀ
 */
int XOSFile_Read(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen);

/*  ��������: Ϊ������������λ������
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
 *		int iOffset [IN]: ����ʾλ��ָ������ڡ���ʼ�㡱�ƶ����ֽ���
 *						  ����0��ʾ������,С��0��ʾ��ǰ��
 *		int iOrigin [IN]: ƫ����ʼ��						
 *						 XOSFILE_SEEK_ORIGIN_BEGIN:	�ļ���ͷ 
 *						 XOSFILE_SEEK_ORIGIN_CUR:	�ļ���ǰλ�� 
 *						 XOSFILE_SEEK_ORIGIN_END:	�ļ�ĩβ
 */
XOS_BOOL XOSFile_Seek(XOS_HANDLE hFile, int iOffset, int iOrigin);

/*  ��������: �õ��ļ��ĵ�ǰλ��
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
 *  ����ֵ: �ļ���ǰλ��������ļ���ͷ���ֽ���,-1��ʾ����
 */
int XOSFile_Tell(XOS_HANDLE hFile);

/*  ��������: ����ļ�������,�ļ���д��ʽ��ʱ������������д���ļ�
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
 */
void XOSFile_Flush(XOS_HANDLE hFile);

/*  ��������: �ж��ļ�ָ���Ƿ�ָ���ļ�β
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
 */
XOS_BOOL XOSFile_Eof(XOS_HANDLE hFile);

/*  ��������: �ر����ļ�ָ��ָ�����ļ�
 *  ����˵��:
 *      XOS_HANDLE hFile [IN]: �Ѵ򿪵��ļ�ָ��
	��ע: �򿪵��ļ�����ʹ����������ر�
 */
void XOSFile_Close(XOS_HANDLE hFile);

/************************************************************************/
/* Ŀ¼����ɾ�����ļ�ɾ������                                           */
/************************************************************************/

/*  ��������: �����ļ���
 *  ����˵��:
 *      XOS_CPSZ cpszFullPath [IN]: �ļ��еľ���·��,�ɰ����༶·��
 *	��ע: �����·���ַ�����Ȼ�������������,���ǻ���ʱ�޸�,Ȼ���ٸĻ�ȥ
 *		  ���Բ��ܹ����볣���ַ���.
 */
XOS_BOOL XOSFile_MakeDir(XOS_CPSZ cpszFullPath);

/*  ��������: �Ƴ��ļ���
 *  ����˵��:
 *      XOS_CPSZ cpszFullPath [IN]: �ļ���·������·��
 */
void XOSFile_RemoveDir(XOS_CPSZ cpszFullPath);

/*  ��������: �Ƴ��ļ�
 *  ����˵��:
 *		XOS_CPSZ cpszFullName [IN]:	ȫ·���ļ���
 *		XOS_CPSZ cpszFullPath [IN]: �ļ�·��
 *      XOS_CPSZ cpszFileName [IN]: Ҫ�Ƴ��ļ���
 */
void XOSFile_RemoveFile2(XOS_CPSZ cpszFullName);
void XOSFile_RemoveFile(XOS_CPSZ cpszFullPath, XOS_CPSZ cpszFileName);

/************************************************************************/
/* Ŀ¼���ļ���������                                                   */
/************************************************************************/

/*  ��������: ���ҵ�һ����Ŀ¼/���ļ�
 *  ����˵��:
 *      XOS_HANDLE *phFind [OUT]:		�ļ����Ҿ��
 *      XOS_CPSZ cpszFullPath [IN]:		Ŀ¼·��
 *		XOS_BOOL bdir [IN]:				�Ƿ���Ŀ¼,XOS_TRUEΪĿ¼,XOS_FALSEΪ�ļ�
 *		char *pszFileName [OUT]:		��Ų�ѯ������Ŀ¼/�ļ����Ļ����ַ
 *		int iFileNameLen [IN]:			���������ַ����󳤶�
 *  ����ֵ: ��Ŀ¼����,NULL��ʾʧ��,������ʾ�ɹ�, 
 */
XOS_PSZ XOSFile_FindFirstSub(XOS_HANDLE *phFind, XOS_CPSZ cpszFullPath, XOS_BOOL bDir, 
							 char *pszFileName, int iFileNameLen);

/*  ��������: ������һ����Ŀ¼/���ļ�
 *  ����˵��:
 *      XOS_HANDLE hFind [IN]:			�ļ����Ҿ��
 *      XOS_CPSZ cpszFullPath [IN]:		Ŀ¼·��,���������XOSFile_FindFirstSubʱ��ͬ
 *		XOS_BOOL bdir [IN]:				�Ƿ���Ŀ¼,���������XOSFile_FindFirstSubʱ��ͬ
 *		char *pszFileName [OUT]:		��Ų�ѯ������Ŀ¼/�ļ����Ļ����ַ
 *		int iFileNameLen [IN]:			���������ַ����󳤶�
 *  ����ֵ: ͬ��
 *  ��ע: Ҫ�ȵ���XOSFile_FindFirstSub��,���ܹ�ʹ��.
 */
XOS_PSZ XOSFile_FindNextSub(XOS_HANDLE hFind, XOS_CPSZ cpszFullPath, XOS_BOOL bDir,
							char *pszFileName, int iFileNameLen);

/*  ��������: ������Ŀ¼/�ļ��Ĳ���
 *  ����˵��:
 *      XOS_HANDLE hFind [IN]:			�ļ����Ҿ��
 *  ��ע: ���ҽ�����ʱ��һ��Ҫ���ô˺�����������
 */
void  XOSFile_FindSubClose(XOS_HANDLE hFind);

#ifdef __cplusplus
}
#endif

#endif //__XOSFILE_H__
