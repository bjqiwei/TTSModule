#include "TTS2Request.h"
#include <sstream>
#include <log4cplus/loggingmacros.h>

//TTS2Request class
TTS2Request::TTS2Request(Config & config, TTSModule * tts,HttpServer & httpClient):m_bRun(false)
	,m_Config(config),m_pTTS(tts),m_httpClient(httpClient)
{ 
	this->log = log4cplus::Logger::getInstance("TTS2Request");
	LOG4CPLUS_TRACE(log, "Construction");
};

TTS2Request::~TTS2Request(){
	LOG4CPLUS_TRACE(log, "Destruction");
};

bool TTS2Request::Start(){
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

void TTS2Request::Stop(){
	if (!isRunning()){
		LOG4CPLUS_WARN(log, " Already stoped.");
		return;
	}
	m_bRun = false;
	LOG4CPLUS_TRACE(log, " waiting stop...");
	join();
	LOG4CPLUS_INFO(log, "Stoped");
};

void TTS2Request::run()
{
	LOG4CPLUS_TRACE(log, "run start");
	while(m_bRun)
	{
		CTTSMessage * msg = NULL;
		m_pTTS->GetResult(msg,200);
		if (msg == NULL){
			continue;
		}

		if (msg->m_Resptype == 0)
		{
			THttpRequest * request = reinterpret_cast<THttpRequest*>(msg->m_Request);
			THttpResponse response(this->log);
			response.SetStatusCode(200);

			response.SetContentData(CTTSMessage::MakeHttpResponseWithMessage(msg,m_Config.GetHttpFileServerIP(),
				m_Config.GetHttpFileServerPort(), m_Config.GetSystemFileServerPath()));
			if(request->Response(response)!= 0)
			{
				LOG4CPLUS_ERROR(log,"request:" << request << ", response error.");
			}
			delete request;
		}
		else if (msg->m_Resptype == 1)
		{
			THttpRequest * request = new THttpRequest(this->log);
			request->SetMethod(HTTP_METHOD_POST);
			std::stringstream host;
			host << msg->m_Respip << ":" << msg->m_Respport; 
			request->SetHost(host.str());

			std::string _url = msg->m_Respurl;
			_url.append("?sessionid=");
			_url.append(msg->m_Sessionid);
			_url.append("&file=");
			std::string fileurl = CTTSMessage::MakeDownLoadFileUrl(msg,m_Config.GetHttpFileServerIP(),
				m_Config.GetHttpFileServerPort(), m_Config.GetSystemFileServerPath());

			_url.append(fileurl);
			request->SetURI(_url);
	
			m_httpClient.PutHttpSendMsg2Queue(request);
		}

		delete msg;
		
	}
	LOG4CPLUS_TRACE(log, "run end");
}