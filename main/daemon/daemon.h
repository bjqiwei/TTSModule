#ifndef __DAEMON_H__
#define __DAEMON_H__
#include <log4cplus/thread/threads.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <sstream>
//全局初始化与退出
void global_init();
void global_exit();

//修改当前的路径
void change_current_dir();

//进程的后台化处理，(参数为service的参数值，对win32有效)
bool daemonize(const char* parms);

//查询进程是否已存在
bool process_is_exist();

//向已存在进程发送命令参数，如'snapshot等'
bool process_send_command(const char* parms);

//停止已存在的进程
bool process_stop();

//main的通用入口
bool main_running(int mode);

//重新加载配置
bool reload_config();
//win32下的入口
#ifdef WIN32
bool win32_WinMain();
#endif

//命令行参数
#if defined(_AIX)||defined(_LINUX)
extern int __argc;          /* count of cmd line args */
extern char ** __argv;      /* pointer to table of cmd line args */
#endif

#endif
