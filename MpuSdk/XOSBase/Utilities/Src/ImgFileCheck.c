#include "../ImgFileCheck.h"
#include "../md5.h"

static char digest[MD5_DIGEST_LEN];
static MD5_CTX context;

static int s_memcmp(char *dest, char *src, int len)
{
	int i;
	for (i=0;i<len;i++) {
		if (dest[i] != src[i]) {
			return -1;
		}
	}	
	return 0;
}

static void s_memcpy(char *dest, char *src, int len)
{
	int i;
	for (i=0;i<len;i++) {
		dest[i] = src[i];
	}	
	return ;
}

static void s_memset(char *p, int v, int len)
{
	int i;
	for (i=0;i<len;i++) {
		p[i] = v;
	}	
	return ;
}

int ImgFileCheck(char *pIn, int inLen, int type, char **ppOut, int *pOutLen)
{
	char *p;
	char ImgFileFlag[] = "NRCA";
	char CodeFlag[] = "CODE";
	char DBDataFlag[] = "DBDT";
	char WebFileFlag[] = "WEBF";
	char FontFlag[] = "FONT";
	char Hash[16];
	int fLen;
	
	if (inLen < (50+32)) {
		return -1;
	}
	
	//跳过前面的50字节的MAC+KEY+SN
	p = pIn + 50;
	//判断文件头
	if (s_memcmp(p, (char *)ImgFileFlag, 4) != 0) {
		return -1;
	}
	p += 4;
	//判断文件版本
	if (p[0] != 0x01) {
		return -1;
	}
	p ++;
	//跳过3字节保留字
	p += 3;
	//判断文件类型
	if (!
		(
		((s_memcmp(p, CodeFlag, 4) == 0) && (type == IMGFILETYPE_CODE)) ||
		((s_memcmp(p, DBDataFlag, 4) == 0) && (type == IMGFILETYPE_DBDATA)) ||
		((s_memcmp(p, WebFileFlag, 4) == 0) && (type == IMGFILETYPE_WEBFILE)) ||
		((s_memcmp(p, FontFlag, 4) == 0) && (type == IMGFILETYPE_FONT)) 
		)
		) {
		return -1;	
	}
	p += 4;
	//判断文件长度
	fLen = ((unsigned char)p[0] << 24) + ((unsigned char)p[1] << 16) + 
			((unsigned char)p[2] << 8) + ((unsigned char)p[3] << 0);
	if (fLen > (inLen-50)) {
		return -1;	
	}
	p += 4;
	
	s_memcpy(Hash, p, 16);
	s_memset(p, 0, 16);
	
	MD5_Init (&context);
	MD5_Update (&context, (unsigned char *)pIn, fLen+50);
	MD5_Final ((unsigned char *)digest, &context);
	
	if (s_memcmp(Hash, digest, 16) != 0) {
		return -1;
	}
	
	*ppOut = p + 16;
	*pOutLen = fLen - 32;
	return 0;
}
