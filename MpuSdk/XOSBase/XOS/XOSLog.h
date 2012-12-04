/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: XOSLog.h
 *  文件标识: 
 *  摘    要:	
 *  
 *  当前版本: 
 *  作    者: Tommy
 *  完成日期: 2009-3-2 20:13:13
 *  修正备注: 
 *  
 */

#ifndef __XOSLOG_H__
#define __XOSLOG_H__

#define XLL_XX		"XX"	//正常日志
#define XLL_IM		"IM"	//不可能出现的情况
#define XLL_EX		"EX"	//异常情况
#define XLL_CR		"CR"	//关键流程
#define XLL_UC		"UC"	//用户操作

//录像用户日志类型
#define USER_LOG_TYPE_NULL					0
#define USER_LOG_TYPE_LOGIN					1
#define USER_LOG_TYPE_LOGOUT				2
#define USER_LOG_TYPE_SYSTEM_BOOTED			3
#define USER_LOG_TYPE_REBOOT				4
#define USER_LOG_TYPE_SET_TIME				5
#define USER_LOG_TYPE_FORMATTED				6
#define USER_LOG_TYPE_OPEN_DOOR				7
#define USER_LOG_TYPE_CLOSE_DOOR			8
#define USER_LOG_TYPE_SHUTDOWN				9


/*  功能描述: 读取字符函数
 *  返回值: 读取到的字符,-1表示没有读取到的字符
 */
typedef char (*XOSLOG_GET_CHAR)();

/*  功能描述: 输出字符函数
 *  参数说明:
 *      char c [IN]:	要输出的字符
 *	备注: 这个函数连同上个函数是在没有网络连接的时候使用输入输出字符用的
 */
typedef void (*XOSLOG_PUT_CHAR)(char c);

/*  功能描述: 命令处理函数
 *  参数说明:
 *      XOS_PSZ pszCommand [IN]:	命令字符串
 *      XOS_PSZ pszParam [IN]:		命令参数字符串
 *  返回值: 返回XOS_FALSE表示上层没有处理,还是需要内部处理,模块内部调用系统处理
 *			返回XOS_FALSE表示上层处理了这个命令,模块内部不需要再处理了.
 *  备注: 但Log控制台收到命令时,发现不是内部命令就调用这个函数让上层处理
 *		  在这个函数中要使用nprintf输出帮助信息,这样就不会带时间前缀
 */
typedef XOS_BOOL (*XOSLOG_ON_COMMAND_IN)(XOS_PSZ pszCommand, XOS_PSZ pszParam);

/*  功能描述: 帮助处理函数
 *  参数说明:
 *      XOS_PSZ pszParam [IN]:		帮助参数字符串,一般是具体输出某一命令的帮助
 *  返回值: 返回XOS_FALSE表示上层没有处理,还是需要内部处理,模块内部会打印出不支持
 *			返回XOS_FALSE表示上层处理了这个命令,模块内部不需要再处理了.
 *  备注: 但Log控制台收到帮助命令时,处理完内部帮助之后,调用这个函数让上层输出帮助
 *		  在这个函数中要使用nprintf输出帮助信息,这样就不会带时间前缀
 */
typedef XOS_BOOL (*XOSLOG_ON_HELP_IN)(XOS_PSZ pszParam);

#ifdef __cplusplus
extern "C" {
#endif

//如果要让Insure++检查printf的错误则屏蔽如下语句
#define printf xprintf
//如果要让Insure++检查xlprintf的错误则放开如下语句
//#define xlprintf printf

/*  功能描述: 帮助输出函数
 *  参数说明:
 *      const char *f [IN]:		打印格式
 *      ... [IN]:				打印数据变参数
 *  备注: 一般代码中打印的,基本都是关键异常情况,不可能情况和关键流程的打印
 *		  与xprintf的区别是这个函数不带时间前缀
 *		  如果不调用XOSLog_Open就直接调用此函数,则无网络功能,标准输出
 */
int nprintf(const char *f, ...);

/*  功能描述: 不带级别的日志输出函数
 *  参数说明:
 *      const char *f [IN]:		打印格式
 *      ... [IN]:				打印数据变参数
 *  备注: 一般代码中打印的,基本都是关键异常情况,不可能情况和关键流程的打印
 *		  与nprintf的区别是,这个函数带时间前缀
 *		  如果不调用XOSLog_Open就直接调用此函数,则无网络功能,标准输出
 */
int xprintf(const char *f, ...);

/*  功能描述: 带级别的日志输出函数
 *  参数说明:
 *      const char *f [IN]:		打印格式,里面必须包号级别的打印方法,
 *								推荐使用"[%s]: xxxxxx.\r\n"
 *      const car *level [IN]:	日志类别,第一个参数就是日志类别,字符串,取值如下:
 *									XLL_XX等宏定义
 *									其他自定义类别,取名规则如下:ModuleName_LogName
 *									最长63字节,全部使用小写
 *									例如: wdial_spdata,表示wdial模块的串口数据打印
 *      ... [IN]:				打印数据变参数
 *  备注: 带级别的日志输出,可以选择打开或者关闭,带时间前缀
 *		  如果不调用XOSLog_Open就直接调用此函数,则无网络功能,标准输出
 */
#ifndef xlprintf
int xlprintf(const char *f, ...);
#endif

/*  功能描述: 打印内存内容
 *  参数说明:
 *      unsigned char *pMem [IN]:	内存地址
 *      int iLen [IN]:				要打印的内存长度
 */
void XOSLog_PrintMem(unsigned char *pMem, int iLen);

/*  功能描述: 启动日志模块
 *  参数说明:
 *      XOS_U16 u16Port [IN]:			网络日志帧听端口,传入0表示不启动网络日志
 *      XOSLOG_GET_CHAR get_char [IN]:	无网络时调用的读取字符函数,
 *										可以传入NULL,表示无输入
 *      XOSLOG_PUT_CHAR put_char [IN]:	无网络时调用的输出字符函数,
 *										可以传入(XOSLOG_PUT_CHAR)&putchar,标准输出
 *      XOSLOG_ON_COMMAND_IN on_command_in [IN]: 控制台接收到的外部命令处理函数
 *												 传入NULL表示不需要处理外部命令
 *      XOSLOG_ON_HELP_IN on_help_in [IN]: 控制接收到帮助命令时的外部帮助输出函数
 *										   传入NULL表示没有外部帮助输出
 */
XOS_BOOL XOSLog_Open(XOS_U16 u16Port, 
					 XOSLOG_GET_CHAR get_char, XOSLOG_PUT_CHAR put_char, 
					 XOSLOG_ON_COMMAND_IN on_command_in, XOSLOG_ON_HELP_IN on_help_in);

/*  功能描述: 使能自定义类别日志
 *  参数说明:
 *      XOS_CPSZ cpszName [IN]:		自定义类别名称,取值见xlprintf中的说明
 *  备注: 如果能够使能的自定义日志类别满了,才会返回失败.一般不会出现这种情况
 */
XOS_BOOL XOSLog_EnableCustomLevel(XOS_CPSZ cpszName);

//录像日志
typedef void (*RECORD_LOG)(const char *pszAction, char cLogType);

/*  功能描述: 注册录像日志的回调
 *  参数说明:
 *      RECORD_LOG recordLog [IN]:	函数地址
 */
void XOSLog_RegisterRecord(RECORD_LOG recordLog);

/*  功能描述: 记录录像日志
 *  参数说明:
 *      XOS_CPSZ cpszAction [IN] : 日志动作内容
 *		char cLogType [IN]: 用户日志类型,参考上面的宏
 */
void XOSLog_Record(XOS_CPSZ cpszAction, char cLogType);

/*  功能描述: 关闭日志模块
 */
void XOSLog_Close();

#ifdef __cplusplus
}
#endif

#endif //__XOSLOG_H__
