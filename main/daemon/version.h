#ifndef __VERSION_H__
#define __VERSION_H__

//�汾��Ϣ�����ļ�

//////////////////////////////////////////////////////
//������Ϣ���ڵ�һ����Ӧ�ó���ʱ�޸�
#define APPLICATION_PRODUCTNAME "tts"					  //��Ʒ����
#define APPLICATION_NAME 		"tts"					  //ģ������
#define APPLICATION_FULLNAME 	"tts"			  //ģ��ȫ��
#define APPLICATION_DESCRIPTION	"tts for yuntongxun"  //������Ϣ
#define APPLICATION_COPYRIGHT   "CopyRight (C) 2012-2015" //ģ��İ�Ȩ���

//������Ϣ���ÿ�η����޸�
#define APPLICATION_WINVER      1,1,3,18			//win����Դ�ļ��İ汾��Ϣ
#define APPLICATION_VERSION     "1.1.3.18"		//ģ��汾��(�ַ�����ʽ)
#define APPLICATION_COMMENTS	""				//ע����Ϣ��


/////////////////////////////////////////////////////
//������Ϣ������޸�

//�������ڣ������޸�
#define APPLICATION_BUILD_DATE __DATE__			//ģ��ı�������
#define APPLICATION_BUILD_TIME __TIME__			//ģ��ı���ʱ��

//��������Ϣ�������޸�
#ifdef __GNUC__
#	define APPLICATION_COMPILER   "GCC "__VERSION__

#elif defined(_MSC_VER)
#	if (_MSC_VER >= 1600)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 10.0"
#	elif (_MSC_VER >= 1500)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 9.0"
#	elif (_MSC_VER >= 1400)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 8.0"
#	elif (_MSC_VER >= 1310)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 7.1"
#	elif (_MSC_VER >= 1300)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 7.0"
#	elif (_MSC_VER >= 1200)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 6.0"
#	elif (_MSC_VER >= 1100)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 5.0"
#	elif (_MSC_VER >= 1000)
#		define APPLICATION_COMPILER "Microsoft Visuall C++ 4.0"
#	else
#		define APPLICATION_COMPILER "Microsoft Visuall C++"
#	endif

#else
#	define APPLICATION_COMPILER "Unkowned C/CPP Compiler"

#endif //__GNUC__

#endif
