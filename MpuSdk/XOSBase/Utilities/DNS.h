/*  
 *  Copyright (c) 2005, 安徽创世科技有限公司
 *  All rights reserved.
 *  
 *  文件名称: DNS.h
 *  文件标识: 
 *  摘    要: DNS模块,内部维护了一个缓冲,解决了DNS查询的阻塞问题.		
 *  
 *  当前版本: 
 *  作    者: zhaoqq
 *  完成日期: 2006-9-28 8:49:17
 *  修正备注: 
 *  
 */

#ifndef __DNS_H__
#define __DNS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
使用方法:
		1.首先全局初始化函数DNS2IPInit
		2.然后调用其他函数使用其功能,无先后顺序之分
 */

/*  功能描述: 初始化通过域名获取IP的功能
 */
int DNS2IPInit();

/*  功能描述: 报告DSN模块内部状态
 *  参数说明:
 *      int (*pPrn)(const char *,...) [IN]:	使用的报告输出函数
 */
void DNS2IPReport(int (*pPrn)(const char *,...));

/*  功能描述: 通过域名得到它对应的IP地址
 *  参数说明:
 *      XOS_PSZ lpsz [IN]:	需要查询的域名
 *	返回值: 失败则返回0,成功返回获得的IP地址.
 *	备注:	可能不是一次就能够查成功的,失败之后需要反复尝试.
 */
XOS_U32 DNS2IP(XOS_CPSZ lpsz);

/*  功能描述: 从字符串地址中提取IP和端口
 *  参数说明:
 *      XOS_PSZ lpsz [IN]:		输入的URL,格式:"域名:端口"或者"IP:端口",例如:"www.crearo.com:8888","202.38.64.10:80"
 *								也可以省略端口,例如::"www.crearo.com","202.38.64.10",此时*pusPort的值不改变
 *      XOS_U32 *puiIP [OUT]:提取的IP地址,如果是域名,则在函数内部进行域名到IP的转换
 *      XOS_U16 *pusPort [OUT]:	提前的端口,如果URL不含端口,则*pusPort的值不改变
 *      XOS_BOOL bDefPort [IN]:		是否有缺省端口,如果为TRUE表示URL中可以不含端口,FALSE表示必须要端口.
 *	备注:	如果URL含有域名,则可能不是一次就能够查成功的,失败之后需要反复尝试.
 */
XOS_BOOL Addr2IPAndPort(XOS_CPSZ lpsz, XOS_U32 *puiIP, XOS_U16 *pusPort, XOS_BOOL bDefPort);

/*  功能描述: 使用IP和端口合成字符串地址
 *  参数说明:
 *      XOS_U32 uiIP [IN]:	32位IP地址
 *      XOS_U16 usPort [IN]:	整型端口
 *  返回值: 合成后的URL字符串
 *	备注:	这个函数不支持多线程重入
 */
XOS_PSZ IPAndPort2Addr(XOS_U32 uiIP, XOS_U16 usPort);

#ifdef __cplusplus
}
#endif

#endif //__DNS_H__
