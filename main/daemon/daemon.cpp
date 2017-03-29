#include "menu.h"
#include "daemon.h"
#include "version.h"
#include <stdio.h>
#include <iostream>
#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "httpserver.h"
#include "iFlyTTs.h"
#include "config.h"
#include "Request2TTS.h"
#include "TTS2Request.h"
#include "alarm.h"
#include <log4cplus/configurator.h>

#ifdef LINUX
///////////////////////////////////////////////////////
#ifndef si_int
#define si_int		si_value.sival_int
#define si_ptr		si_value.sival_ptr
#endif
#endif

//路径最大长度的常量定义
#ifndef MAX_PATH
#define MAX_PATH 256
#endif

//////////////////////////////////////////////////////////////
static bool  g_bExit = false;			//系统退出标识
#ifdef WIN32
//与windows系统服务相关
static SERVICE_STATUS_HANDLE hss = NULL;
static SERVICE_STATUS        sstatus;
static HANDLE g_hMail = NULL;
#else
static int  g_nSignalValue = 0; //外部信号通知的加载命令
const int const_signal_exit_id	= 999;	//退出信息携带的值
#endif
/////////////////////////////////////////////////////////
static HttpServer * g_pHttpServer = NULL;
static TTSModule * g_pTTS = NULL;
static Config * g_Config = NULL;
static HttpFileServer * g_pHttpFileServer = NULL;
static Request2TTS * g_Request2TTS = NULL;
static TTS2Request * g_TTS2Request = NULL;
static TAlarm	   * g_pAlarm = NULL;
static log4cplus::ConfigureAndWatchThread * pLogConfigThread = NULL;

#ifdef WIN32
#pragma comment(lib, "Advapi32.lib") 
#pragma comment(lib, "User32.lib")
#endif

static bool InitInstance()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");

	if (pLogConfigThread == NULL){
		pLogConfigThread = new log4cplus::ConfigureAndWatchThread(LOG4CPLUS_TEXT("../etc/log4cplus.properties"), 5 * 1000);
	}

	THttpRequest::CreateConnectLock();

	if (g_Config == NULL)
	{
		g_Config = new Config();
	}
	if(g_Config->LoadFile("../etc/config.xml") != tinyxml2::XML_SUCCESS){
		LOG4CPLUS_FATAL(log,"Load config file failed.");
	}

	if (g_pHttpServer == NULL)
	{
		g_pHttpServer = new HttpServer();
	}

	if (g_pAlarm == NULL)
	{
		g_pAlarm = new TAlarm(*g_pHttpServer);
	}
	g_pAlarm->Start(g_Config->GetSystemIP(),g_Config->GetSystemEntityID(),
		g_Config->GetAlarmCenterIP(),g_Config->GetAlarmCenterPort());

	g_pHttpServer->SetAllowIp(g_Config->GetAllowIp());
	if(!g_pHttpServer->StartHttpServer(g_Config->GetSystemIP(),g_Config->GetSystemPort())){
		g_pAlarm->PostAlarmMessage(TTSMod_Error_HttpListen,AL_Fatal,"start http server listen failed.", g_pHttpServer->GetLog());
	}

	if (g_Config->GetEnableHttpFileServer()){
		if (g_pHttpFileServer == NULL)
		{
			g_pHttpFileServer = new HttpFileServer();
		}
		//g_pHttpFileServer->SetAllowIp(g_Config->GetAllowIp());
		if(!g_pHttpFileServer->StartHttpServer(g_Config->GetHttpFileServerIP(),
			g_Config->GetHttpFileServerPort(),
			g_Config->GetHttpFileServerDocumentRoot())){
				g_pAlarm->PostAlarmMessage(TTSMod_Error_HttpListen,AL_Fatal,"start http file server listen failed.",g_Config->GetLog());
		}
	}

	if (g_pTTS == NULL)
	{
		g_pTTS = new iFlyTTs(*g_pAlarm);
	}
	if(!g_pTTS->StartTTSModule(g_Config->GetTTSLicenseNumber(), g_Config->GetTTSSerialNumber(), g_Config->GetTTSCode())){
		g_pAlarm->PostAlarmMessage(TTSMod_Error_TTSThread,AL_Fatal,"TTS创建主线程失败!", g_pTTS->GetLog());
	}

	if (g_Request2TTS == NULL)
	{
		g_Request2TTS = new Request2TTS(*g_pHttpServer, *g_Config, g_pTTS);
	}
	g_Request2TTS->Start();

	if (g_TTS2Request == NULL)
	{
		g_TTS2Request = new TTS2Request(*g_Config, g_pTTS, *g_pHttpServer);
	}
	g_TTS2Request->Start();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}
static bool ExitInstance()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	
	if (g_TTS2Request)
	{
		g_TTS2Request->Stop();
		delete g_TTS2Request;
		g_TTS2Request = NULL;
	}

	if (g_Request2TTS)
	{
		g_Request2TTS->Stop();
		delete g_Request2TTS;
		g_Request2TTS = NULL;
	}

	if (g_pTTS)
	{
		g_pAlarm->PostAlarmMessage(TTSMod_Warnning_Closed, AL_Common, "TTS模块正常关闭! 请求关注尽快重新启动!!!", g_pTTS->GetLog());
		g_pTTS->StopTTSModule();
		delete g_pTTS;
		g_pTTS = NULL;
	}

	if (g_pHttpFileServer)
	{
		g_pHttpFileServer->StopHttpServer();
		delete g_pHttpFileServer;
		g_pHttpFileServer = NULL;
	}

	if (g_pAlarm)
	{
		g_pAlarm->Stop();
		delete g_pAlarm;
		g_pAlarm = NULL;
	}

	if (g_pHttpServer)
	{
		g_pHttpServer->StopHttpServer();
		delete g_pHttpServer;
		g_pHttpServer = NULL;
	}

	if (g_Config)
	{
		delete g_Config;
		g_Config = NULL;
	}

	THttpRequest::DeleteConnectLock();

	if (pLogConfigThread){
		delete pLogConfigThread;
		pLogConfigThread = NULL;
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

/////////////////////////////////////////////////////////////
//获取应用程序的绝对路径
static const char* get_execute_path()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	static char path[MAX_PATH+64] = "";
	if (*path) 
		return path;
#ifdef WIN32
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	::GetModuleFileName(::GetModuleHandle(NULL), path, MAX_PATH);

	assert(strrchr(path, '\\'));
	*strrchr(path, '\\') = 0;
#else // NIX OR _LINUX
#ifdef _LINUX
	char buff[MAX_PATH+64] = "";
	sprintf(buff,"/proc/%d/exe",getpid());

	getcwd(path, MAX_PATH);
	if (readlink(buff, path, MAX_PATH) == -1)
		return path;

	assert(strrchr(path, '/'));
	*strrchr(path, '/') = 0;
#else //*NIX
		//保存初始当前目录
	char initdir[MAX_PATH+64] = "";
	getcwd(initdir, MAX_PATH);
	strcpy(path, initdir);
	
	char buff[MAX_PATH+64] = "";
	const char* symfile = __argv[0];
	while (true)
	{
		strcat(path, "/");
		if (symfile[0] == '/')
			strcpy(path, symfile);
		else
			strcat(path, symfile);

		assert(strrchr(path, '/'));
		*strrchr(path, '/') = 0;
		
		//当为非符号连接时，返回
		if (readlink(symfile, buff, MAX_PATH) == -1)
			break;	
		
		chdir(path);
		getcwd(path, MAX_PATH);
		symfile = buff;
	}
	//得到可执行目录的最短路径名
	chdir(path);
	getcwd(path, MAX_PATH);

	//恢复原始当前路径
	chdir(initdir);
#endif
#endif
	LOG4CPLUS_INFO(log,"execute path=" << path);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return path;
}
#ifdef WIN32
//打开进程通信句柄
static void create_mailslot()
{	
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	if (g_hMail != INVALID_HANDLE_VALUE){
		return;
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "%s/%s.pid", get_execute_path(), APPLICATION_NAME);

	g_hMail = CreateMailslot(pidfile, 32, 0, NULL);
	LOG4CPLUS_DEBUG(log, "CreateMailslot:" << pidfile << (g_hMail != INVALID_HANDLE_VALUE?"Success":"Failed"));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}

//关闭进程通信句柄
static void destroy_mailslot()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	if (g_hMail == INVALID_HANDLE_VALUE)
		return;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	::CloseHandle(g_hMail);
	g_hMail = INVALID_HANDLE_VALUE;
	LOG4CPLUS_DEBUG(log, "CloseMailslot");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}

//执行mailslot的进程通知内容
static void read_mailslot(void)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	DWORD len = 0;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	::GetMailslotInfo(g_hMail, 0, &len, 0, NULL);
	if (len == MAILSLOT_NO_MESSAGE){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}

	char buffer[32] = "";
	if (!::ReadFile(g_hMail, buffer, 31, &len, NULL)){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}

	int ch = buffer[1] ? get_short_command(buffer) : buffer[0];

	LOG4CPLUS_INFO(log, "recv sig: ["<< buffer<< "] ==> (\"" << get_long_command(ch) << "\")");
	std::cout << "recv sig: ["<< buffer<< "] ==> (\"" << get_long_command(ch) << "\")" << std::endl;
	run_menu_command(ch);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}

//console的退出回调函数
static BOOL WINAPI console_control_handler(DWORD control)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	switch (control)
	{
	case CTRL_LOGOFF_EVENT:
		LOG4CPLUS_INFO(log, "recv logout.event(" << control << "), ignore");
		return FALSE;
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		LOG4CPLUS_INFO(log, "recv ctrl-c.event(" << control << "), exit");
		break;
	case CTRL_CLOSE_EVENT:
		LOG4CPLUS_INFO(log, "recv close.event(" << control << "), exit");
		break;
	case CTRL_SHUTDOWN_EVENT:
		LOG4CPLUS_INFO(log, "recv sysdown.event(" << control << "), exit");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}

	if (g_bExit)
	{
		LOG4CPLUS_INFO(log, "recv second.exit.singal:" << control << ", killed");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}
	g_bExit = true;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return TRUE;
}

//win.server的回调(sc)
static void WINAPI service_control_hander( DWORD control )
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	switch ( control )
	{
	case SERVICE_CONTROL_STOP:
		sstatus.dwCurrentState = SERVICE_STOP_PENDING;
		sstatus.dwCheckPoint = 0;
		g_bExit = true;
		break;
	case SERVICE_CONTROL_INTERROGATE:  /* return status immediately */
		break;
	}
	SetServiceStatus( hss, &sstatus );
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}

//win.server的入口函数
static void WINAPI win32_ServiceMain(DWORD Argc, LPTSTR* Argv)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	hss = RegisterServiceCtrlHandler( APPLICATION_NAME, (LPHANDLER_FUNCTION)service_control_hander );
	if ( !hss ){
		LOG4CPLUS_ERROR(log,"RegisterServiceCtrlHandler Error.");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}

	LOG4CPLUS_TRACE(log, "RegisterServiceCtrlHandler");

	sstatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	sstatus.dwCurrentState = SERVICE_RUNNING;
	sstatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	sstatus.dwWin32ExitCode = NO_ERROR;
	sstatus.dwCheckPoint = 0;
	sstatus.dwWaitHint = 1000;
	SetServiceStatus( hss, &sstatus );
	LOG4CPLUS_INFO(log, "service.daemon started.");
	main_running(RUNMODE_SERVICE);

	sstatus.dwWin32ExitCode = 0;
	sstatus.dwServiceSpecificExitCode = 0;
	sstatus.dwCurrentState = SERVICE_STOPPED;
	sstatus.dwCheckPoint = 0;
	SetServiceStatus( hss, &sstatus );
	LOG4CPLUS_INFO(log, "service.daemon stopped.");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}
#else
//检查正在运行的pid，否则返回0
static long get_running_pid()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	char pidfile[MAX_PATH+64] = "";
	const char* exepath = get_execute_path();
	sprintf(pidfile, "%s/%s.pid", exepath, APPLICATION_NAME);
	
	FILE *fp = NULL;
	fp = fopen(pidfile, "r");
	if (fp == NULL){
		LOG4CPLUS_DEBUG(log,"Can not open pid file:" << pidfile);
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return 0;
	}
	
	long pid = 0;
	fscanf(fp, "%ld", &pid);
	fclose(fp);
	fp = NULL;
	
	//用kill(0)来检测进程是否存在
	if (pid && kill(pid, 0) == -1)
	{
		LOG4CPLUS_DEBUG(log,"not process pid =" << pid );
		unlink(pidfile);
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return 0;
	}
	
	//判断是否为本执行程序
	char cwdfile[64] = "";
	char path[MAX_PATH+1] = "";
	sprintf(cwdfile, "/proc/%ld/cwd", pid);
	readlink(cwdfile, path, MAX_PATH);
	if (strncmp(exepath, path, strlen(exepath)) == 0){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return pid;
	}
	
	//无效的pid文件
	LOG4CPLUS_DEBUG(log,"Invalid pid file:" << pidfile << "return pid=0" );
	unlink(pidfile);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return 0;
}

//写pid到共享文件中
static bool write_pidfile()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "%s/%s.pid", get_execute_path(), APPLICATION_NAME);
	
	FILE *fp = NULL;
	fp = fopen(pidfile, "w");
	if (fp == NULL){
		LOG4CPLUS_DEBUG(log,"Can not open pid file:" << pidfile);
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}
	
	fprintf(fp, "%ld", (long)getpid());
	LOG4CPLUS_DEBUG(log, "write pidfile:" << getpid());
	fclose(fp);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

static bool delete_pidfile()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "%s/%s.pid", get_execute_path(), APPLICATION_NAME);
	
	unlink(pidfile);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

////////////////////////////////////
//信号处理函数
//通知退出函数
__attribute__((unused))
static void sig_term(int signum)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	switch (signum)
	{
	case SIGABRT:
	case SIGINT:
		LOG4CPLUS_INFO(log, "get term.signal(" << signum << "), settimer to exiting...");
		
		struct itimerval timevalue;
		memset(&timevalue, 0, sizeof(timevalue));
		timevalue.it_value.tv_sec = 5;
		timevalue.it_value.tv_usec = 0;
		timevalue.it_interval.tv_sec = 0;
		timevalue.it_interval.tv_usec = 0;
		setitimer(ITIMER_REAL, &timevalue, NULL);
		break;
	case SIGALRM:
		if (!g_bExit)
			return;
		LOG4CPLUS_WARN(log, "got SIGALARM("<< signum << "), exit.timerout");
		raise(SIGKILL);
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	case SIGTERM:
	case SIGQUIT:
	default:
		LOG4CPLUS_INFO(log, "got SIGTERM(" << signum << "), exiting");
		break;
	}

	if (g_bExit)
	{
		LOG4CPLUS_WARN(log, "recv second.exit.singal(" << signum << "), killed.");
		raise(SIGALRM);
	}
	
	g_bExit = true;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}

/*系统命令配置函数*/
static void sig_proc(int signum, siginfo_t *info, void *)
{		
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (signum != SIGUSR1)
	{
		LOG4CPLUS_WARN(log, "recv unkown signal(" << signum << "), ignored.");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}

	//检测是否退出
	if (const_signal_exit_id == info->si_int)
	{
		LOG4CPLUS_INFO(log, "recv[USR1] stop-service(" << info->si_int<<") from pid("<< info->si_pid<<") uid("<< info->si_uid<< ")");
		g_bExit = true;
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}
		
	//普通的命令消息
	LOG4CPLUS_INFO(log, "recv[USR1], command("<< info->si_int<<"), from pid("<< info->si_pid<<") uid("<< info->si_uid<<")");
	g_nSignalValue = info->si_int;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}

__attribute__((unused))
static bool set_signal_handler(int signum,  void (*handler)(int))
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	
	struct sigaction sa;
	
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,signum);
	sa.sa_flags = SA_RESTART;
	
	if (sigaction(signum, &sa, NULL) < 0)
	{
		LOG4CPLUS_ERROR(log,"Can't Catch SIG(" << signum << "), err(" << strerror(errno) << ")");
		return false;
	}else
		LOG4CPLUS_INFO(log, __FUNCTION__ << " " << signum);
	return true;	
}

static bool set_signal_action(int signum, void (*action)(int, siginfo_t *, void *))
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	struct sigaction sa;
	
	sa.sa_handler = NULL;
    sa.sa_sigaction = action;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, signum);
	sa.sa_flags = SA_RESTART|SA_SIGINFO;
	
	if (sigaction(signum, &sa, NULL) < 0)
	{
		LOG4CPLUS_ERROR(log,"Can't Catch SIG(" << signum<< "), err(" << strerror(errno)<< ")");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

//设置信号处理
static void set_signal()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	/*忽略处理的signal*/
	signal(SIGHUP, SIG_IGN); 	//终端挂起或者控制进程终止(1)
	signal(SIGPIPE, SIG_IGN); 	//管道破裂(pipe/socket)(13)
	signal(SIGCHLD, SIG_IGN); 	//子进程结束信号(17)
	signal(SIGURG, SIG_IGN); 	//socket有紧急数据到达(23)
	signal(SIGUSR2, SIG_IGN); 	//用户信号2
	
	//set_signal_handler(SIGABRT, sig_term);//ABORT(6)
	//set_signal_handler(SIGTSTP, sig_term);	//停止进程的运行(20) [ctrl+'z']
	
#ifdef _DEBUG
	set_signal_handler(SIGTERM, sig_term);	//终止信号(15) 
	set_signal_handler(SIGQUIT, sig_term);	//退出(3) [ctrl+'\']
	set_signal_handler(SIGINT, sig_term);	//键盘中断(2) [ctrl+'c']
	set_signal_handler(SIGALRM, sig_term);  //时钟定时信号(14)
#else
	signal(SIGTERM, SIG_IGN);	//终止信号(15) 
	signal(SIGQUIT, SIG_IGN);	//退出(3) [ctrl+'\']
	signal(SIGINT, SIG_IGN);	//键盘中断(2) [ctrl+'c']
	signal(SIGALRM, SIG_IGN);   //时钟定时信号(14)
#endif

	set_signal_action(SIGUSR1, sig_proc);	//用户信号1(10)
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

//取消信号的设置
static void unset_signal()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	signal(SIGHUP, SIG_DFL);	//终端挂起或者控制进程终止(1)
	signal(SIGPIPE, SIG_DFL);	//管道破裂(pipe/socket)(13)
	signal(SIGCHLD, SIG_DFL);	//子进程结束信号(17)
	signal(SIGURG, SIG_DFL);	//socket有紧急数据到达(23)
	signal(SIGALRM, SIG_DFL);	//时钟定时信号(14)

	signal(SIGTERM, SIG_DFL);	//终止信号(15) 
	signal(SIGINT, SIG_DFL);	//键盘中断(2) [ctrl+'c']
	signal(SIGQUIT, SIG_DFL);	//退出(3) [ctrl+'\']
	signal(SIGABRT, SIG_DFL);	//ABORT(6)
	signal(SIGTSTP, SIG_DFL);	//停止进程的运行(20) [ctrl+'z']
	
	signal(SIGUSR1, SIG_DFL);	//用户信号1(10)
	signal(SIGUSR2, SIG_DFL);	//用户信号2(12)	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

static void read_signal(void)
{
	//信号量执行
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (g_nSignalValue <= 0){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}

	LOG4CPLUS_INFO(log,"recv sig: ["<< g_nSignalValue <<"] ==> (\"" << get_long_command(g_nSignalValue) << "\")");
	std::cout << "recv sig: ["<< g_nSignalValue <<"] ==> (\"" << get_long_command(g_nSignalValue) << "\")" << std::endl;
	run_menu_command(g_nSignalValue);
	
	g_nSignalValue = 0;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return;
}
#endif
////////////////////////////////////////////////////////////////////
//全局初始化与析构
void global_exit()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void global_init()
{	
	//设置标题栏显示　
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	HWND handle = ::GetConsoleWindow();
	if (handle)
	{
		char buffer[MAX_PATH+64] = "";
		char buffer1[MAX_PATH+64] = "";
		::GetWindowText(handle, buffer1, MAX_PATH);
		sprintf(buffer, "%s - ( %s-%s )",
			buffer1, APPLICATION_NAME, APPLICATION_VERSION);
		::SetWindowText(handle, buffer);
	}
#endif
	atexit(global_exit);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void change_current_dir()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	chdir(get_execute_path());
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

//判断进程是否存在
bool process_is_exist()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "%s/%s.pid", get_execute_path(), APPLICATION_NAME);

	HANDLE hMail = ::CreateFile(pidfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//当创建失败，表示主进程不存在
	if (hMail == INVALID_HANDLE_VALUE){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}

	::CloseHandle(hMail);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
#else
	return (get_running_pid() != 0);
#endif
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

//进程命令加载
bool process_send_command(const char* parms)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	LOG4CPLUS_DEBUG(log,"send command:" << parms);
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "/%s/%s.pid", get_execute_path(), APPLICATION_NAME);

	HANDLE hMail = ::CreateFile(pidfile,
		GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hMail == INVALID_HANDLE_VALUE)
#else
	long pid = get_running_pid();
	
	if (pid == 0)
#endif
	{
		LOG4CPLUS_ERROR(log,"send command("<< parms << ") failed.");
		std::cout << "send command("<< parms << ") failed." <<std::endl;
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}
#ifdef WIN32
	DWORD size = strlen(parms);
	::WriteFile(hMail, parms, size, &size, NULL);

	LOG4CPLUS_DEBUG(log,"send command(" << parms << ") succ");
	std::cout << "send command(" << parms << ") succ" << std::endl;
	::CloseHandle(hMail);
#else
	union sigval sv;
	sv.sival_int = get_short_command(parms);
	sigqueue(pid, SIGUSR1, sv);
	LOG4CPLUS_DEBUG(log,"send command(" << parms << ") to pid(" << pid << ") succ");
	std::cout << "send command(" << parms << ") to pid(" << pid << ") succ" << std::endl;
#endif
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

//通知进程停止
bool process_stop()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "%s/%s.pid", get_execute_path(), APPLICATION_NAME);

	HANDLE hMail = ::CreateFile(pidfile,
		GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hMail == INVALID_HANDLE_VALUE){
#else
	long pid = get_running_pid();
	
	if (pid == 0){
#endif
		LOG4CPLUS_ERROR(log,"Can not get current process id.");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return true;
	}
	LOG4CPLUS_INFO(log, APPLICATION_NAME << " is running, stopping...");

//通知退出, SIGTERM
#ifdef WIN32
	DWORD size = strlen("quit");
	::WriteFile(hMail, "quit", size, &size, NULL);
	LOG4CPLUS_DEBUG(log,"send \"quit\" to mailslot.");
	::CloseHandle(hMail);
#else
	union sigval sv;
	sv.sival_int = const_signal_exit_id; //25ms
	sigqueue(pid, SIGUSR1, sv);
	LOG4CPLUS_DEBUG(log,"send sig id:" << const_signal_exit_id << " to:" <<  pid);
#endif

	//等待退出(30s)
	int count = 30;
#ifdef WIN32
	do {
		Sleep(1000);
		printf(".");
		fflush(stdout);
	} while (--count && process_is_exist());
#else
	do {
		usleep(1000*1000);
		printf(".");
		fflush(stdout);
	} while (--count && kill(pid, 0) != -1);
#endif
	
	//未退出，则直接kill(9)
	if (count == 0){
#ifdef WIN32
		LOG4CPLUS_ERROR(log, "stop pid(" << GetCurrentProcessId()<< ")timeout, and killed");
#else
		kill(pid, SIGKILL);
		LOG4CPLUS_ERROR(log,"stop pid(" << pid<< ")timeout, and killed");
#endif
	}
	else
		LOG4CPLUS_INFO(log, "\t\t[off]");

	fflush(stdout);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

//进行deamon处理(进行后台处理)
bool daemonize(const char* parms)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	//释放控制台
	::FreeConsole();

	//后台运行子进程，直接返回
	if (strcmp(parms, "noconsole") == 0){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return true;
	}

	//windows.server运行
	if (strcmp(parms, "daemon") == 0)
	{
		/* run as service under windows NT */
		static SERVICE_TABLE_ENTRY service_table[] = {
			{ "", (LPSERVICE_MAIN_FUNCTION) win32_ServiceMain },
			{ NULL, NULL }
		};

		LOG4CPLUS_INFO(log, "Start service control dispatcher, wait...");
		if ( !StartServiceCtrlDispatcher( service_table ) )
		{
			LOG4CPLUS_ERROR(log, "Cannot start service control dispatcher; error "<< std::hex <<  GetLastError());
			LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
			exit(1);
			return false;
		}
		//主进程退出
		LOG4CPLUS_DEBUG(log, "service control dispatchered");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(0);
		return true;
	}

	//创建子进程,控制台后台运行
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof(si ) );
	si.cb = sizeof(si);

	char command_line[MAX_PATH+64] = "";

	if ( __argv[0][1] == ':' )
		command_line[0] = 0;
	else
	{
		::GetCurrentDirectory( MAX_PATH, command_line );
		strcat( command_line, "\\" );
	}
	strcat( command_line, __argv[0] );
	strcat( command_line, " --service=noconsole" );

	if (!CreateProcess( NULL, command_line, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi ))
	{
		//失败
		LOG4CPLUS_ERROR(log, "Cannot create process; error " << std::hex << GetLastError());
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(1);
		return false;
	}

	::CloseHandle( pi.hThread );
	::CloseHandle( pi.hProcess );
#else
	//1、第一次调用fork()，让父进程退出
	//   子进程获得一个新的进程ID，并继承了父进程的进程组ID
	switch (fork())
	{
	case 0:	 //返回值是0，表明是子进程，继续在后台运行
		break;
	case -1: //出错
		LOG4CPLUS_ERROR(log,"fork() failed");
		//perror("fork() failed");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(-1);
		return false;
	default: //返回值不为0，表明是父进程部分，结束
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(0); 
		return true;
	}

	//2、调用setsid创建一个新的session
	//   使自己成为新session和新进程组的leader，并使进程没有控制终端(tty)
	setsid();

	//3、第二次调用fork()，确保即使当进程打开终端设备时，也无法获得控制终端，
	//   这是因为由session leader派生的子进程一定不是session leader，也就无法获得控制终端；
	//switch (fork())
	//{
	//case 0:	 //返回值是0，表明是子进程，继续在后台运行
	//	break;
	//case -1: //出错
	//	LOG4CPLUS_ERROR(log,"fork() failed");
	//	//perror("fork() failed");
	//	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	//	exit(-1);
	//	return false;
	//default: //返回值不为0，表明是父进程部分，结束
	//	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	//	exit(0); 
	//	return true;
	//}
	
	//4、更改守护进程的工作目录，应该由调用者负责当前目录问题
	//if (chdir("/")) < 0)
	//	exit(-1);

	//5、 设置文件创建mask为0，避免创建文件时权限的影响
	umask(0);

	//6、关闭不需要的打开文件描述符，主要为stdin/stdout/stderr
	//struct rlimit rl;
	//if (getrlimit(RLIMIT_NOFILE,&rl) < 0)
	//	exit(1);

	//if (rl.rlim_max == RLIM_INFINITY)
	//	rl.rlim_max = 1024;

	//unsigned int i = 0;
	//for (i=0; i<rl.rlim_max; i++)
	//	close(i);
	close(0);
	close(1);
	close(2);
	//7、将stdin/stdout/stderr指向/dev/null
	open("/dev/null", O_RDONLY);    
	open("/dev/null", O_RDWR);   
	open("/dev/null", O_RDWR);
#endif

	//主进程退出
	LOG4CPLUS_DEBUG(log,"service daemonized");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

//////////////////////////////////
bool main_running(int mode)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	#ifdef WIN32
	//打开mailslot通信
	create_mailslot();
	#else
	//写入pid
	write_pidfile();
	#endif

	//应用模块启动
	LOG4CPLUS_INFO(log, "start service, wait...");
	if (!InitInstance())
	{
		LOG4CPLUS_ERROR(log," service start failed.");
#ifdef WIN32
		destroy_mailslot();
#else
		delete_pidfile();
#endif
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return false;
	}
	LOG4CPLUS_INFO(log,"\n\n\n+++++++++++++++++start service success......+++++++++++++++++\t\t[on]");

	//控制台模式下，控制台窗口
	if (mode == RUNMODE_INTETACTIVE)
	{
	#ifdef WIN32
		//设置控制台窗口的事件捕获
		::SetConsoleCtrlHandler(console_control_handler, TRUE);
	#endif
		//显示交互式帮助菜单
		show_commands();
		fflush(stdout);
	}	

#ifndef WIN32
	set_signal();
#endif
	while (!g_bExit)
	{
		//先处理其它进行的通知
#ifdef WIN32
		read_mailslot();
#else
		read_signal();
#endif

		if (mode != RUNMODE_INTETACTIVE)
		{
		
#ifdef WIN32
			Sleep(100);
#else
#ifdef _LINUX
			pause();
#else
			usleep(100000);
#endif
#endif
			LOG4CPLUS_TRACE(log,  " cmd interface loop.");
			continue;
		}
		
		//用户输入
		char input[MAX_PATH] = "";
		std::cin.getline(input,MAX_PATH-1);
		if ( input == NULL || *input == 0)
		{
#ifdef WIN32
			Sleep(100);
#else
			usleep(100000);
#endif
			LOG4CPLUS_TRACE(log,  " cmd interface loop.");
			continue;
		}

		int ch = input[1] ? get_short_command(input) : input[0];
		LOG4CPLUS_DEBUG(log,"recv: ["<< input<< "] ==> (\"" << (get_long_command(ch))<< "\")");
		std::cout << "recv: ["<< input<< "] ==> (\"" << (get_long_command(ch))<< "\")" << std::endl;

		switch (ch)
		{
		case 'q':
			g_bExit = true;
			break;
		case 'h':
			show_commands();
			break;
		case 'v':
			print_version();
			break;
		default:
			run_menu_command(ch);
			break;
		}
		//fflush( stdout );
	}  //end while
	
	//应用模块退出
	LOG4CPLUS_INFO(log, "stop service, wait...");
	ExitInstance();
	
	LOG4CPLUS_INFO(log,"stop service, wait......\t\t[off]");
	
#ifdef WIN32
	//放弃对控制台的事件捕获
	if (mode == RUNMODE_INTETACTIVE)
		::SetConsoleCtrlHandler(NULL, FALSE);
	//删除mailslot
	destroy_mailslot();
#else
	unset_signal();
	delete_pidfile();
#endif
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

bool reload_config(){
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	if(g_Config->LoadFile("../etc/config.xml") != tinyxml2::XML_SUCCESS){
		LOG4CPLUS_ERROR(log,"Load config file failed.");
		return false;
	}
	g_Config->Clear();
	g_pHttpServer->SetAllowIp(g_Config->GetAllowIp());
	g_pTTS->StopTTSModule();
	if(!g_pTTS->StartTTSModule(g_Config->GetTTSLicenseNumber(), g_Config->GetTTSSerialNumber(), g_Config->GetTTSCode())){
		g_pAlarm->PostAlarmMessage(TTSMod_Error_TTSThread,AL_Fatal,"TTS创建主线程失败!", g_pTTS->GetLog());
	}
	g_pAlarm->Stop();
	g_pAlarm->Start(g_Config->GetSystemIP(),g_Config->GetSystemEntityID(),
		g_Config->GetAlarmCenterIP(),g_Config->GetAlarmCenterPort());

	return true;
}