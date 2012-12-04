/*  
 *  Copyright (c) 2005, ���մ����Ƽ����޹�˾
 *  All rights reserved.
 *  
 *  �ļ�����: des.h
 *  �ļ���ʶ: 
 *  ժ    Ҫ:	
 *  
 *  ��ǰ�汾: 
 *  ��    ��: Tommy
 *  �������: 2008-10-20 15:30:56
 *  ������ע: 
 *  
 */

#ifndef __DES_H__
#define __DES_H__

//////////////////////////////////////////////////////////////////////////
//	DES�ڲ����ݽṹ
//////////////////////////////////////////////////////////////////////////

#define DES_BLOCK_LEN	8
typedef struct _TDes {
	int sp_[8][64]; 	// Combined S and P boxes 
	char iperm[16][16][8];	// Initial and final permutations 
	char fperm[16][16][8];
	// 8 6-bit subkeys for each of 16 rounds, initialized by setkey() 
	unsigned char kn[16][8];
	int desmode;
} TDes,*DES_HANDLE;

//////////////////////////////////////////////////////////////////////////
//	DES�����ӿ�
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/*
	ʹ�÷���:
		1.�ȵ���Des_Init
		2.Ȼ�����Des_SetKey
		3.Ȼ����м���(Des_Enc)���߽���(Des_Dec)
 */


/*  ��������: ��ʼ��DESģ��
 *  ����˵��:
 *      DES_HANDLE Handle [IN]:	DESģ����,��TDes�ṹָ��
 *      int Mode [IN]:			DESģʽ,Ŀǰȡ0,��ʾ��׼�����㷨
 *										ȡ1��2�ĺ����c�ļ��е�˵��.
 *  ����ֵ: 0��ʾ�ɹ�,������ʾʧ��,Ŀǰ���᷵��ʧ��.
 */
int  Des_Init(DES_HANDLE Handle, int Mode);

/*  ��������: 
 *  ����˵��:
 *      DES_HANDLE Handle [IN]:	DESģ����,��TDes�ṹָ��
 *      char *Key [IN]:			DES��������,���ȹ̶�Ϊ8�ֽ�
 *  ����ֵ: void 
 *  ��ע:	
 */
void Des_SetKey(DES_HANDLE Handle, char *Key);

/*  ��������: 
 *  ����˵��:
 *      DES_HANDLE Handle [IN]:	DESģ����,��TDes�ṹָ��
 *      char *Block [IN, OUT]:	��ȥΪ����ǰ������,����Ϊ���ܺ������,����Ϊ8�ֽ�
 *  ��ע:	
 */
void Des_Enc(DES_HANDLE Handle, char *Block);

/*  ��������: 
 *  ����˵��:
 *      DES_HANDLE Handle [IN]:	DESģ����,��TDes�ṹָ��
 *      char *Block [IN, OUT]:	��ȥΪ���ܺ������,����Ϊ����ǰ������,����Ϊ8�ֽ�
 *  ��ע:	
 */
void Des_Dec(DES_HANDLE Handle, char *Block);

#ifdef __cplusplus
}
#endif

#endif //__DES_H__
