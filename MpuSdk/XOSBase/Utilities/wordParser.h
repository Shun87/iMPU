#ifndef	__WORDPARSER__
#define __WORDPARSER__

/************************************************************************/
/*	从一个文本中读出一个单词或符号,单词以分隔符						
	参数:	第一次使用text参数为文本的指针,以后使用参数为NULL
	返回值:	一个字符串,'\0'结尾
	备注:	单词最大长度为MAXWORDLENGHTH, 
	长度大于MAXWORDLENGHTH的单词做截尾处理
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
