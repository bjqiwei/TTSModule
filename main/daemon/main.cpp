
#include "menu.h"
#include "daemon.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <ctype.h>
#include <assert.h>
#include <log4cplus/logger.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>



//�����в���
#if defined(_AIX)||defined(_LINUX)
	int __argc = 0;          /* count of cmd line args */
	char ** __argv = 0;      /* pointer to table of cmd line args */
#endif

//����ʽ����
static void run_as_daemon(const char* parms)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_DEBUG(log,"run a daemon.");
	if (strcmp(parms, "status") == 0)
		return;

	if (strcmp(parms, "stop") == 0)
	{
		LOG4CPLUS_DEBUG(log,"stop command.");
		process_stop();
		return;
	}

	if (strcmp(parms, "restart") == 0)
	{
		LOG4CPLUS_WARN(log,"restart command.");
		process_stop();
	}

	if (process_is_exist())
	{
		LOG4CPLUS_WARN(log,APPLICATION_NAME<<"already running....");
		printf("%s already running...\n", APPLICATION_NAME);
		return;
	}

	//start || restart || noconsole || daemon
	printf("%s starting.....\t\t\t[on]\n", APPLICATION_NAME);
	printf("%s run as daemon\n", APPLICATION_NAME);
	printf("if you want to review run result\n");
	printf("you can try:  \'%s --service=status\'\n", APPLICATION_NAME);
#ifdef _LINUX
	printf("        or: \'tail /var/log/messages\'\n"); 
#endif
	
	//��̨������
	daemonize(parms);

	main_running(RUNMODE_SERVICE);
	return;
}

//����̨��ʽ����
static void run_as_console()
{
	//��������ѭ������
	main_running(RUNMODE_INTETACTIVE);
}

//Ĭ�Ϸ�ʽ����
static void run_as_default()
{
#ifdef WIN32
	main_running(RUNMODE_INTETACTIVE);
#else
	run_as_console();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//������
int main(int argc, char** argv, char** env)
{	
	log4cplus::initialize();
	//log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);
	log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("../etc/log4cplus.properties"));
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_DEBUG(log, "starting...");
	
#if defined(_AIX)||defined(_LINUX)
	__argc = argc;
	__argv = argv;
#endif

	//ȫ�ֳ�ʼ��
	global_init();

	//�˵���ʼ��
	init_menu();

	//ɨ��������
	input_command_option input_opts;
	parse_options(argc, argv, &input_opts);
	
	//���Ĺ���·��
	if (!input_opts.nochdir)
		change_current_dir();
	
	//��ӡ��ǰ״̬
	bool proExist = process_is_exist();
	LOG4CPLUS_INFO(log, APPLICATION_NAME" status: \t\t\t\t[" << (proExist?"on":"off") << "]");
	std::cout <<  APPLICATION_NAME" status: \t\t\t\t[" << (proExist?"on":"off") << "]" << std::endl;
	//Ĭ��ʹ��intetactive��ʽ
	switch (input_opts.run_mode)
	{
	case RUNMODE_COMMAND:
		process_send_command(input_opts.parms);
		break;
	case RUNMODE_SERVICE:	//status��start��stop��restart��daemon(win32)��noconsole(win32)
		run_as_daemon(input_opts.parms);
		break;
	case RUNMODE_INTETACTIVE:
		if (process_is_exist()){
			LOG4CPLUS_WARN(log,APPLICATION_NAME << " already running...");
			printf("%s already running...\n", APPLICATION_NAME);
		}
		else
			run_as_console();
		break;
	case RUNMODE_DEFAULT:
		if (process_is_exist()){
			LOG4CPLUS_WARN(log,APPLICATION_NAME << " already running...");
			printf("%s already running...\n", APPLICATION_NAME);
		}
		else
			run_as_default();
		break;
	default:
		assert(0);
		show_options();
		LOG4CPLUS_WARN(log,"invalid run mode("<<input_opts.run_mode << "), exit" );
		printf("invalid run mode(%d), exit\n\n", input_opts.run_mode);
		exit(-1);
		break;
	}
	
	//�����˳�
	LOG4CPLUS_INFO(log, "main function exit.");
	return 0;
}
