/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSFile.c
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2007-6-29 15:33:34
 *  修正备注: 
 *  
 */


#include "../XOS.h"

#ifdef WIN32
	#ifdef WINCE
	#else
		#include <sys/stat.h>
		#include <direct.h>
		#include <errno.h>	
	#endif
#elif _CREARO_NDK
#else //LINUX
#endif

int XOSFile_Length(XOS_CPSZ cpszFileName)
{
	int length;
	XOS_HANDLE hFile;
	hFile = XOSFile_Open(cpszFileName, XOS_TRUE);
	if (hFile == NULL)
	{
		return -1;
	}
	if (!XOSFile_Seek(hFile, 0, XOSFILE_SEEK_ORIGIN_END))
	{
		printf("file length seek failed: %s.\r\n", cpszFileName);
		XOSFile_Close(hFile);
		return -1;
	}
	length = XOSFile_Tell(hFile);
	XOSFile_Close(hFile);
	return length;
}

XOS_HANDLE XOSFile_Open(XOS_CPSZ cpszFileName, XOS_BOOL bReadOnly)
{
	FILE *hFile;
	if (bReadOnly)
	{
		return fopen(cpszFileName, "rb");
	}
	else
	{
		hFile = fopen(cpszFileName, "rb+");
		if (hFile == NULL)
		{
			hFile = fopen(cpszFileName, "wb+");
		}
		return hFile;
	}
}

int XOSFile_Write(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen)
{
	return fwrite(pData, 1, uiLen, hFile);
}

int XOSFile_Read(XOS_HANDLE hFile, void *pData, XOS_U32 uiLen)
{
	return fread(pData, 1, uiLen, hFile);
}

XOS_BOOL XOSFile_Seek(XOS_HANDLE hFile, int iOffset, int iOrigin)
{
	int rv;
	rv = fseek(hFile, iOffset, iOrigin);
	return (rv == 0);
}

int XOSFile_Tell(XOS_HANDLE hFile)
{
	return ftell(hFile);
}

void XOSFile_Flush(XOS_HANDLE hFile)
{
	int rv;
	rv = fflush(hFile);
	if (rv != 0)
	{
		printf("file flush failed: %d.\r\n", rv);
	}
	return ;
}

XOS_BOOL XOSFile_Eof(XOS_HANDLE hFile)
{
	int rv;
	rv = feof((FILE *)hFile);
	if (rv == 0)
	{
		return XOS_FALSE;
	}
	else
	{
		return XOS_TRUE;
	}
}

void XOSFile_Close(XOS_HANDLE hFile)
{
	int rv;
	rv = fclose(hFile);
	if (rv != 0)
	{
		printf("file close failed: %d %d.\r\n", rv, hFile);
	}
	return ;
}

/************************************************************************/
/* 目录创建删除和文件删除函数                                           */
/************************************************************************/

XOS_BOOL XOSFile_MakeDir(XOS_CPSZ cpszFullPath)
{
#ifdef WIN32
	#ifdef WINCE
		return XOS_FALSE;
	#else
		int rv;
		char *p = (char *)cpszFullPath;
		while (1)
		{
			p = strstr(p, "/");
			if (p == NULL)
			{
				rv = _mkdir(cpszFullPath);
				if (rv == 0)
				{
					return XOS_TRUE;
				}
				else
				{
					rv = errno;
					if (rv == EEXIST)
					{
						return XOS_TRUE;
					}
					else
					{
						return XOS_FALSE;
					}
				}
			}
			else
			{
				p[0] = 0;
				rv = _mkdir(cpszFullPath);
				if (rv != 0)
				{
					rv = errno;
					if (rv != EEXIST)
					{
						return XOS_FALSE;
					}
				}
				p[0] = '/';
				p ++;
			}
		}
	#endif
#elif _CREARO_NDK 
	return XOS_FALSE;
#else //LINUX
	int rv;
	char *p = (char *)(cpszFullPath+1);
	while (1)
	{
		p = strstr(p, "/");
		if (p == NULL)
		{
			rv = mkdir(cpszFullPath, S_IREAD|S_IWRITE);
			if (rv == 0)
			{
				return XOS_TRUE;
			}
			else
			{
				rv = errno;
				if (rv == EEXIST)
				{
					return XOS_TRUE;
				}
				else
				{
					return XOS_FALSE;
				}
			}
		}
		else
		{
			p[0] = 0;
			rv = mkdir(cpszFullPath, S_IREAD|S_IWRITE);
			if (rv != 0)
			{
				rv = errno;
				if (rv != EEXIST)
				{
					return XOS_FALSE;
				}
			}
			p[0] = '/';
			p ++;
		}
	}
#endif
}

void XOSFile_RemoveDir(XOS_CPSZ cpszFullPath)
{
#ifdef WIN32
	#ifdef WINCE
		return ;
	#else
		int rv;
		rv = _rmdir(cpszFullPath);
		if (rv != 0)
		{
			printf("file remove dir failed: %d %s.\r\n", errno, cpszFullPath);
		}
	#endif
#elif _CREARO_NDK 
#else //LINUX
	int rv;
	rv = rmdir(cpszFullPath);
	if (rv != 0)
	{
		printf("file remove dir failed: %d %s.\r\n", errno, cpszFullPath);
	}
#endif
	return ;
}

void XOSFile_RemoveFile(XOS_CPSZ cpszFullPath, XOS_CPSZ cpszFileName)
{
	static char szFullName[256]; 
	szFullName[0] = 0;
	sprintf(szFullName, "%s/%s", cpszFullPath, cpszFileName);
	XOSFile_RemoveFile2(szFullName);
	return ;
}

void XOSFile_RemoveFile2(XOS_CPSZ cpszFullName)
{
#ifdef WIN32
	#ifdef WINCE
		return ;
	#else
		int rv;
		rv = remove(cpszFullName);
		if (rv != 0)
		{
			printf("file remove file failed: %d %s.\r\n", errno, cpszFullName);
		}
	#endif
#elif _CREARO_NDK 
#else //LINUX
	int rv;
	rv = remove(cpszFullName);
	if (rv != 0)
	{
		printf("file remove file failed: %d %s.\r\n", errno, cpszFullName);
	}
#endif
	return ;
}

/************************************************************************/
/* 目录遍历函数                                                         */
/************************************************************************/

static XOS_PSZ file_findsub(XOS_HANDLE *phFind, XOS_CPSZ cpszFullPath,
								XOS_BOOL bdir, XOS_BOOL bFirst, char *pfilename, int ifilenamelen)
{
#ifdef WIN32
	#ifdef WINCE
		return NULL;
	#else
		char szBuffer[256] = "";
		WIN32_FIND_DATA FindFileData;

		memset(pfilename, 0, ifilenamelen);
		while (1)
		{
			if (bFirst)
			{
				XOS_HANDLE hFind;
				bFirst = XOS_FALSE;
				*phFind = INVALID_HANDLE_VALUE;
				szBuffer[0] = 0;
				sprintf(szBuffer, "%s/*", cpszFullPath);
				hFind = FindFirstFile(szBuffer, &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE)
				{
					return NULL;
				}
				*phFind = hFind;
			}
			else
			{
				if (*phFind == INVALID_HANDLE_VALUE)
				{
					return NULL;
				}
				if (FindNextFile(*phFind, &FindFileData) == 0)
				{
					return NULL;
				}
			}
			
			if (bdir)
			{
				if ((strcmp(FindFileData.cFileName, ".") != 0) && 
					(strcmp(FindFileData.cFileName, "..") != 0) &&
					(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
				{
					if ((int)strlen(FindFileData.cFileName) > (ifilenamelen-1))
					{
						return NULL;
					}
					else
					{
						memcpy(pfilename, FindFileData.cFileName, (ifilenamelen-1));
						return pfilename;
					}
				}
			}
			else
			{
				if ((strcmp(FindFileData.cFileName, ".") != 0) && 
					(strcmp(FindFileData.cFileName, "..") != 0) &&
					(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) &&
					!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
				{
					if ((int)strlen(FindFileData.cFileName) > (ifilenamelen-1))
					{
						return NULL;
					}
					else
					{
						memcpy(pfilename, FindFileData.cFileName, (ifilenamelen-1));
						return pfilename;
					}
				}
			}
		}
	#endif
#elif _CREARO_NDK 
	return NULL;
#else //LINUX
	char szBuffer[256] = "";
	struct dirent *dt;
	while (1)
	{
		if (bFirst)
		{
			//printf("opendir(%s)\r\n", cpszFullPath);
			DIR *dir;
			bFirst = XOS_FALSE;
			*phFind = NULL;
			dir = opendir(cpszFullPath);
			if (dir == NULL)
			{
				return NULL;
			}
			*phFind = dir;
		}
		else
		{
			if (*phFind == NULL)
			{
				return NULL;
			}
		}

		//printf("readdir(%08X)\r\n", *phFind);
		dt = readdir(*phFind);
		if (dt == NULL)
		{
			//printf("readdir failed\r\n");
			return NULL;
		}
		//printf("readdir ok %s\r\n", dt->d_name);

		if ((strcmp(dt->d_name, ".") != 0) && 
			(strcmp(dt->d_name, "..") != 0))
		{
		    struct stat s;
			sprintf(szBuffer, "%s/%s", cpszFullPath, dt->d_name);
			//printf("%s(%d)\r\n", szBuffer, strlen(szBuffer));
			if (lstat(szBuffer, &s) == 0)
			{
				if (bdir)
				{
					if (S_ISDIR(s.st_mode))
					{   
						return dt->d_name;
					}
				}
				else
				{
					if (!S_ISDIR(s.st_mode))
					{   
						return dt->d_name;
					}
				}
			}   
		}
	}
#endif
}

XOS_PSZ XOSFile_FindFirstSub(XOS_HANDLE *phFind, XOS_CPSZ cpszFullPath, XOS_BOOL bDir, 
							 char *pszFileName, int iFileNameLen)
{
	return file_findsub(phFind, cpszFullPath, bDir, XOS_TRUE, pszFileName, iFileNameLen);
}

XOS_PSZ XOSFile_FindNextSub(XOS_HANDLE hFind, XOS_CPSZ cpszFullPath, XOS_BOOL bDir,
							char *pszFileName, int iFileNameLen)
{
	return file_findsub(&hFind, cpszFullPath, bDir, XOS_FALSE, pszFileName, iFileNameLen);
}

void  XOSFile_FindSubClose(XOS_HANDLE hFind)
{
#ifdef WIN32
	#ifdef WINCE
	#else
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
		}
	#endif
#elif _CREARO_NDK 
#else //LINUX
	if (hFind != NULL)
	{
		closedir(hFind);
	}
#endif
	return ;
}
