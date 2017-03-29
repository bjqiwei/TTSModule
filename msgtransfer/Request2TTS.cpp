#include "Request2TTS.h"
#include <sstream>
#include <log4cplus/loggingmacros.h>
#include "http.h"
#include "tinyxml2.h"
#include "../common/stringHelper.h"

//Request2TTS class
Request2TTS::Request2TTS(HttpServer &server, Config &config, TTSModule *tts):m_bRun(false)
	,m_HttpServer(server),m_Config(config),m_pTTS(tts)
{
	this->log = log4cplus::Logger::getInstance("Request2TTS");
	LOG4CPLUS_TRACE(log, "Construction");
};

Request2TTS::~Request2TTS(){
	LOG4CPLUS_TRACE(log, "Destruction");
};

bool Request2TTS::Start(){
	
	if (!isRunning())
	{
		m_bRun = true;
		start();
		LOG4CPLUS_INFO(log, "Started");
	}
	else{
		LOG4CPLUS_WARN(log, " Already running.");
	}
	return true;
}

void Request2TTS::Stop(){
	if (!isRunning()){
		LOG4CPLUS_WARN(log, " Already stoped.");
		return;
	}
	m_bRun = false;
	LOG4CPLUS_TRACE(log, " waiting stop...");
	join();
	LOG4CPLUS_INFO(log, "Stoped");
}

void Request2TTS::ResponseError(THttpRequest * &request, ErrorCode err)
{
	LOG4CPLUS_TRACE(log,__FUNCTION__ << " start.");
	THttpResponse response(this->log);
	response.SetContentData(CTTSMessage::MakeHttpResponseWithErrorCode(err));
	if(request->Response(response) != 0){
		LOG4CPLUS_ERROR(log,"Response Error.");
	}
	LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
}

bool Request2TTS::ParseContext2Msg(THttpRequest * &request, CTTSMessage * &msg)
{
	LOG4CPLUS_TRACE(log,__FUNCTION__ << " start.");

	if(tinyxml2::XML_SUCCESS != xmlDoc.Parse(request->GetContentData().c_str())){
		LOG4CPLUS_ERROR(log, "HttpBody Invalid XML:" << request->GetContentData());
		ResponseError(request,Http_BodyInvalidXML);
		LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
		return false;
	}

	msg->m_TxtEncode = request->GetEncode();
	helper::string::toUpper(msg->m_TxtEncode);

	tinyxml2::XMLElement * em = xmlDoc.FirstChildElement("Request");
	if (em == NULL)
	{
		LOG4CPLUS_ERROR(log," HttpBody not find Request Element:" << request->GetContentData());
		ResponseError(request,Http_MissingRequestElement);
		LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
		return false;
	}

	//serviceid
	tinyxml2::XMLElement * subEm = em->FirstChildElement("serviceid");
	if (subEm)
		msg->m_Serviceid = subEm->GetText()?subEm->GetText():"";
	else{
		LOG4CPLUS_ERROR(log, "HttpBody not find serviceid Element:" << request->GetContentData());
		ResponseError(request,Http_MissingServiceidElement);
		LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
		return false;
	}

	//sessionid
	subEm = em->FirstChildElement("sessionid");
	if (subEm)
		msg->m_Sessionid = subEm->GetText()?subEm->GetText():"";
	else{
		LOG4CPLUS_ERROR(log, "HttpBody not find sessionid Element:" << request->GetContentData());
		ResponseError(request, Http_MissingSessionidElement);
		LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
		return false;
	}

	//txt
	subEm = em->FirstChildElement("txt");
	if (subEm)
		msg->m_TransferTxt = subEm->GetText()?subEm->GetText():"";
	else{
		LOG4CPLUS_ERROR(log, "HttpBody not find txt Element:" << request->GetContentData());
		ResponseError(request, Http_MissingTxtElement);
		LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
		return false;
	}

	//vid
	subEm = em->FirstChildElement("vid");
	if (subEm)
		msg->m_nVID = subEm->GetText()?atoi(subEm->GetText()):0;

	//speed
	subEm = em->FirstChildElement("speed");
	if (subEm)
		msg->m_nSpeed = subEm->GetText()?atoi(subEm->GetText()):0;

	//volume
	subEm = em->FirstChildElement("volume");
	if (subEm)
		msg->m_nVolume = subEm->GetText()?atoi(subEm->GetText()):0;

	//pitch
	subEm = em->FirstChildElement("pitch");
	if (subEm)
		msg->m_nPitch = subEm->GetText()?atoi(subEm->GetText()):0;

	//bgsound
	subEm = em->FirstChildElement("bgsound");
	if (subEm)
		msg->m_nBGSound = subEm->GetText()?atoi(subEm->GetText()):0;

	//audioformat
	subEm = em->FirstChildElement("audiofmt");
	if (subEm)
		msg->m_nAudioFmt = subEm->GetText()?atoi(subEm->GetText()):0;

	//resptype
	subEm = em->FirstChildElement("resptype");
	if (subEm)
		msg->m_Resptype = subEm->GetText()?atoi(subEm->GetText()):0;

	if (msg->m_Resptype == 0){
	}
	else if (msg->m_Resptype == 1)
	{
		//respurl
		subEm = em->FirstChildElement("respurl");
		if (subEm)
			msg->m_Respurl = subEm->GetText()?subEm->GetText():"";

		//respip
		subEm = em->FirstChildElement("respip");
		if (subEm)
			msg->m_Respip = subEm->GetText()?subEm->GetText():"";
		else{
			LOG4CPLUS_ERROR(log, "HttpBody not find respip Element");
			ResponseError(request, Http_MissingResponseIPElement);
			LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
			return false;
		}
		//respport
		subEm = em->FirstChildElement("respport");
		if (subEm)
			msg->m_Respport = subEm->GetText()?atoi(subEm->GetText()):0;

	}
	else{
		LOG4CPLUS_ERROR(log, "HttpBody invalid response type:" << msg->m_Resptype);
		ResponseError(request, Http_InvalidResponseType);
		LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
		return false;
	}
	
	LOG4CPLUS_TRACE(log,__FUNCTION__ << " end.");
	return true;
}
void Request2TTS::run()
{
	LOG4CPLUS_TRACE(log, "run start");
	while(this->m_bRun)
	{
		THttpRequest * request = NULL;
		m_HttpServer.GetHttpRequest(request,1000);
		if (request == NULL){
			static unsigned int i = 0x0;
			if ((i++ % 60) == 0){
				LOG4CPLUS_TRACE(log,"waiting..... request.");
			}
			continue;
		}
		
		LOG4CPLUS_TRACE(log, "tranfer a request:" << request << " to TTSModule.");
		if (m_pTTS->GetInstanceNum() < 1)
		{
			LOG4CPLUS_ERROR(log, "No License.");
			ResponseError(request, TTSMod_NoLicense);
			delete request;
			continue;
		}
		CTTSMessage * msg = new CTTSMessage();
		if(!ParseContext2Msg(request,msg))
		{
			delete request;
			delete msg;
			continue;
		}
		if (msg->m_Resptype == 1)
		{
			THttpResponse response(this->log);
			if(request->Response(response) != 0){
				LOG4CPLUS_ERROR(log,"Response Error.");
			}
			delete request;
		}else if (msg->m_Resptype == 0){
			msg->m_Request = request;
		}
		
		msg->m_TtsFile  = m_Config.GetSystemFileServerPath();

		m_pTTS->PutMessage(msg);
		
	}
	LOG4CPLUS_TRACE(log, "run end");
}

