#ifndef	__WORDPARSER__
#define __WORDPARSER__

/************************************************************************/
/*	��һ���ı��ж���һ�����ʻ����,�����Էָ���						
	����:	��һ��ʹ��text����Ϊ�ı���ָ��,�Ժ�ʹ�ò���ΪNULL
	����ֵ:	һ���ַ���,'\0'��β
	��ע:	������󳤶�ΪMAXWORDLENGHTH, 
	���ȴ���MAXWORDLENGHTH�ĵ�������β����
																		*/
/************************************************************************/
//#define MAXWORDLENGHTH 128
#define MAXWORDLENGHTH 192

#ifdef __cplusplus
extern "C" {
#endif

char* WPGetWord(char* text);

#ifdef __cplusplus
}
#endif

#endif//__WORDPARSER__
