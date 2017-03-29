#ifndef __MENU_H__
#define __MENU_H__

/*命令行扫描内容保存项*/
enum { RUNMODE_DEFAULT, RUNMODE_INTETACTIVE, RUNMODE_COMMAND, RUNMODE_SERVICE, };
typedef struct { int run_mode; const char* parms; bool nochdir; } input_command_option;

/*帮助及菜单*/
void show_options();
void show_commands();
void print_version();

/*初始化菜单项*/
void init_menu();

//运行短命令，如"s"
bool run_menu_command(int short_command);

//命令菜单的注册, win32-gui中调用
bool register_menu_command(void* regist);

//根据command.name来获取短名称，如 snapshot==>'s'
int get_short_command(const char* long_command);
//获取comman的长名称， 如 's'==>"snapshot"
const char* get_long_command(int short_command);

/*命令行扫描*/
void parse_options(int argc, char **argv, input_command_option* options);

#endif
