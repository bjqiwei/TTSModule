#ifndef __RRS2REQUEST_H__
#define __RRS2REQUEST_H__
#include <log4cplus/thread/threads.h>
#include <log4cplus/logger.h>
#include "httpserver.h"
#include "config.h"
#include "tts.h"

class TTS2Request : public log4cplus::thread::AbstractThread
{
public:
	TTS2Request(Config & config, TTSModule * tts, HttpServer & httpClient);
	virtual ~TTS2Request();
	bool Start();
	void Stop();
protected:
	virtual void run();
protected:
	log4cplus::Logger log;
	volatile bool m_bRun;
	Config &m_Config;
	TTSModule * m_pTTS;
	HttpServer & m_httpClient;
};

#endif
