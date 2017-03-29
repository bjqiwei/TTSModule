#include "config.h"
#include <log4cplus/loggingmacros.h>
#include <sstream>

Config::Config():m_SystemPort(-1),m_TTSLicenseNumber(0),m_HttpFileServerPort(-1)
	,m_AlarmCenterPort(-1),m_nTTSCode(-1),m_EnableHttpFileServer(true)
{
	log = log4cplus::Logger::getInstance("Config");
	LOG4CPLUS_TRACE(log, "Construction");
}
Config::~Config()
{
	LOG4CPLUS_TRACE(log, "Destruction");
}

ConfigError Config::LoadFile( const char* filename )
{
	LOG4CPLUS_INFO(log, "LoadFile:" << filename);
	return XMLDocument::LoadFile(filename);
}
ConfigError Config::LoadFile( FILE* fp)
{
	return XMLDocument::LoadFile(fp);
}

void Config::Clear()
{
	m_SystemIP.clear();
	m_SystemPort = -1;
	m_TTSLicenseNumber = 0;
	m_TTSServerFileServerPath.clear();
	m_TTSSerialNumber.clear();
	m_HttpFileServerIP.clear();
	m_HttpFileServerPort = -1;
	m_HttpFileServerDocumentRoot.clear();
	m_AllowIp.clear();
	m_EntityID.clear();
	m_AlarmCenterIP.clear();
	m_AlarmCenterPort = -1;
	m_nTTSCode = -1;
	m_EnableHttpFileServer = true;
}

const std::string & Config::GetSystemIP()
{
	if (m_SystemIP.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("System");
		if (em != NULL)
		{
			tinyxml2::XMLElement * ipEm = em->FirstChildElement("ip");
			if (ipEm != NULL && ipEm->GetText())
			{
				m_SystemIP = ipEm->GetText();
			}

		}
	}
	return m_SystemIP;
}

int Config::GetSystemPort()
{
	if (m_SystemPort == -1)
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("System");
		if (em != NULL)
		{
			tinyxml2::XMLElement * portEm = em->FirstChildElement("port");
			if (portEm != NULL && portEm->GetText())
			{
				m_SystemPort = atoi(portEm->GetText());
			}

		}
	}
	return m_SystemPort;
}

unsigned long Config::GetTTSLicenseNumber()
{
	if (m_TTSLicenseNumber == 0)
	{
		tinyxml2::XMLElement * tts = XMLDocument::FirstChildElement("TTS");
		if (tts != NULL)
		{
			tinyxml2::XMLElement * li = tts->FirstChildElement("LicenseNumber");
			if (li != NULL && li->GetText())
			{
				m_TTSLicenseNumber = atoi(li->GetText());
			}

		}
	}
	return m_TTSLicenseNumber;
}

const std::string & Config::GetSystemFileServerPath()
{
	if (m_TTSServerFileServerPath.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("System");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("FileServerPath");
			if (subEm != NULL && subEm->GetText())
			{
				m_TTSServerFileServerPath = subEm->GetText();
			}
		}
	}
	return m_TTSServerFileServerPath;
}

const std::string & Config::GetTTSSerialNumber()
{
	if (m_TTSSerialNumber.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("TTS");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("SerialNumber");
			if (subEm != NULL && subEm->GetText())
			{
				m_TTSSerialNumber = subEm->GetText();
			}
		}
	}
	return m_TTSSerialNumber;
}

bool Config::GetEnableHttpFileServer()
{
	if (m_EnableHttpFileServer)
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("HttpFileServer");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("enable");
			if (subEm != NULL && subEm->GetText())
			{
				m_EnableHttpFileServer = atoi(subEm->GetText());
			}
		}
	}
	return m_EnableHttpFileServer;
}

const std::string & Config::GetHttpFileServerIP()
{
	if (m_HttpFileServerIP.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("HttpFileServer");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("ip");
			if (subEm != NULL && subEm->GetText())
			{
				m_HttpFileServerIP = subEm->GetText();
			}
		}
	}
	return m_HttpFileServerIP;
}

int Config::GetHttpFileServerPort()
{
	if (m_HttpFileServerPort == -1)
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("HttpFileServer");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("port");
			if (subEm != NULL && subEm->GetText())
			{
				m_HttpFileServerPort = atoi(subEm->GetText());
			}
		}
	}
	return m_HttpFileServerPort;
}

const std::string & Config::GetHttpFileServerDocumentRoot()
{
	if (m_HttpFileServerDocumentRoot.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("HttpFileServer");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("documentroot");
			if (subEm != NULL && subEm->GetText())
			{
				m_HttpFileServerDocumentRoot = subEm->GetText();
			}
		}
	}
	return m_HttpFileServerDocumentRoot;
}

const std::vector<std::string> & Config::GetAllowIp()
{
	if (m_AllowIp.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("AllowIp");
		if (em != NULL)
		{
			for(tinyxml2::XMLElement * subEm = em->FirstChildElement("ip"); subEm != NULL; subEm = subEm->NextSiblingElement("ip"))
			{
				if(subEm->GetText())m_AllowIp.push_back(subEm->GetText());
			}
		}
	}
	return m_AllowIp;
}

const std::string & Config::GetSystemEntityID()
{
	if (m_EntityID.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("System");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("EntryID");
			if (subEm != NULL && subEm->GetText())
			{
				m_EntityID = subEm->GetText();
			}
		}
	}
	return m_EntityID;
}

const std::string & Config::GetAlarmCenterIP()
{
	if (m_AlarmCenterIP.empty())
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("AlarmCenter");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("ip");
			if (subEm != NULL && subEm->GetText())
			{
				m_AlarmCenterIP = subEm->GetText();
			}
		}
	}
	return m_AlarmCenterIP;
}

const int Config::GetAlarmCenterPort()
{
	if (m_AlarmCenterPort == -1)
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("AlarmCenter");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("port");
			if (subEm != NULL && subEm->GetText())
			{
				m_AlarmCenterPort = atoi(subEm->GetText());
			}
		}
	}
	return m_AlarmCenterPort;
}

const int Config::GetTTSCode()
{
	if (m_nTTSCode == -1)
	{
		tinyxml2::XMLElement * em = XMLDocument::FirstChildElement("TTS");
		if (em != NULL)
		{
			tinyxml2::XMLElement * subEm = em->FirstChildElement("Code");
			if (subEm != NULL && subEm->GetText())
			{
				m_nTTSCode = atoi(subEm->GetText());
			}
		}
		if (m_nTTSCode < 0 || m_nTTSCode >6){
			m_nTTSCode = 0;
		}
	}
	return m_nTTSCode;
}