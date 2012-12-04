#ifndef _CR_AUDIODEC_SHELL_H_
#define _CR_AUDIODEC_SHELL_H_

#ifdef WIN32
#include <windows.h>
#else
#define WINAPI 
#endif

#define CRM_OK							0
#define CRM_ERROR_UNKONWN				-100  //这一层封装的错误码从-100开始。-1~-99是解码库返回的错误
#define CRM_ERROR_DLLNAME_INVALID		-101  //DLL库的路径或者名字不对
#define CRM_ERROR_NO_DLL				-102  //对于媒体帧没有合适的解码库
#define CRM_ERROR_NO_DLL_FUNC			-103  //厂商提供的DLL库中实现的函数不全
#define CRM_ERROR_FALID_CREATE_DECODER	-104  //调用创建解码器实例的函数失败
#define CRM_ERROR_FALID_DEC_FRM			-105  //没能成功解出一帧数据

typedef struct _AUDIODEC_FRAME_PARAM {
	//输入
	unsigned char *pData; //pData存放压缩数据,指向音频频帧头开始的数据。
	int DataLen; //DataLen是数据帧长度,
	//输出
	char AlgName[32]; //字符串输出，算法名称。最长31字节,仅仅为调试用，空字符串输出也行
	unsigned char *pPCM; //pPCM由调用者分配，用来存储输出的解码后数据
	int PCMLen; //PCMLen，返回解码后数据的长度
}AUDIODEC_FRAME_PARAM;

#ifdef __cplusplus
extern "C" {
#endif

// 创建一个解码器壳实例
//char *pDLLPath是VS_OEMPlugin的路径(需要加最后的\)，一般形如："C:\\windows\\system32\\VS_OEMPlugin\\"
unsigned int WINAPI AudioDecShell_Create(char *pDLLPath);

// 删除一个解码器壳实例
void WINAPI AudioDecShell_Delete(unsigned int Handle);

//解码一帧音频数据
//*pProducerID用来返回厂商ID
//pAudioDecParam存放和解码有关的输入输出数据
//返回0，说明成功解出一帧数据，-1表明失败
int WINAPI AudioDecShell_DecFrm(unsigned int Handle, unsigned short *pProducerID, AUDIODEC_FRAME_PARAM *pAudioDecParam); 

#ifdef __cplusplus
}
#endif


#endif
