/* 
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: ptzp.h
 *  文件标识: 
 *  摘    要: PTZP = PTZ Protocol	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2005-11-07 15:46:34
 *  修正备注: 
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2006-10-19 21:40:28
 *  修正备注: 1.将文件更名为PTZProtocol.h
 *			  2.修改了获取支持的云台协议的接口
 *  
 */

#ifndef __PTZPROTOCOL_H__
#define __PTZPROTOCOL_H__

#define PTZP_CMDTYPE_StartTurnLeft								1
#define PTZP_CMDTYPE_StartTurnRight								2	
#define PTZP_CMDTYPE_StartTurnUp								3	
#define PTZP_CMDTYPE_StartTurnDown								4	
#define PTZP_CMDTYPE_StopTurn									5	
#define PTZP_CMDTYPE_AugmentAperture							6	
#define PTZP_CMDTYPE_MinishAperture								7	
#define PTZP_CMDTYPE_StopApertureZoom							8	
#define PTZP_CMDTYPE_MakeFocusFar								9	
#define PTZP_CMDTYPE_MakeFocusNear								10	
#define PTZP_CMDTYPE_StopFocusMove								11	
#define PTZP_CMDTYPE_ZoomOutPicture								12	
#define PTZP_CMDTYPE_ZoomInPicture								13	
#define PTZP_CMDTYPE_StopPictureZoom							14	
#define PTZP_CMDTYPE_GotoOriginalPresetPosition					15	
#define PTZP_CMDTYPE_SetPresetPosition							16	
#define PTZP_CMDTYPE_StartSecondaryDevice						20	
#define PTZP_CMDTYPE_StopSecondaryDevice						21	
#define PTZP_CMDTYPE_MoveToPresetPosition						22
#define PTZP_CMDTYPE_StartAutoScan								23
#define PTZP_CMDTYPE_StopAutoScan								24
#define PTZP_CMDTYPE_SetAutoScanBegin							25
#define PTZP_CMDTYPE_SetAutoScanEnd								26
#define PTZP_CMDTYPE_StartPatrol								27
#define PTZP_CMDTYPE_StopPatrol									28
#define PTZP_CMDTYPE_BeginSetPatrol								29
#define PTZP_CMDTYPE_SetPatrolMid								30
#define PTZP_CMDTYPE_EndSetPatrol								31

typedef struct _TPTZPCmdData {
	int DecoderAddress;		//解码器地址
	int Angle;				//实际的角度
	int Speed;				//0-100之间的值
	int PresetPosition;		//预置位编号
	int SecondaryDevice;	//辅助设备编号
} TPTZPCmdData;

#ifdef __cplusplus
extern "C" {
#endif

/*  功能描述: 得到支持的云台协议
 *  参数说明:
 *      int *pProtocolNum [OUT]:	支持的协议数目
 *  返回值: 支持的协议的数组指针
 */
char **PTZP_GetSupportedProtocols(int *pProtocolNum);

/*  功能描述: 从云台的操作得到云台控制的命令串
 *  参数说明:
 *      char *Cmd [OUT]:			返回的命令串
 *      int *pCmdLen [IN, OUT]:		输入的时候是Cmd所能容下的最大长度,输出的时候是Cmd的实际长度
 *      char *Protocol [IN]:		云台协议	
 *      int CmdType [IN]:			云台操作的类型
 *      TPTZPCmdData *pCmdData [IN]:云台操作的类型所需的数据
 *      int CmdDataLen [IN]:		上述数据的长度	
 *  返回值: 0表示成功,其他表示错误代码
 */
int PTZP_GetCmd(char *Cmd, int *pCmdLen, char *Protocol, int CmdType, TPTZPCmdData *pCmdData, int CmdDataLen);

#ifdef __cplusplus
}
#endif

#endif //__PTZPROTOCOL_H__
