#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "../tinyxml/tinyxml2.h"
#include <log4cplus/logger.h>
#include <vector>


typedef tinyxml2::XMLError ConfigError;
class Config :
	private tinyxml2::XMLDocument
{
public:
	Config();
	virtual ~Config();

    ConfigError LoadFile( const char* filename );
    ConfigError LoadFile( FILE* );

	void Clear();
	const std::string & GetSystemIP();
	int GetSystemPort();
	unsigned long GetTTSLicenseNumber();
	const std::string & GetSystemFileServerPath();
	const std::string & GetTTSSerialNumber();
	const int GetTTSCode();
	bool GetEnableHttpFileServer();
	const std::string & GetHttpFileServerIP();
	int GetHttpFileServerPort();
	const std::string & GetHttpFileServerDocumentRoot();
	const std::vector<std::string> & GetAllowIp();
	const std::string & GetSystemEntityID();
	const std::string & GetAlarmCenterIP();
	const int GetAlarmCenterPort();
	const log4cplus::Logger & GetLog(){ return log;};
private:
	log4cplus::Logger log;
	std::string m_SystemIP;
	int m_SystemPort;
	unsigned long m_TTSLicenseNumber;
	std::string m_TTSServerFileServerPath;
	std::string m_TTSSerialNumber;
	std::string m_HttpFileServerIP;
	int m_HttpFileServerPort;
	std::string m_HttpFileServerDocumentRoot;
	std::vector<std::string> m_AllowIp;
	std::string m_EntityID;
	std::string m_AlarmCenterIP;
	int m_AlarmCenterPort;
	int m_nTTSCode;
	bool m_EnableHttpFileServer;
};
#endif