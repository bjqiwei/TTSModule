#ifndef __VERSION_H__
#define __VERSION_H__

//版本信息描述文件

//////////////////////////////////////////////////////
//以下信息，在第一建立应用程序时修改
#define APPLICATION_PRODUCTNAME "tts"					  //产品名称
#define APPLICATION_NAME 		"tts"					  //模块名称
#define APPLICATION_FULLNAME 	"tts"			  //模块全称
#define APPLICATION_DESCRIPTION	"tts for yuntongxun"  //描述信息
#define APPLICATION_COPYRIGHT   "CopyRight (C) 2012-2015" //模块的版权年份

//以下信息项，随每次发布修改
#define APPLICATION_WINVER      1,1,3,18			//win的资源文件的版本信息
#define APPLICATION_VERSION     "1.1.3.18"		//模块版本号(字符串格式)
#define APPLICATION_COMMENTS	""				//注释信息段


/////////////////////////////////////////////////////
//以下信息项，请勿修改

//编译日期，请勿修改
#define APPLICATION_BUILD_DATE __DATE__			//模块的编译日期
#define APPLICATION_BUILD_TIME __TIME__			//模块的编译时间

//编译器信息，请勿修改
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
