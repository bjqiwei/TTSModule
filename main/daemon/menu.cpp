#include "menu.h"
#include "daemon.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <iostream>

typedef  bool (*CommandAction) () ;
/*命令子选项中的数据定义*/
typedef struct 
{
	int short_command;			//短命令
	const char* long_command;	//长命令
	CommandAction action;				//执行动作，由application.h中定义
	const char* comment;		//注释项
} menu_command_option;

//菜单命令，按实际支持的动态加载项修改
static const int max_menu_count = 20;
static menu_command_option menu_command_options[max_menu_count+1] = { {0, 0, 0, 0} };


////////////////////////////////////////////////////////////////////
//命令行


static struct { const char* namel; int has_arg; int* flag; int val; } long_options[] = {
	{ "interactive", 0, 0, 'i' },
	{ "service", 1, 0, 's'},
	{ "command", 1, 0, 'c' },
	{ "version", 0, 0, 'v' },
	{ "no-change-dir", 0, 0, 'n' },
	{ "help", 0, 0, 'h' },
	{ 0, 0, 0, 0 }
};

static void insert_to_menu(int short_command, const char* long_command, CommandAction action, const char* comment)
{
	for (int i=0; i<max_menu_count; i++)
	{
		if (menu_command_options[i].short_command != 0)
			continue;
		menu_command_options[i].short_command = short_command;
		menu_command_options[i].long_command = long_command;
		menu_command_options[i].action = action;
		menu_command_options[i].comment = comment;
		break;
	}
	return;
}

//菜单初始化
void init_menu()
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_DEBUG(log, "init menu.");
	//加载应用菜单
	//RegistAppMenu((void*)insert_to_menu);
	//加载通用菜单
	LOG4CPLUS_DEBUG(log, "register common menu.");
	insert_to_menu('h', "help", NULL, "show this menu");
	insert_to_menu('q', "quit", NULL, "quit program");
	insert_to_menu('v', "app version", NULL, "show app version");
	insert_to_menu('l', "loadconfig", reload_config, "reload application configure");
}

void usage(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	printf("Your Input: \n  >");
	for (int i=0; i<__argc; i++) printf(" %s", __argv[i]);
	printf("\n");
	vprintf(fmt, ap);
	printf("Type '%s --help' for usage.\n\n", APPLICATION_NAME);
	va_end(ap);
	exit(1);
}

void print_version()
{
	static const char __version_info[] = ""
		""APPLICATION_NAME" ( "APPLICATION_FULLNAME" @ "APPLICATION_PRODUCTNAME" ) "APPLICATION_VERSION"\n"
		"Build at "APPLICATION_BUILD_DATE" "APPLICATION_BUILD_TIME"\n"
		""APPLICATION_COPYRIGHT" Beijing Hisunsray Information Technology Co., Ltd.\n"
		"All Rights Reserved.\n"
		"Compiled by "APPLICATION_COMPILER"\n"
		""APPLICATION_DESCRIPTION"\n"
		;
	printf(__version_info);
}

//显示命令行
void show_options()
{
	menu_command_option* p = NULL;

	printf("\n");
	printf("Hint: %s [-hv] [--no-change-dir] [--option=option-string] \n\n", APPLICATION_NAME);
	printf("General Options:\n");

	//--intetactive
	printf("  -i, --intetactive:       Run as an interactive console application [default]\n");
	//printf("\n");

	//--command
	printf("  -c, --command=");
	for (p = menu_command_options; p->short_command != 0; p++)
		if (p->action != NULL) printf("%s|", p->long_command);
	printf("\b \n");
	printf("                           Send command to service\n");
	for (p = menu_command_options; p->short_command != 0; p++)
		if (p->action != NULL) printf("      \"%s\":         \t%s\n", p->long_command, p->comment);
	//printf("\n");

	//--service
	printf("  -s, --service=status|start|stop|restart\n");
	printf("                           Run as daemon(service)\n");
	printf("      \"status\":                 show running status of service\n");
	printf("      \"start\":                  start service\n");
	printf("      \"stop\":                   stop service\n");
	printf("      \"restart\":                restart service\n");
	//printf("\n");
	printf("  --no-change-dir:         Not change current dir\n");
	printf("  -h [--help]:             Show this help\n");
	printf("  -v [--version]:          Display version information\n");
	printf("\n");

	//shotcut
	printf("ShotCut:\n");
	printf("     start:           \tsame as \"--service=start\"\n");
	printf("     stop:            \tsame as \"--service=stop\"\n");
	printf("     status:          \tsame as \"--service=status\"\n");
	printf("     restart:         \tsame as \"--service=restart\"\n");
	for (p = menu_command_options; p->short_command != 0; p++)
		if (p->action != NULL) printf("     %s:       \tsame as \"--command=%s\"\n", p->long_command, p->long_command);
	printf("\n");

	printf("Exsample:\n");
	printf("  > %s --service=start\n", APPLICATION_NAME);
	printf("  > %s start --no-change-dir\n", APPLICATION_NAME);
	printf("  > %s stop\n", APPLICATION_NAME);
	printf("  > %s --version\n", APPLICATION_NAME);
	printf("\n");

	//退出
	exit(0);
}

//显示帮助菜单
void show_commands()
{
	printf( "  ==========================================\n");
	printf( "   run in intetactive mode:\n");
	printf( "   valid command:\n");

	for (menu_command_option* p = menu_command_options; p->short_command != 0; p++)
	{
		if (isalnum(p->short_command))
			printf(" %18s (%c):    %s\n", p->long_command, p->short_command, p->comment);
		else
			printf(" %18s:    %s\n", p->long_command, p->comment);
	}

	printf( "  ==========================================\n");
	fflush( stdout );
}

//根据command.name来获取短名称，如 snapshot==>'s'
int get_short_command(const char* long_command)
{
	if (long_command == NULL)
		return 0;
	for (menu_command_option* p = menu_command_options; p->short_command != 0; p++)
		if (strcmp(p->long_command, long_command) == 0)
			return p->short_command;
	return 0;
}

//获取comman的长名称， 如 's'==>"snapshot"
const char* get_long_command(int short_command)
{
	for (menu_command_option* p = menu_command_options; p->short_command != 0; p++)
		if (p->short_command == short_command)
			return p->long_command;
	return "unknow cmd";
}

//运行命令
bool run_menu_command(int short_command)
{
	bool result = false;
	for (menu_command_option* p = menu_command_options; p->short_command != 0; p++)
		if (p->action && p->short_command == short_command)
			result = p->action();
	std::cout << "ok!!!" << std::endl;
	return result;
}

//命令菜单的注册
bool register_menu_command(void* regist)
{
	for (menu_command_option* p = menu_command_options; p->short_command != 0; p++)
		if (p->action != NULL)
			((void(*)(const char*, CommandAction))regist)(p->long_command, p->action);
	return true;
}

//////////////////////////////////////////////////////////
//有效性判断
static bool is_valid_service_optarg(const char* arg)
{
	if (arg == NULL)
		return false;
	if (strcmp(arg, "status") == 0 ||
#ifdef WIN32
		strcmp(arg, "daemon") == 0 ||
		strcmp(arg, "noconsole") == 0 ||
#endif
		strcmp(arg, "start") == 0 ||
		strcmp(arg, "stop") == 0 ||
		strcmp(arg, "restart") == 0)
		return true;
	return false;		
}

static bool is_valid_command_optarg(const char* arg)
{
	if (arg == NULL)
		return false;
	if (get_short_command(arg) == 0)
		return false;
	return true;
}

static bool is_valid_service_shortcut_optarg(const char* arg)
{
	return is_valid_service_optarg(arg);
}

static bool is_valid_command_shortcut_optarg(const char* arg)
{
	if (arg == NULL)
		return false;
	for (menu_command_option* p = menu_command_options; p->short_command != 0; p++)
		if (strcmp(p->long_command, arg) == 0 && p->action)
			return true;
	return false;
}

//获取短options，没有匹配到返回-1
static int match_long_options(const char* argv)
{
	unsigned int i=0;
	char optarg[128] = "";

	strncpy(optarg, argv, 64);
	if (strchr(optarg, '=') != NULL)
		*strchr(optarg, '=') = 0;

	for (i = 0; long_options[i].namel != NULL; i++)
		if (strcmp(long_options[i].namel, optarg) == 0)
			return long_options[i].val;
	
	return -1;
}

//默认采用交互性控制台方式运行，改变默认的工作当前路径到可执行程序目录下
void parse_options(int argc, char **argv, input_command_option* options)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance(APPLICATION_NAME);
	LOG4CPLUS_DEBUG(log, "parse command line parameter.");
	options->run_mode = RUNMODE_DEFAULT;
	options->nochdir = false;
	
	int mode_opt_count = 0;

	for (int i=1; i<argc; i++) 
	{
		if (argv[i][0] != '-')
		{
			if (is_valid_service_shortcut_optarg(argv[i]))
			{
				mode_opt_count++;
				options->run_mode = RUNMODE_SERVICE;
				options->parms = argv[i];
			}
			else if (is_valid_command_shortcut_optarg(argv[i]))
			{
				mode_opt_count++;
				options->run_mode = RUNMODE_COMMAND;
				options->parms = argv[i];
			}
			else
			{
				LOG4CPLUS_ERROR(log,"Error: Unkown Shotcut("<< argv[i] <<")");
				usage("Error: Unkown Shotcut(%s)\n", argv[i]);
				exit(1);
			}
			continue;
		}
		
		int opt = argv[i][1];
		const char* optarg = argv[i]+2;
		
		if (opt == '-') //long.options
		{
			if (*optarg == 0){
				LOG4CPLUS_ERROR(log,"Error: invalid command-line");
				usage("Error: invalid command-line\n");
			}

			opt = match_long_options(optarg);
			optarg = strchr(optarg, '=');
			if (optarg != NULL)
				optarg++;
			else
				optarg = "";
		}
		
		//short.options
		switch (opt) 
		{
		case 'i':
			mode_opt_count++;
			options->run_mode = RUNMODE_INTETACTIVE;
			options->parms = "";
			break;
		case 'c':
			mode_opt_count++;
			if (*optarg == 0) optarg = argv[++i];
			if (!is_valid_command_optarg(optarg)){
				LOG4CPLUS_ERROR(log,"Error: Unkown command-ARG("<< optarg<<")");
				usage("Error: Unkown command-ARG(%s)\n", optarg);
			}
			options->run_mode = RUNMODE_COMMAND;
			options->parms = optarg;
			break;
		case 's':
			mode_opt_count++;
			if (*optarg == 0) optarg = argv[++i];
			if (!is_valid_service_optarg(optarg)){
				LOG4CPLUS_ERROR(log,"Error: Unkown service-ARG("<< optarg <<")");
				usage("Error: Unkown service-ARG(%s)\n", optarg);
			}
			options->run_mode = RUNMODE_SERVICE;
			options->parms = optarg;
			break;
		case 'n':
			options->nochdir = true;
			break;
		case 'v':
			print_version();
			exit(0);
			break;
		case 'h':
		case '?':
			show_options();
			break;
		default:
			{
				LOG4CPLUS_ERROR(log,"Error: Unkown OPTION("<<argv[i]<<")");
				usage("Error: Unkown OPTION(%s)\n", argv[i]);
				break;
			}
		}
	} //end for

	if (mode_opt_count > 1){
		usage("You must specify exactly one of -i, -c, -s.\n"
			  "     or shotcut of these options\n");
	}
	
	return;
}
