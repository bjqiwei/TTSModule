#include "version.h"

//版本信息导出函数

#ifdef __cplusplus
	extern "C"
#endif

#ifdef WIN32
__declspec(dllexport)
#else
__attribute__ ((__section__(".yuntongxun.version")))
__attribute__ ((visibility("default")))
#endif

const char* get_version_info()
{
	static const char __version_info[] = ""
		""APPLICATION_NAME" ( "APPLICATION_FULLNAME" @ "APPLICATION_PRODUCTNAME" ) "APPLICATION_VERSION"\n"
		"Build at "APPLICATION_BUILD_DATE" "APPLICATION_BUILD_TIME"\n"
		""APPLICATION_COPYRIGHT" Yuntongxun Information Technology Co., Ltd.\n"
		"All Rights Reserved.\n"
		"Compiled by "APPLICATION_COMPILER"\n"
		""APPLICATION_DESCRIPTION"\n"
		;
	return __version_info;
}
