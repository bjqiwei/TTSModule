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

//·����󳤶ȵĳ�������
#ifndef MAX_PATH
#define MAX_PATH 256
#endif

//////////////////////////////////////////////////////////////
static bool  g_bExit = false;			//ϵͳ�˳���ʶ
#ifdef WIN32
//��windowsϵͳ�������
static SERVICE_STATUS_HANDLE hss = NULL;
static SERVICE_STATUS        sstatus;
static HANDLE g_hMail = NULL;
#else
static int  g_nSignalValue = 0; //�ⲿ�ź�֪ͨ�ļ�������
const int const_signal_exit_id	= 999;	//�˳���ϢЯ����ֵ
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
		g_pAlarm->PostAlarmMessage(TTSMod_Error_TTSThread,AL_Fatal,"TTS�������߳�ʧ��!", g_pTTS->GetLog());
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
		g_pAlarm->PostAlarmMessage(TTSMod_Warnning_Closed, AL_Common, "TTSģ�������ر�! �����ע������������!!!", g_pTTS->GetLog());
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
//��ȡӦ�ó���ľ���·��
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
		//�����ʼ��ǰĿ¼
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
		
		//��Ϊ�Ƿ�������ʱ������
		if (readlink(symfile, buff, MAX_PATH) == -1)
			break;	
		
		chdir(path);
		getcwd(path, MAX_PATH);
		symfile = buff;
	}
	//�õ���ִ��Ŀ¼�����·����
	chdir(path);
	getcwd(path, MAX_PATH);

	//�ָ�ԭʼ��ǰ·��
	chdir(initdir);
#endif
#endif
	LOG4CPLUS_INFO(log,"execute path=" << path);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return path;
}
#ifdef WIN32
//�򿪽���ͨ�ž��
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

//�رս���ͨ�ž��
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

//ִ��mailslot�Ľ���֪ͨ����
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

//console���˳��ص�����
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

//win.server�Ļص�(sc)
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

//win.server����ں���
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
//����������е�pid�����򷵻�0
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
	
	//��kill(0)���������Ƿ����
	if (pid && kill(pid, 0) == -1)
	{
		LOG4CPLUS_DEBUG(log,"not process pid =" << pid );
		unlink(pidfile);
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return 0;
	}
	
	//�ж��Ƿ�Ϊ��ִ�г���
	char cwdfile[64] = "";
	char path[MAX_PATH+1] = "";
	sprintf(cwdfile, "/proc/%ld/cwd", pid);
	readlink(cwdfile, path, MAX_PATH);
	if (strncmp(exepath, path, strlen(exepath)) == 0){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return pid;
	}
	
	//��Ч��pid�ļ�
	LOG4CPLUS_DEBUG(log,"Invalid pid file:" << pidfile << "return pid=0" );
	unlink(pidfile);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return 0;
}

//дpid�������ļ���
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
//�źŴ�����
//֪ͨ�˳�����
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

/*ϵͳ�������ú���*/
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

	//����Ƿ��˳�
	if (const_signal_exit_id == info->si_int)
	{
		LOG4CPLUS_INFO(log, "recv[USR1] stop-service(" << info->si_int<<") from pid("<< info->si_pid<<") uid("<< info->si_uid<< ")");
		g_bExit = true;
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return;
	}
		
	//��ͨ��������Ϣ
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

//�����źŴ���
static void set_signal()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	/*���Դ����signal*/
	signal(SIGHUP, SIG_IGN); 	//�ն˹�����߿��ƽ�����ֹ(1)
	signal(SIGPIPE, SIG_IGN); 	//�ܵ�����(pipe/socket)(13)
	signal(SIGCHLD, SIG_IGN); 	//�ӽ��̽����ź�(17)
	signal(SIGURG, SIG_IGN); 	//socket�н������ݵ���(23)
	signal(SIGUSR2, SIG_IGN); 	//�û��ź�2
	
	//set_signal_handler(SIGABRT, sig_term);//ABORT(6)
	//set_signal_handler(SIGTSTP, sig_term);	//ֹͣ���̵�����(20) [ctrl+'z']
	
#ifdef _DEBUG
	set_signal_handler(SIGTERM, sig_term);	//��ֹ�ź�(15) 
	set_signal_handler(SIGQUIT, sig_term);	//�˳�(3) [ctrl+'\']
	set_signal_handler(SIGINT, sig_term);	//�����ж�(2) [ctrl+'c']
	set_signal_handler(SIGALRM, sig_term);  //ʱ�Ӷ�ʱ�ź�(14)
#else
	signal(SIGTERM, SIG_IGN);	//��ֹ�ź�(15) 
	signal(SIGQUIT, SIG_IGN);	//�˳�(3) [ctrl+'\']
	signal(SIGINT, SIG_IGN);	//�����ж�(2) [ctrl+'c']
	signal(SIGALRM, SIG_IGN);   //ʱ�Ӷ�ʱ�ź�(14)
#endif

	set_signal_action(SIGUSR1, sig_proc);	//�û��ź�1(10)
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

//ȡ���źŵ�����
static void unset_signal()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	signal(SIGHUP, SIG_DFL);	//�ն˹�����߿��ƽ�����ֹ(1)
	signal(SIGPIPE, SIG_DFL);	//�ܵ�����(pipe/socket)(13)
	signal(SIGCHLD, SIG_DFL);	//�ӽ��̽����ź�(17)
	signal(SIGURG, SIG_DFL);	//socket�н������ݵ���(23)
	signal(SIGALRM, SIG_DFL);	//ʱ�Ӷ�ʱ�ź�(14)

	signal(SIGTERM, SIG_DFL);	//��ֹ�ź�(15) 
	signal(SIGINT, SIG_DFL);	//�����ж�(2) [ctrl+'c']
	signal(SIGQUIT, SIG_DFL);	//�˳�(3) [ctrl+'\']
	signal(SIGABRT, SIG_DFL);	//ABORT(6)
	signal(SIGTSTP, SIG_DFL);	//ֹͣ���̵�����(20) [ctrl+'z']
	
	signal(SIGUSR1, SIG_DFL);	//�û��ź�1(10)
	signal(SIGUSR2, SIG_DFL);	//�û��ź�2(12)	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

static void read_signal(void)
{
	//�ź���ִ��
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
//ȫ�ֳ�ʼ��������
void global_exit()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void global_init()
{	
	//���ñ�������ʾ��
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

//�жϽ����Ƿ����
bool process_is_exist()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	char pidfile[MAX_PATH+64] = "";
	sprintf(pidfile, "%s/%s.pid", get_execute_path(), APPLICATION_NAME);

	HANDLE hMail = ::CreateFile(pidfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//������ʧ�ܣ���ʾ�����̲�����
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

//�����������
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

//֪ͨ����ֹͣ
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

//֪ͨ�˳�, SIGTERM
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

	//�ȴ��˳�(30s)
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
	
	//δ�˳�����ֱ��kill(9)
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

//����deamon����(���к�̨����)
bool daemonize(const char* parms)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
#ifdef WIN32
	//�ͷſ���̨
	::FreeConsole();

	//��̨�����ӽ��̣�ֱ�ӷ���
	if (strcmp(parms, "noconsole") == 0){
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return true;
	}

	//windows.server����
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
		//�������˳�
		LOG4CPLUS_DEBUG(log, "service control dispatchered");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(0);
		return true;
	}

	//�����ӽ���,����̨��̨����
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
		//ʧ��
		LOG4CPLUS_ERROR(log, "Cannot create process; error " << std::hex << GetLastError());
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(1);
		return false;
	}

	::CloseHandle( pi.hThread );
	::CloseHandle( pi.hProcess );
#else
	//1����һ�ε���fork()���ø������˳�
	//   �ӽ��̻��һ���µĽ���ID�����̳��˸����̵Ľ�����ID
	switch (fork())
	{
	case 0:	 //����ֵ��0���������ӽ��̣������ں�̨����
		break;
	case -1: //����
		LOG4CPLUS_ERROR(log,"fork() failed");
		//perror("fork() failed");
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(-1);
		return false;
	default: //����ֵ��Ϊ0�������Ǹ����̲��֣�����
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		exit(0); 
		return true;
	}

	//2������setsid����һ���µ�session
	//   ʹ�Լ���Ϊ��session���½������leader����ʹ����û�п����ն�(tty)
	setsid();

	//3���ڶ��ε���fork()��ȷ����ʹ�����̴��ն��豸ʱ��Ҳ�޷���ÿ����նˣ�
	//   ������Ϊ��session leader�������ӽ���һ������session leader��Ҳ���޷���ÿ����նˣ�
	//switch (fork())
	//{
	//case 0:	 //����ֵ��0���������ӽ��̣������ں�̨����
	//	break;
	//case -1: //����
	//	LOG4CPLUS_ERROR(log,"fork() failed");
	//	//perror("fork() failed");
	//	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	//	exit(-1);
	//	return false;
	//default: //����ֵ��Ϊ0�������Ǹ����̲��֣�����
	//	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	//	exit(0); 
	//	return true;
	//}
	
	//4�������ػ����̵Ĺ���Ŀ¼��Ӧ���ɵ����߸���ǰĿ¼����
	//if (chdir("/")) < 0)
	//	exit(-1);

	//5�� �����ļ�����maskΪ0�����ⴴ���ļ�ʱȨ�޵�Ӱ��
	umask(0);

	//6���رղ���Ҫ�Ĵ��ļ�����������ҪΪstdin/stdout/stderr
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
	//7����stdin/stdout/stderrָ��/dev/null
	open("/dev/null", O_RDONLY);    
	open("/dev/null", O_RDWR);   
	open("/dev/null", O_RDWR);
#endif

	//�������˳�
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
	//��mailslotͨ��
	create_mailslot();
	#else
	//д��pid
	write_pidfile();
	#endif

	//Ӧ��ģ������
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

	//����̨ģʽ�£�����̨����
	if (mode == RUNMODE_INTETACTIVE)
	{
	#ifdef WIN32
		//���ÿ���̨���ڵ��¼�����
		::SetConsoleCtrlHandler(console_control_handler, TRUE);
	#endif
		//��ʾ����ʽ�����˵�
		show_commands();
		fflush(stdout);
	}	

#ifndef WIN32
	set_signal();
#endif
	while (!g_bExit)
	{
		//�ȴ����������е�֪ͨ
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
		
		//�û�����
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
	
	//Ӧ��ģ���˳�
	LOG4CPLUS_INFO(log, "stop service, wait...");
	ExitInstance();
	
	LOG4CPLUS_INFO(log,"stop service, wait......\t\t[off]");
	
#ifdef WIN32
	//�����Կ���̨���¼�����
	if (mode == RUNMODE_INTETACTIVE)
		::SetConsoleCtrlHandler(NULL, FALSE);
	//ɾ��mailslot
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
		g_pAlarm->PostAlarmMessage(TTSMod_Error_TTSThread,AL_Fatal,"TTS�������߳�ʧ��!", g_pTTS->GetLog());
	}
	g_pAlarm->Stop();
	g_pAlarm->Start(g_Config->GetSystemIP(),g_Config->GetSystemEntityID(),
		g_Config->GetAlarmCenterIP(),g_Config->GetAlarmCenterPort());

	return true;
}