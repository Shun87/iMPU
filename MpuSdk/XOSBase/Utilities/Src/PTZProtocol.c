/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: PTZProtocol.c
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2006-10-19 21:41:23
 *  ������ע: 
 *  
 */

#include "../PTZProtocol.h"
#include <string.h>

#define MAX_PROTOCOL_NUM		6

static char *s_Protocols[MAX_PROTOCOL_NUM] =
{
	"Pelco D",
	"EVI-D70/P",
	"Panasonic",
	"Pelco P",
	"SAMSUNG",
	"BW-Y50"
};

char **PTZP_GetSupportedProtocols(int *pProtocolNum)
{
	*pProtocolNum = MAX_PROTOCOL_NUM;
	return s_Protocols;
}

int PTZP_GetCmd(char *Cmd, int *pCmdLen, char *Protocol, int CmdType, TPTZPCmdData *pCmdData, int CmdDataLen)
{
	TPTZPCmdData*pPTZPCmdData;

	if (CmdDataLen != sizeof(TPTZPCmdData)){
		return -1;
	}
	pPTZPCmdData = (TPTZPCmdData*)pCmdData;

	if (strcmp(Protocol,"Pelco D") == 0){
		memset(Cmd,0,*pCmdLen);
		Cmd[0] = (char)0x0FF;
		Cmd[1] = (char)pPTZPCmdData->DecoderAddress;

		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			Cmd[3] = (char)0x004;
			Cmd[4] = pPTZPCmdData->Speed*63/100;
			Cmd[5] = (char)0x000;
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			Cmd[3] = (char)0x002;
			Cmd[4] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			Cmd[3] = (char)0x008;
			Cmd[5] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			Cmd[3] = (char)0x010;
			Cmd[5] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StopTurn:
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			Cmd[2] = (char)0x002;
			break;
		case PTZP_CMDTYPE_MinishAperture:
			Cmd[2] = (char)0x004;
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			Cmd[3] = (char)0x080;
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			Cmd[2] = (char)0x001;
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			Cmd[3] = (char)0x040;
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			Cmd[3] = (char)0x020;
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			Cmd[3] = (char)0x007;
			Cmd[5] = 0x022;
			break;
		case PTZP_CMDTYPE_MoveToPresetPosition:
			Cmd[3] = (char)0x007;
			Cmd[5] = (char)pPTZPCmdData->PresetPosition;
			break;
		case PTZP_CMDTYPE_SetPresetPosition:
			Cmd[3] = (char)0x003;
			Cmd[5] = (char)pPTZPCmdData->PresetPosition;
			break;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			Cmd[3] = 0x009;
			Cmd[5] = (char)pPTZPCmdData->SecondaryDevice;
			break;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			Cmd[3] = 0x00B;
			Cmd[5] = (char)pPTZPCmdData->SecondaryDevice;
			break;
		case PTZP_CMDTYPE_StartAutoScan:
			Cmd[3] = 0x01F;
			break;
		case PTZP_CMDTYPE_StopAutoScan:
			Cmd[3] = 0x021;
			break;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			Cmd[3] = 0x011;
			Cmd[5] = (char)pPTZPCmdData->PresetPosition;
			break;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			Cmd[3] = 0x013;
			Cmd[5] = (char)pPTZPCmdData->PresetPosition;
			break;
		case PTZP_CMDTYPE_StartPatrol:
			Cmd[3] = 0x01F;
			break;
		case PTZP_CMDTYPE_StopPatrol:
			Cmd[3] = 0x021;
			break;
		case PTZP_CMDTYPE_BeginSetPatrol:
			break;
		case PTZP_CMDTYPE_SetPatrolMid:
			break;
		case PTZP_CMDTYPE_EndSetPatrol:
			break;
		default:
			return -1;
		}	

		Cmd[6] = (char)(Cmd[1]+Cmd[2]+Cmd[3]+Cmd[4]+Cmd[5])%(0x100);
		*pCmdLen = 7;
		
	}
	else if (strcmp(Protocol,"EVI-D70/P") == 0) {
		//ʵ�ֵļ�˵��:
		//�����Ǹ��ĵ�����������������.ֻ��Ҫ֧����������ת��,������Զ��ͷ,���ڽ���Զ��,���ڹ�Ȧ��С.
		//��Щ����ֱ����ĵ���31ҳ��,��Ҫ��ҳ��˵������:
		//	32ҳ����İ���ʽ.
		//	39ҳ��Zoom/Focus����
		//	40ҳ��Iris����
		//	42ҳ����������ת��������
		//���ڵ�����:9600bps/38400bps Data Bits:8 Start bit:1 Stop bit:1 Nonparity,�Ǹ��ĵ������е�,��ժ¼������.
		//�ο�����Pelco DЭ���ʵ�ַ�ʽ,�������������,���������ַ����Ϳ�����.����*pCmdLen��ֵ����ݲ�ͬ���������ͬ.
		//���Э����������.ǰ���8x�е�x��ʾ��ַ,FFΪ������,�ƺ�û��У��

		memset(Cmd,0,*pCmdLen);
		Cmd[0] = (char) ( 0x80 | ( (char)pPTZPCmdData->DecoderAddress & 0x07 ) );

		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x06;
			Cmd[3] = (char)0x01;
			Cmd[4] = pPTZPCmdData->Speed*17/100;	//Pan Speed
			Cmd[5] = 0;										//Tilt Speed
			Cmd[6] = (char)0x01;
			Cmd[7] = (char)0x03;
			Cmd[8] = (char)0xFF;
			*pCmdLen = 9;
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x06;
			Cmd[3] = (char)0x01;
			Cmd[4] = pPTZPCmdData->Speed*17/100;	//Pan Speed
			Cmd[5] = 0;										//Tilt Speed
			Cmd[6] = (char)0x02;
			Cmd[7] = (char)0x03;
			Cmd[8] = (char)0xFF;
			*pCmdLen = 9;
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x06;
			Cmd[3] = (char)0x01;
			Cmd[4] = 0;										//Pan Speed
			Cmd[5] = pPTZPCmdData->Speed*17/100;	//Tilt Speed
			Cmd[6] = (char)0x03;
			Cmd[7] = (char)0x01;
			Cmd[8] = (char)0xFF;
			*pCmdLen = 9;
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x06;
			Cmd[3] = (char)0x01;
			Cmd[4] = 0;										//Pan Speed
			Cmd[5] = pPTZPCmdData->Speed*17/100;	//Tilt Speed
			Cmd[6] = (char)0x03;
			Cmd[7] = (char)0x02;
			Cmd[8] = (char)0xFF;
			*pCmdLen = 9;
			break;
		case PTZP_CMDTYPE_StopTurn:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x06;
			Cmd[3] = (char)0x01;
			Cmd[4] = 0x10;
			Cmd[5] = Cmd[4];
			Cmd[6] = (char)0x03;
			Cmd[7] = (char)0x03;
			Cmd[8] = (char)0xFF;
			*pCmdLen = 9;
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			//power on
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x00;
			Cmd[4] = (char)0x02;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_MinishAperture:
			//power off
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x00;
			Cmd[4] = (char)0x03;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x08;
			Cmd[4] = (char)0x02;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x08;
			Cmd[4] = (char)0x03;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x08;
			Cmd[4] = (char)0x00;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x07;
			Cmd[4] = (char)0x03;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x07;
			Cmd[4] = (char)0x02;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			Cmd[1] = (char)0x01;
			Cmd[2] = (char)0x04;
			Cmd[3] = (char)0x07;
			Cmd[4] = (char)0x00;
			Cmd[5] = (char)0xFF;
			*pCmdLen = 6;
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			break;
		case PTZP_CMDTYPE_SetPresetPosition:
			break;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			break;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			break;
		case PTZP_CMDTYPE_StartAutoScan:
			return -1;
		case PTZP_CMDTYPE_StopAutoScan:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			return -1;
		case PTZP_CMDTYPE_StartPatrol:
			return -1;
		case PTZP_CMDTYPE_StopPatrol:
			return -1;
		case PTZP_CMDTYPE_BeginSetPatrol:
			return -1;
		case PTZP_CMDTYPE_SetPatrolMid:
			return -1;
		case PTZP_CMDTYPE_EndSetPatrol:
			return -1;
		default:
			break;
		}
	}
	else if (strcmp(Protocol,"Panasonic") == 0){
		memset(Cmd,0,*pCmdLen);
		Cmd[0] = (char)pPTZPCmdData->DecoderAddress;
		Cmd[1] = (char)0xBF;
		Cmd[2] = (char)0x31;
		Cmd[3] = (char)0x00;

		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			Cmd[2] = (char)0x52;
			Cmd[3] = (char)0x07;
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			Cmd[2] = (char)0x53;
			Cmd[3] = (char)0x07;
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			Cmd[2] = (char)0x50;
			Cmd[3] = (char)0x07;
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			Cmd[2] = (char)0x51;
			Cmd[3] = (char)0x07;
			break;
		case PTZP_CMDTYPE_StopTurn:
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			Cmd[2] = (char)0x08;
			Cmd[3] = (char)0x00;
			break;
		case PTZP_CMDTYPE_MinishAperture:
			Cmd[2] = (char)0x09;
			Cmd[3] = (char)0x00;
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			Cmd[2] = (char)0x0A;
			Cmd[3] = (char)0x00;
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			Cmd[2] = (char)0x0B;
			Cmd[3] = (char)0x00;
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			Cmd[2] = (char)0x0D;
			Cmd[3] = (char)0x00;
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			Cmd[2] = (char)0x0C;
			Cmd[3] = (char)0x00;
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			return -1;
		case PTZP_CMDTYPE_SetPresetPosition:
			return -1;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			return -1;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			return -1;
		case PTZP_CMDTYPE_StartAutoScan:
			return -1;
		case PTZP_CMDTYPE_StopAutoScan:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			return -1;
		case PTZP_CMDTYPE_StartPatrol:
			return -1;
		case PTZP_CMDTYPE_StopPatrol:
			return -1;
		case PTZP_CMDTYPE_BeginSetPatrol:
			return -1;
		case PTZP_CMDTYPE_SetPatrolMid:
			return -1;
		case PTZP_CMDTYPE_EndSetPatrol:
			return -1;
		default:
			return -1;
		}	

		Cmd[4] = (char)(Cmd[0]+Cmd[1]+Cmd[2]+Cmd[3])%(0x100);
		*pCmdLen = 5;
		
	}
	else if (strcmp(Protocol,"Pelco P") == 0){
		memset(Cmd,0,*pCmdLen);
		Cmd[0] = (char)0x0A0;
		Cmd[1] = (char)pPTZPCmdData->DecoderAddress;

		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			Cmd[3] = (char)0x004;
			Cmd[4] = pPTZPCmdData->Speed*63/100;
			Cmd[5] = (char)0x000;
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			Cmd[3] = (char)0x002;
			Cmd[4] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			Cmd[3] = (char)0x008;
			Cmd[5] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			Cmd[3] = (char)0x010;
			Cmd[5] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StopTurn:
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			Cmd[2] = (char)0x004;
			break;
		case PTZP_CMDTYPE_MinishAperture:
			Cmd[2] = (char)0x008;
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			Cmd[2] = (char)0x001;
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			Cmd[2] = (char)0x002;
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			Cmd[3] = (char)0x040;
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			Cmd[3] = (char)0x020;
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			Cmd[3] = (char)0x007;
			Cmd[5] = 0x022;
			break;
		case PTZP_CMDTYPE_MoveToPresetPosition:
			Cmd[3] = (char)0x007;
			Cmd[5] = (char)pPTZPCmdData->PresetPosition;
			break;
		case PTZP_CMDTYPE_SetPresetPosition:
			Cmd[3] = (char)0x003;
			Cmd[5] = (char)pPTZPCmdData->PresetPosition;
			break;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			Cmd[3] = 0x009;
			Cmd[5] = (char)pPTZPCmdData->SecondaryDevice;
			break;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			Cmd[3] = 0x00B;
			Cmd[5] = (char)pPTZPCmdData->SecondaryDevice;
			break;
		case PTZP_CMDTYPE_StartAutoScan:
			return -1;
		case PTZP_CMDTYPE_StopAutoScan:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			return -1;
		case PTZP_CMDTYPE_StartPatrol:
			return -1;
		case PTZP_CMDTYPE_StopPatrol:
			return -1;
		case PTZP_CMDTYPE_BeginSetPatrol:
			return -1;
		case PTZP_CMDTYPE_SetPatrolMid:
			return -1;
		case PTZP_CMDTYPE_EndSetPatrol:
			return -1;
		default:
			return -1;
		}	
		Cmd[6] = (char)0x0AF;
		Cmd[7] = (Cmd[0]^Cmd[1]^Cmd[2]^Cmd[3]^Cmd[4]^Cmd[5]^Cmd[6]);
		*pCmdLen = 8;
	}
	else if (strcmp(Protocol,"SAMSUNG") == 0){
		memset(Cmd,0,*pCmdLen);
		Cmd[0] = (char)0x0A0;
		Cmd[1] = (char)0x001;
		Cmd[2] = (char)pPTZPCmdData->DecoderAddress;
		Cmd[3] = (char)0x001;
		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			Cmd[4] = (char)0x000;
			Cmd[5] = (char)0x001;
			Cmd[6] = pPTZPCmdData->Speed*63/100;
			Cmd[7] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			Cmd[4] = (char)0x000;
			Cmd[5] = (char)0x002;
			Cmd[6] = pPTZPCmdData->Speed*63/100;
			Cmd[7] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			Cmd[4] = (char)0x000;
			Cmd[5] = (char)0x004;
			Cmd[6] = pPTZPCmdData->Speed*63/100;
			Cmd[7] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			Cmd[4] = (char)0x000;
			Cmd[5] = (char)0x008;
			Cmd[6] = pPTZPCmdData->Speed*63/100;
			Cmd[7] = pPTZPCmdData->Speed*63/100;
			break;
		case PTZP_CMDTYPE_StopTurn:
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			Cmd[4] = (char)0x008;
			Cmd[5] = (char)0x000;
			Cmd[6] = (char)0x020;
			Cmd[7] = (char)0x020;
			break;
		case PTZP_CMDTYPE_MinishAperture:
			Cmd[4] = (char)0x010;
			Cmd[5] = (char)0x000;
			Cmd[6] = (char)0x020;
			Cmd[7] = (char)0x020;
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			Cmd[4] = (char)0x001;
			Cmd[5] = (char)0x000;
			Cmd[6] = (char)0x020;
			Cmd[7] = (char)0x020;
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			Cmd[4] = (char)0x002;
			Cmd[5] = (char)0x000;
			Cmd[6] = (char)0x020;
			Cmd[7] = (char)0x020;
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			Cmd[4] = (char)0x040;
			Cmd[5] = (char)0x000;
			Cmd[6] = (char)0x020;
			Cmd[7] = (char)0x020;
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			Cmd[4] = (char)0x020;
			Cmd[5] = (char)0x000;
			Cmd[6] = (char)0x020;
			Cmd[7] = (char)0x020;
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			return -1;
		case PTZP_CMDTYPE_MoveToPresetPosition:
			return -1;
		case PTZP_CMDTYPE_SetPresetPosition:
			return -1;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			return -1;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			return -1;
		case PTZP_CMDTYPE_StartAutoScan:
			return -1;
		case PTZP_CMDTYPE_StopAutoScan:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			return -1;
		case PTZP_CMDTYPE_StartPatrol:
			return -1;
		case PTZP_CMDTYPE_StopPatrol:
			return -1;
		case PTZP_CMDTYPE_BeginSetPatrol:
			return -1;
		case PTZP_CMDTYPE_SetPatrolMid:
			return -1;
		case PTZP_CMDTYPE_EndSetPatrol:
			return -1;
		default:
			return -1;
		}	
		Cmd[8] = ~(char)(Cmd[1]+Cmd[2]+Cmd[3]+Cmd[4]+Cmd[5]+Cmd[6]+Cmd[7]);
		*pCmdLen = 9;
	}
	else if (strcmp(Protocol,"BW-Y50") == 0){
		Cmd[0] = (char)0x0AA;
		Cmd[1] = (char)pPTZPCmdData->DecoderAddress;
		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			Cmd[2] = 0x012;
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			Cmd[2] = 0x013;
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			Cmd[2] = 0x010;
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			Cmd[2] = 0x011;
			break;
		case PTZP_CMDTYPE_StopTurn:
			Cmd[2] = 0x021;
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			Cmd[2] = 0x01C;
			break;
		case PTZP_CMDTYPE_MinishAperture:
			Cmd[2] = 0x01D;
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			Cmd[2] = 0x020;
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			Cmd[2] = 0x01A;
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			Cmd[2] = 0x01B;
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			Cmd[2] = 0x020;
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			Cmd[2] = 0x019;
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			Cmd[2] = 0x018;
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			Cmd[2] = 0x020;
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			return -1;
		case PTZP_CMDTYPE_MoveToPresetPosition:
			return -1;
		case PTZP_CMDTYPE_SetPresetPosition:
			return -1;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			if (pPTZPCmdData->SecondaryDevice == 1) {
				//�л���
				Cmd[2] = 0x016;
			}
			else if (pPTZPCmdData->SecondaryDevice == 2) {
				//��ˢ��
				Cmd[2] = 0x014;
			}
			else {
				return -1;
			}
			break;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			if (pPTZPCmdData->SecondaryDevice == 1) {
				//�л���
				Cmd[2] = 0x017;
			}
			else if (pPTZPCmdData->SecondaryDevice == 2) {
				//��ˢͣ
				Cmd[2] = 0x015;
			}
			else {
				return -1;
			}
			break;
		case PTZP_CMDTYPE_StartAutoScan:
			Cmd[2] = 0x030;
			break;
		case PTZP_CMDTYPE_StopAutoScan:
			Cmd[2] = 0x022;
			break;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			return -1;
		case PTZP_CMDTYPE_StartPatrol:
			//��Ϊ��Ѳ������,ʹ�ø�λ����
			Cmd[2] = 0x055;
			break;
		case PTZP_CMDTYPE_StopPatrol:
			return -1;
		case PTZP_CMDTYPE_BeginSetPatrol:
			return -1;
		case PTZP_CMDTYPE_SetPatrolMid:
			return -1;
		case PTZP_CMDTYPE_EndSetPatrol:
			return -1;
		default:
			return -1;
		}	
		*pCmdLen = 3;
	}
	else{
/*		
		switch(CmdType) {
		case PTZP_CMDTYPE_StartTurnLeft:
			break;
		case PTZP_CMDTYPE_StartTurnRight:
			break;
		case PTZP_CMDTYPE_StartTurnUp:
			break;
		case PTZP_CMDTYPE_StartTurnDown:
			break;
		case PTZP_CMDTYPE_StopTurn:
			break;
		case PTZP_CMDTYPE_AugmentAperture:
			break;
		case PTZP_CMDTYPE_MinishAperture:
			break;
		case PTZP_CMDTYPE_StopApertureZoom:
			break;
		case PTZP_CMDTYPE_MakeFocusFar:
			break;
		case PTZP_CMDTYPE_MakeFocusNear:
			break;
		case PTZP_CMDTYPE_StopFocusMove:
			break;
		case PTZP_CMDTYPE_ZoomOutPicture:
			break;
		case PTZP_CMDTYPE_ZoomInPicture:
			break;
		case PTZP_CMDTYPE_StopPictureZoom:
			break;
		case PTZP_CMDTYPE_GotoOriginalPresetPosition:
			break;
		case PTZP_CMDTYPE_MoveToPresetPosition:
			return -1;
		case PTZP_CMDTYPE_SetPresetPosition:
			break;
		case PTZP_CMDTYPE_StartSecondaryDevice:
			break;
		case PTZP_CMDTYPE_StopSecondaryDevice:
			break;
		case PTZP_CMDTYPE_StartAutoScan:
			return -1;
		case PTZP_CMDTYPE_StopAutoScan:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanBegin:
			return -1;
		case PTZP_CMDTYPE_SetAutoScanEnd:
			return -1;
		case PTZP_CMDTYPE_StartPatrol:
			return -1;
		case PTZP_CMDTYPE_StopPatrol:
			return -1;
		case PTZP_CMDTYPE_BeginSetPatrol:
			return -1;
		case PTZP_CMDTYPE_SetPatrolMid:
			return -1;
		case PTZP_CMDTYPE_EndSetPatrol:
			return -1;
		default:
			return -1;
		}	
*/		
		return -1;
	}

	return 0;
}
