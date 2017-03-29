//************************************************
// Module : TTS Class Head File Defined Here.
// Time   : 2005.09.10
// AT Work Group(R) CopyRight(C).2005
//************************************************
#ifndef __TTS_H__
#define __TTS_H__
#include <string>
#include <log4cplus/logger.h>
#include "CEventBuffer.h"
#include "msgdefine.h"
#include "alarm.h"

//=====================
#define  BaseErrorCode         170000
#define  MakeStatusCode(id)    (BaseErrorCode+(id))

//======================
									
class TTSModule 
{
public:
	TTSModule(const TAlarm &alarm);
	virtual ~TTSModule() ; 
	virtual bool StartTTSModule( unsigned long licenseNum = 0, const std::string & userData = "",int code=0) = 0;
	virtual void StopTTSModule() = 0;
	virtual void PutMessage(CTTSMessage * msg);
	virtual bool GetResult(CTTSMessage * &msg, unsigned long dwMilliseconeds);
	virtual unsigned int GetInstanceNum() = 0;
	const log4cplus::Logger & GetLog(){ return log;};
protected:
	helper::CEventBuffer<CTTSMessage *> m_RecMsgQueue;
	helper::CEventBuffer<CTTSMessage *> m_ResultMsgQueue;
	const TAlarm &m_Alarm;
	log4cplus::Logger log;
};

#endif
