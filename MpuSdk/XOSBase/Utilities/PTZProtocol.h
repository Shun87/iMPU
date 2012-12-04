/* 
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: ptzp.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ: PTZP = PTZ Protocol	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2005-11-07 15:46:34
 *  ������ע: 
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-10-19 21:40:28
 *  ������ע: 1.���ļ�����ΪPTZProtocol.h
 *			  2.�޸��˻�ȡ֧�ֵ���̨Э��Ľӿ�
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
	int DecoderAddress;		//��������ַ
	int Angle;				//ʵ�ʵĽǶ�
	int Speed;				//0-100֮���ֵ
	int PresetPosition;		//Ԥ��λ���
	int SecondaryDevice;	//�����豸���
} TPTZPCmdData;

#ifdef __cplusplus
extern "C" {
#endif

/*  ��������: �õ�֧�ֵ���̨Э��
 *  ����˵��:
 *      int *pProtocolNum [OUT]:	֧�ֵ�Э����Ŀ
 *  ����ֵ: ֧�ֵ�Э�������ָ��
 */
char **PTZP_GetSupportedProtocols(int *pProtocolNum);

/*  ��������: ����̨�Ĳ����õ���̨���Ƶ����
 *  ����˵��:
 *      char *Cmd [OUT]:			���ص����
 *      int *pCmdLen [IN, OUT]:		�����ʱ����Cmd�������µ���󳤶�,�����ʱ����Cmd��ʵ�ʳ���
 *      char *Protocol [IN]:		��̨Э��	
 *      int CmdType [IN]:			��̨����������
 *      TPTZPCmdData *pCmdData [IN]:��̨�������������������
 *      int CmdDataLen [IN]:		�������ݵĳ���	
 *  ����ֵ: 0��ʾ�ɹ�,������ʾ�������
 */
int PTZP_GetCmd(char *Cmd, int *pCmdLen, char *Protocol, int CmdType, TPTZPCmdData *pCmdData, int CmdDataLen);

#ifdef __cplusplus
}
#endif

#endif //__PTZPROTOCOL_H__
