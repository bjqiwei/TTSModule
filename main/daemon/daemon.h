#ifndef __DAEMON_H__
#define __DAEMON_H__
#include <log4cplus/thread/threads.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <sstream>
//ȫ�ֳ�ʼ�����˳�
void global_init();
void global_exit();

//�޸ĵ�ǰ��·��
void change_current_dir();

//���̵ĺ�̨������(����Ϊservice�Ĳ���ֵ����win32��Ч)
bool daemonize(const char* parms);

//��ѯ�����Ƿ��Ѵ���
bool process_is_exist();

//���Ѵ��ڽ��̷��������������'snapshot��'
bool process_send_command(const char* parms);

//ֹͣ�Ѵ��ڵĽ���
bool process_stop();

//main��ͨ�����
bool main_running(int mode);

//���¼�������
bool reload_config();
//win32�µ����
#ifdef WIN32
bool win32_WinMain();
#endif

//�����в���
#if defined(_AIX)||defined(_LINUX)
extern int __argc;          /* count of cmd line args */
extern char ** __argv;      /* pointer to table of cmd line args */
#endif

#endif
