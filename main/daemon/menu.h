#ifndef __MENU_H__
#define __MENU_H__

/*������ɨ�����ݱ�����*/
enum { RUNMODE_DEFAULT, RUNMODE_INTETACTIVE, RUNMODE_COMMAND, RUNMODE_SERVICE, };
typedef struct { int run_mode; const char* parms; bool nochdir; } input_command_option;

/*�������˵�*/
void show_options();
void show_commands();
void print_version();

/*��ʼ���˵���*/
void init_menu();

//���ж������"s"
bool run_menu_command(int short_command);

//����˵���ע��, win32-gui�е���
bool register_menu_command(void* regist);

//����command.name����ȡ�����ƣ��� snapshot==>'s'
int get_short_command(const char* long_command);
//��ȡcomman�ĳ����ƣ� �� 's'==>"snapshot"
const char* get_long_command(int short_command);

/*������ɨ��*/
void parse_options(int argc, char **argv, input_command_option* options);

#endif
