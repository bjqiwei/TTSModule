#ifndef __REQUEST2TTS_H__
#define __REQUEST2TTS_H__
#include <log4cplus/thread/threads.h>
#include <log4cplus/logger.h>
#include "httpserver.h"
#include "config.h"
#include "tts.h"
#include "msgdefine.h"

class Request2TTS : public log4cplus::thread::AbstractThread
{
public:
	explicit Request2TTS(HttpServer & server,Config & config, TTSModule * tts);
	virtual ~Request2TTS();
	bool Start();
	void Stop();
protected:
	virtual void run();
	void ResponseError(THttpRequest * & request, ErrorCode err);
	bool ParseContext2Msg(THttpRequest * &request, CTTSMessage * &msg);
protected:
	log4cplus::Logger log;
	volatile bool m_bRun;
	HttpServer &m_HttpServer;
	Config &m_Config;
	TTSModule * m_pTTS;
	tinyxml2::XMLDocument xmlDoc ;
};


#endif
