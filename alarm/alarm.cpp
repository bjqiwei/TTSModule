#include "alarm.h"
#include <log4cplus/loggingmacros.h>
#include <time.h>
#include <strstream>

const std::string TAlarm::m_ModuleName = "TTSModule";
const std::string TAlarm::m_Url = "/alarmmessage";

TAlarm::TAlarm(HttpServer &httpClient)
	:m_HttpClient(httpClient),m_AlarmCenterPort(80)
{
	this->log = log4cplus::Logger::getInstance("Alarm");
	LOG4CPLUS_TRACE(log, "Construction");
}

TAlarm::~TAlarm()
{
	LOG4CPLUS_TRACE(log, "Destruction");
}


void TAlarm::Start(const std::string & systemIP, const std::string & systemEntiryId,
	const std::string & alarmCenterIp, int alarmCenterPort )
{
	m_LocalIP = systemIP;
	m_EntityID = systemEntiryId;
	m_AlarmCenterIP = alarmCenterIp;
	m_AlarmCenterPort = alarmCenterPort;
	LOG4CPLUS_INFO(log,"LocalIP:" << m_LocalIP << " EntityID:" << m_EntityID << " AlarmCenterIP:" << m_AlarmCenterIP << " AlarmCenterPort:" << m_AlarmCenterPort);
	LOG4CPLUS_INFO(log, "Started.");
}

void TAlarm::Stop()
{
	LOG4CPLUS_INFO(log, "Stoped.");
}

void TAlarm::PostAlarmMessage(ErrorCode errId, AlarmLevel lev, const std::string & des, const log4cplus::Logger & log)const
{
	THttpRequest * request = new THttpRequest(log);
	std::stringstream host;
	host << m_AlarmCenterIP << ":" << m_AlarmCenterPort;
	request->SetHost(host.str());
	request->SetMethod(HTTP_METHOD_POST);
	request->SetURI(m_Url);
	request->SetContentData(CreateAlarmMsg(errId,lev, des));
	LOG4CPLUS_DEBUG(log,"Post Alarm content:" << request->GetContentData());
	m_HttpClient.PutHttpSendMsg2Queue(request);
}

std::string TAlarm::CreateAlarmMsg(ErrorCode alarmid,AlarmLevel lev, const std::string &des)const
{
	time_t now = time(NULL);
	std::ostrstream oss;
	oss << MsgType_Alarm << "$" << alarmid << "&" << AlarmMsg_Create << "&" << now << "&"<< m_ModuleName << "&" << m_EntityID
	<< "&" << m_LocalIP << "&" << lev << "$" << des << std::ends;
	std::string result(oss.str());
	delete[] oss.str();
	return result;
}





