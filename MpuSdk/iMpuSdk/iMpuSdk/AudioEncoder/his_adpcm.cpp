#include <stdlib.h>
#include <string.h>
#include "his_adpcm.h"
#include "adpcm.h"

typedef struct _HIS_ADPCM_OBJ 
{
	adpcm_state state;
	unsigned char HisHead[4];
}HIS_ADPCM_OBJ;

int HIS_ADPCM_Create(HIS_ADPCM_HANDLE *phHIS)
{
	HIS_ADPCM_OBJ *pObj;
	pObj = (HIS_ADPCM_OBJ *)malloc(sizeof(HIS_ADPCM_OBJ));
	memset(pObj, 0, sizeof(HIS_ADPCM_OBJ));

	pObj->HisHead[0] = 0x0;
	pObj->HisHead[1] = 0x01;
	pObj->HisHead[2] = 0x16;
	pObj->HisHead[3] = 0x0;
	
	*phHIS = (HIS_ADPCM_HANDLE)pObj;
	return 0;
}

void HIS_ADPCM_Delete(HIS_ADPCM_HANDLE hHIS)
{
	HIS_ADPCM_OBJ *pObj = (HIS_ADPCM_OBJ *)hHIS;
	if (NULL == pObj) 
	{
		return;
	}
	free(pObj);
}

int HIS_ADPCM_Encode(HIS_ADPCM_HANDLE hHIS, short *pPCM, int SampleNum, unsigned char *pStream, int *pMaxStreamLen)
{
	HIS_ADPCM_OBJ *pObj = (HIS_ADPCM_OBJ *)hHIS;
	if (NULL == pObj) 
	{
		return HIS_ADPCM_E_HANDLE;
	}
	if (HIS_ADPCM_SAMPLE_NUM != SampleNum)
	{
		return HIS_ADPCM_E_SAMPLE_NUM;
	}
	if (*pMaxStreamLen < HIS_ADPCM_FRM_LEN)
	{
		return HIS_ADPCM_E_BUF_LEN;
	}
	memcpy(pStream, pObj->HisHead, 4);
	memcpy(pStream+4, &(pObj->state), sizeof(adpcm_state));
	adpcm_coder(pPCM, (char *)pStream+4+sizeof(adpcm_state), HIS_ADPCM_SAMPLE_NUM, &(pObj->state));
	*pMaxStreamLen = HIS_ADPCM_FRM_LEN;

	return HIS_ADPCM_OK;
}

int HIS_ADPCM_Decode(HIS_ADPCM_HANDLE hHIS, short *pPCM, int *pMaxSampleNum, unsigned char *pStream, int StreamLen)
{
	HIS_ADPCM_OBJ *pObj = (HIS_ADPCM_OBJ *)hHIS;
	if (NULL == pObj) 
	{
		return HIS_ADPCM_E_HANDLE;
	}
	if (HIS_ADPCM_FRM_LEN != StreamLen)
	{
		return HIS_ADPCM_E_STREAM_FMT;
	}
	if (*pMaxSampleNum < HIS_ADPCM_SAMPLE_NUM)
	{
		return HIS_ADPCM_E_BUF_LEN;
	}
	int rv;
	rv = memcmp(pStream, pObj->HisHead, 4);
	if (0 != rv)
	{
		//HIS码流的头不对
		return HIS_ADPCM_E_STREAM_FMT;
	}
	adpcm_state tmp_state;
	memcpy(&tmp_state, pStream+4, sizeof(adpcm_state));
	adpcm_decoder((char *)pStream+4+sizeof(adpcm_state), pPCM, HIS_ADPCM_SAMPLE_NUM, &tmp_state);
	*pMaxSampleNum = HIS_ADPCM_SAMPLE_NUM;
	
	return HIS_ADPCM_OK;
}
