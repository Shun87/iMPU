#ifndef __IMGFILECHECK_H__
#define __IMGFILECHECK_H__

#define IMGFILETYPE_CODE		1
#define IMGFILETYPE_DBDATA		2
#define IMGFILETYPE_WEBFILE		3
#define IMGFILETYPE_FONT		4

int ImgFileCheck(char *pIn, int inLen, int type, char **ppOut, int *pOutLen);

#endif //__IMGFILECHECK_H__
