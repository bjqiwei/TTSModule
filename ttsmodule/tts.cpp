//************************************************
// Module : TT_TTS Class Head File Defined Here.
// Time   : 2005.09.10
// AT Work Group(R) CopyRight(C).2005 
//************************************************
#include "tts.h"
#include <log4cplus/loggingmacros.h>
#include <sstream>

TTSModule::TTSModule(const TAlarm &alarm):m_Alarm(alarm){
	this->log = log4cplus::Logger::getInstance("TTSModule");
}

void TTSModule::PutMessage(CTTSMessage * msg)
{
	m_RecMsgQueue.addData(msg);
	static log4cplus::Logger msglog = log4cplus::Logger::getInstance("MsgQueue");
	LOG4CPLUS_INFO(msglog, "CTTSMessage:" << this->m_RecMsgQueue.size());
	if (m_RecMsgQueue.size() > this->GetInstanceNum() * 10)
	{ 
		m_Alarm.PostAlarmMessage(TTSMod_NoLicense,AL_Important,"TTS转换并发数已经达到并发上限",msglog);
	}
}

bool TTSModule::GetResult(CTTSMessage * &msg,unsigned long dwMilliseconeds)
{
	static log4cplus::Logger msglog = log4cplus::Logger::getInstance("MsgQueue");
	bool ret = m_ResultMsgQueue.getData(msg,dwMilliseconeds);
	if(ret) LOG4CPLUS_INFO(msglog, "CTTSResultMessage:" << m_ResultMsgQueue.size());
	return ret;
}

TTSModule::~TTSModule(){
}