#ifndef MY_IFLY_TTS_H
#define MY_IFLY_TTS_H

#include "tts.h"
#include <ifly_tts.h>
#include <ttscon_utils.h>
#include <log4cplus/logger.h>
#include <log4cplus/thread/threads.h>
#include <list>

class iFlyTTs;
class iFlyTTsInstance : public log4cplus::thread::AbstractThread
{
public:
	iFlyTTsInstance(log4cplus::Logger parentLog, iFlyTTs & ttsmodule);
	virtual ~iFlyTTsInstance();
	bool Start();
	void Stop();
protected:
	virtual void run();
	bool Connect(const CTTSMessage * param);
	bool SetParam(const CTTSMessage * parma);
	bool DisConnect();
protected:
	log4cplus::Logger log;
	volatile bool m_bRun;
	TTSConnectStruct	ttsConnect;
	TTSData ttsData;
	TTSCallBacks TtsCallBacks;
	iFlyTTs & m_TTSModule;
	HTTSINSTANCE m_instance;
	TTSINT32 m_nMaxInBufSize;
	std::string m_Filename;
	friend class iFlyTTs;
};

class iFlyTTs :
	public TTSModule
{
public:
	iFlyTTs(const TAlarm & alarm);
	virtual ~iFlyTTs(void);
	virtual bool StartTTSModule(unsigned long licenseNum = 0, const std::string &SerialNumber = "",int code=0);
	virtual void StopTTSModule();
	virtual unsigned int GetInstanceNum();
protected:
	bool InitTTSProc();
	void UninitTTSProc();
	bool Initialize();
	bool Uninitialize();
	std::list<iFlyTTsInstance * > m_FlyTTsInstance;
private:
	log4cplus::Logger log;
private:
	static TTSCON_Dll_Handle lib;
	static std::string ttslib_Name;
	std::string m_SerialNumber;
	int m_nTTSCode;
protected:
	static Proc_TTSInitializeEx		m_TTSInitializeEx;
	static Proc_TTSUninitialize		m_TTSUninitialize;
	static Proc_TTSSynthTextEx		m_TTSSynthTextEx;
	static Proc_TTSSynthText		m_TTSSynthText;
	static Proc_TTSFetchNext		m_TTSFetchNext;
	static Proc_TTSGetParam			m_TTSGetParam;
	static Proc_TTSSetParam			m_TTSSetParam;
	static Proc_TTSLoadUserLib		m_TTSLoadUserLib;
	static Proc_TTSDisconnect		m_TTSDisconnect;
	static Proc_TTSConnect			m_TTSConnect;
	static Proc_TTSUnloadUserLib	m_TTSUnloadUserLib;
	static Proc_TTSSynthText2File	m_TTSSynthText2File;
	static Proc_TTSFormatMessage	m_TTSFormatMessage;
	static Proc_TTSClean			m_TTSClean;
	static const std::string TTSGetErrorMsg(TTSINT32 code);
	static TTSRETVAL SynthProcessProc(HTTSINSTANCE instance,PTTSData tts_data, TTSINT32 lparam, PTTSVOID user_data);
	static int  CatWavFile(const char* wave_file, unsigned char * buf_new, TTSDWORD buf_new_len);
	friend class iFlyTTsInstance;
};
#endif
