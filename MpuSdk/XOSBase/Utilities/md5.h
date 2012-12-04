#ifndef __MD5_H__
#define __MD5_H__

//////////////////////////////////////////////////////////////////////////
//	DES�ڲ����ݽṹ�Ͷ���
//////////////////////////////////////////////////////////////////////////

/*MD5.H - MD5C.Cͷ�ļ�*/
#define MD5_DIGEST_LEN		16
/* POINTER �����һ����ͨ��ָ������ */
typedef unsigned char *POINTER;
/* UINT2 ��������ֽڵ��� */
typedef unsigned short UINT2;
/* UINT4��һ�����ֽڵ���  */
typedef unsigned int UINT4;
/* MD5 context. */
typedef struct 
{
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* λ����, ģ 2^64 (��λ��ǰ) */
  unsigned char buffer[64];                         /* ���뻺���� */
} MD5_CTX;

//////////////////////////////////////////////////////////////////////////
//	DES�����ӿ�
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/*
	ʹ�÷���:
		1.�ȵ���MD5_Init
		2.Ȼ�����MD5_Update,����ԭ��
		3.Ȼ�����MD5_Final�õ�ɢ��ժҪ
 */

/*  ��������: ��ʼ��md5���ݽṹ
 *  ����˵��:
 *      MD5_CTX *context [IN]:	��Ҫ��ʼ����md5���ݽṹָ��
 */
void MD5_Init(MD5_CTX *context);

/*  ��������: ������Ҫɢ�е�����
 *  ����˵��:
 *      MD5_CTX *context [IN]:	��ʼ���õ�md5���ݽṹָ��
 *      unsigned char *input [IN]:	��Ҫɢ�е����ݻ����ַ
 *      unsigned int inputLen [IN]:	��Ҫɢ�е����ݵĳ���
 */
void MD5_Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);

/*  ��������: ����ɢ�н��
 *  ����˵��:
 *      unsigned char digest[16] [OUT]:	���������16�ֽڵ�ɢ������
 *      MD5_CTX *context [IN]:			��ʼ���õ�md5���ݽṹָ��
 *  ����ֵ: void 
 *  ��ע:	
 */
void MD5_Final(unsigned char digest[16], MD5_CTX *context);

#ifdef __cplusplus
}
#endif

#endif //__MD5_H__
