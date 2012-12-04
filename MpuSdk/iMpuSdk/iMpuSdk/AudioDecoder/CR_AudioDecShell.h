#ifndef _CR_AUDIODEC_SHELL_H_
#define _CR_AUDIODEC_SHELL_H_

#ifdef WIN32
#include <windows.h>
#else
#define WINAPI 
#endif

#define CRM_OK							0
#define CRM_ERROR_UNKONWN				-100  //��һ���װ�Ĵ������-100��ʼ��-1~-99�ǽ���ⷵ�صĴ���
#define CRM_ERROR_DLLNAME_INVALID		-101  //DLL���·���������ֲ���
#define CRM_ERROR_NO_DLL				-102  //����ý��֡û�к��ʵĽ����
#define CRM_ERROR_NO_DLL_FUNC			-103  //�����ṩ��DLL����ʵ�ֵĺ�����ȫ
#define CRM_ERROR_FALID_CREATE_DECODER	-104  //���ô���������ʵ���ĺ���ʧ��
#define CRM_ERROR_FALID_DEC_FRM			-105  //û�ܳɹ����һ֡����

typedef struct _AUDIODEC_FRAME_PARAM {
	//����
	unsigned char *pData; //pData���ѹ������,ָ����ƵƵ֡ͷ��ʼ�����ݡ�
	int DataLen; //DataLen������֡����,
	//���
	char AlgName[32]; //�ַ���������㷨���ơ��31�ֽ�,����Ϊ�����ã����ַ������Ҳ��
	unsigned char *pPCM; //pPCM�ɵ����߷��䣬�����洢����Ľ��������
	int PCMLen; //PCMLen�����ؽ�������ݵĳ���
}AUDIODEC_FRAME_PARAM;

#ifdef __cplusplus
extern "C" {
#endif

// ����һ����������ʵ��
//char *pDLLPath��VS_OEMPlugin��·��(��Ҫ������\)��һ�����磺"C:\\windows\\system32\\VS_OEMPlugin\\"
unsigned int WINAPI AudioDecShell_Create(char *pDLLPath);

// ɾ��һ����������ʵ��
void WINAPI AudioDecShell_Delete(unsigned int Handle);

//����һ֡��Ƶ����
//*pProducerID�������س���ID
//pAudioDecParam��źͽ����йص������������
//����0��˵���ɹ����һ֡���ݣ�-1����ʧ��
int WINAPI AudioDecShell_DecFrm(unsigned int Handle, unsigned short *pProducerID, AUDIODEC_FRAME_PARAM *pAudioDecParam); 

#ifdef __cplusplus
}
#endif


#endif
