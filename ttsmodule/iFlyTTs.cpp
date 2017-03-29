#include "iFlyTTs.h"
#include <stdio.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/sleep.h>
#include "../common/common.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//两个字节的WORD高低位倒转
#define TTS_WORD_LE_BE(val)	 ((TTSWORD) ( (((TTSWORD) (val) & (TTSWORD) 0x00ffU) << 8) | \
	(((TTSWORD) (val) & (TTSWORD) 0xff00U) >> 8)))
//四个字节的DWORD高低位倒转
#define TTS_DWORD_LE_BE(val) ((TTSDWORD) ( (((TTSDWORD) (val) & (TTSDWORD) 0x000000ffU) << 24) | \
	(((TTSDWORD) (val) & (TTSDWORD) 0x0000ff00U) <<  8) | \
	(((TTSDWORD) (val) & (TTSDWORD) 0x00ff0000U) >>  8) | \
	(((TTSDWORD) (val) & (TTSDWORD) 0xff000000U) >> 24)))

static inline bool IsLittleEndian( )
{
	TTSDWORD dwTestValue = 0xFF000000;
	return *((char*)(&dwTestValue))==0;
}

/** 
 * @brief 	CatWavFile
 *  
 * Append some audio data to a audiofile
 *  
 * @author	lygao
 * @date	2005-11-27
 * @return	int	- Return 0 in success, otherwise return error code.
 * @param	const char* wave_file	- [in,out] 
 * @param	PBYTE buf_new	- [in] 
 * @param	TTSDWORD buf_new_len	- [in] 
 * @see		
 */
int  iFlyTTs::CatWavFile(const char* wave_file, unsigned char * buf_new, TTSDWORD buf_new_len)
{
	FILE* fp;
	TTSDWORD file_size = 0, data_size = 0, wav_size = 0;

	if ( buf_new_len <= 0 )
	{
		return TTSERR_OK;
	}

	std::cout << TEXT("*");

	fp = fopen(wave_file, "r+b");
	if ( fp == NULL )
	{
		fp = fopen(wave_file, "wb");
	}

	if ( fp == NULL )
	{
		return TTSERR_WRITEFILE;
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	if (file_size == 0  /*|| raw_audio_data_ == TRUE*/)
	{
		fwrite(buf_new, 1, buf_new_len, fp);
	}
	else
	{
		TTSDWORD head_size = 0;
		/*if( synth_param_[TTS_PARAM_AUDIODATAFMT] >= TTS_ADF_PCM8K8B1C &&
			synth_param_[TTS_PARAM_AUDIODATAFMT] <= TTS_ADF_PCM6K16B1C)
		{
			head_size = 44;
		}*/

		// aLaw/uLaw Wav数据
		//if(synth_param_[TTS_PARAM_AUDIODATAFMT] >= TTS_ADF_ALAW16K1C &&
		//	synth_param_[TTS_PARAM_AUDIODATAFMT] <= TTS_ADF_ULAW6K1C)
		//{
		//	head_size = 58;
		//}
		//else
		//{
		//	head_size = 44;
		//}
		head_size = 44;	
		fwrite(buf_new + head_size, 1, buf_new_len - head_size, fp);
		file_size = file_size + buf_new_len - head_size;
		data_size = file_size - head_size;
		wav_size = file_size - 8;

		fseek(fp, head_size - sizeof(TTSDWORD), SEEK_SET);
		if ( !IsLittleEndian() )
		{
			data_size = TTS_DWORD_LE_BE(data_size);
		}
		fwrite(&data_size, 1, sizeof(TTSDWORD), fp);
		fseek(fp, 4, SEEK_SET);
		if ( !IsLittleEndian() )
		{
			wav_size = TTS_DWORD_LE_BE(wav_size);
		}
		fwrite(&wav_size, 1, sizeof(TTSDWORD), fp);
	}

	fclose(fp);
	//fflush(stdout);
	return TTSERR_OK;
}

/** 
 * @brief 	SynthProcessProc
 *  
 *  TTS processing callback function
 *  
 * @author	lygao
 * @date	2005-11-27
 * @return	TTSRETVAL	- Return 0 in success, otherwise return error code.
 * @param	HTTSINSTANCE instance	- [in] 
 * @param	PTTSData tts_data	- [in] 
 * @param	TTSINT32 lparam	- [in] 
 * @param	PTTSVOID user_data	- [in] 
 * @see		
 */
TTSRETVAL iFlyTTs::SynthProcessProc(HTTSINSTANCE instance,
						   PTTSData tts_data, TTSINT32 lparam, PTTSVOID user_data)
{
	iFlyTTsInstance * This = reinterpret_cast<iFlyTTsInstance *>(user_data);
	if (tts_data->dwOutBufSize > 0)
	{
		int ret = CatWavFile(This->m_Filename.c_str(), 
			(unsigned char*)tts_data->pOutBuf, tts_data->dwOutBufSize);
		if (ret != TTSERR_OK){
			LOG4CPLUS_ERROR(This->log, iFlyTTs::TTSGetErrorMsg(ret));
			This->m_TTSModule.m_Alarm.PostAlarmMessage(TTSMod_Error_CreatePath, AL_Fatal,"Create wav file failed.",This->log);
			return TTSGETERRCODE(ret);
		}
	}

	if (tts_data->dwOutFlags == TTS_FLAG_DATA_END)
	{
	}
	
	// You can return TTSERR_CANCEL to stop Synthesizing Procedure
	return tts_data->dwErrorCode; 
}

Proc_TTSInitializeEx	iFlyTTs::m_TTSInitializeEx(NULL);
Proc_TTSUninitialize	iFlyTTs::m_TTSUninitialize(NULL);
Proc_TTSSynthTextEx		iFlyTTs::m_TTSSynthTextEx(NULL);
Proc_TTSSynthText		iFlyTTs::m_TTSSynthText(NULL);
Proc_TTSFetchNext		iFlyTTs::m_TTSFetchNext(NULL);
Proc_TTSGetParam		iFlyTTs::m_TTSGetParam(NULL);
Proc_TTSSetParam		iFlyTTs::m_TTSSetParam(NULL);
Proc_TTSLoadUserLib		iFlyTTs::m_TTSLoadUserLib(NULL);
Proc_TTSDisconnect		iFlyTTs::m_TTSDisconnect(NULL);
Proc_TTSConnect			iFlyTTs::m_TTSConnect(NULL);
Proc_TTSUnloadUserLib	iFlyTTs::m_TTSUnloadUserLib(NULL);
Proc_TTSSynthText2File	iFlyTTs::m_TTSSynthText2File(NULL);
Proc_TTSFormatMessage	iFlyTTs::m_TTSFormatMessage(NULL);
Proc_TTSClean			iFlyTTs::m_TTSClean(NULL);

TTSCON_Dll_Handle		iFlyTTs::lib;
#ifdef WIN32
std::string				iFlyTTs::ttslib_Name = "iFlyTTS.dll";
#else
std::string				iFlyTTs::ttslib_Name = "libsem.so";
#endif
std::string ttslib_Name;

iFlyTTs::iFlyTTs(const TAlarm & alarm):TTSModule(alarm),m_nTTSCode(0)
{
	this->log = log4cplus::Logger::getInstance("iFlyTTs");

	LOG4CPLUS_TRACE(log, "Construction");
}


iFlyTTs::~iFlyTTs(void)
{
	if (!m_FlyTTsInstance.empty())
	{
		StopTTSModule();
	}
	LOG4CPLUS_TRACE(log, "Destruction");
}

bool iFlyTTs::StartTTSModule(unsigned long licenseNum, const std::string &SerialNumber, int code)
{
	LOG4CPLUS_TRACE(log, "StartTTSModule, licenseNum:" << licenseNum << ", SerialNumber:" << SerialNumber << ", Code:" << code);
	if (!InitTTSProc()){
		m_Alarm.PostAlarmMessage(TTSMod_Error_Init, AL_Fatal, "加载TTS动态库失败",log);
	}
	Initialize();
	m_SerialNumber = SerialNumber;
	m_nTTSCode = code;
	for (unsigned long i =0 ; i < licenseNum ; i++)
	{
		iFlyTTsInstance * inst = new iFlyTTsInstance(this->log, *this);
		inst->Start();
		m_FlyTTsInstance.push_back(inst);
	}
	if (m_FlyTTsInstance.empty()) return false;
	return true ;
}

void iFlyTTs::StopTTSModule()
{
	LOG4CPLUS_TRACE(log, "StopTTSModule");

	std::list<iFlyTTsInstance * > m_tempIns = m_FlyTTsInstance;
	m_FlyTTsInstance.clear();
	for (std::list<iFlyTTsInstance * >::const_iterator it = m_tempIns.begin();
		it != m_tempIns.end(); ++it)
	{
		(*it)->Stop();
	}
	for (std::list<iFlyTTsInstance * >::const_iterator it = m_tempIns.begin();
		it != m_tempIns.end(); ++it)
	{
		if((*it)->isRunning())
			(*it)->join();
		delete (*it);
	}
	Uninitialize();
	UninitTTSProc();
}

unsigned int iFlyTTs::GetInstanceNum()
{
	return m_FlyTTsInstance.size();
}

bool iFlyTTs::InitTTSProc()
{
	void* lib_handle = TTSCON_INVALID_HANDLE;
	lib.open(ttslib_Name.c_str(),lib_handle);
	lib_handle = lib.get_handle();

	if ( lib_handle == TTSCON_INVALID_HANDLE )
	{		
		LOG4CPLUS_ERROR(log, "Load "<< ttslib_Name <<" fail!");
		return false;
	}

	m_TTSInitializeEx = ( Proc_TTSInitializeEx) lib.get_func_addr(TEXT("TTSInitializeEx"));
	if ( !m_TTSInitializeEx ){		
		LOG4CPLUS_ERROR(log, "GetProc TTSInitializeEx Error.");
		lib.close(1);
		return false;
	}

	m_TTSSynthTextEx = ( Proc_TTSSynthTextEx) lib.get_func_addr(TEXT ("TTSSynthTextEx"));
	if ( !m_TTSSynthTextEx ){		
		LOG4CPLUS_ERROR(log, "GetProc TTSSynthTextEx Error.");
		lib.close(1);
		return false;
	}

	m_TTSSynthText = ( Proc_TTSSynthText) lib.get_func_addr( TEXT("TTSSynthText"));
	if (!m_TTSSynthText ){	
		LOG4CPLUS_ERROR(log, "GetProc TTSSynthText Error.");
		lib.close(1);
		return false;
	}

	m_TTSFetchNext = ( Proc_TTSFetchNext) lib.get_func_addr( TEXT("TTSFetchNext"));
	if ( !m_TTSFetchNext ){
		LOG4CPLUS_ERROR(log, "GetProc TTSFetchNext Error.");
		lib.close(1);
		return false;
	}
	
	m_TTSGetParam = ( Proc_TTSGetParam) lib.get_func_addr(TEXT("TTSGetParam"));
	if ( !m_TTSGetParam ){
		LOG4CPLUS_ERROR(log, "GetProc TTSGetParam Error.");
		lib.close(1);
		return false;
	}

	m_TTSSetParam = ( Proc_TTSSetParam) lib.get_func_addr(TEXT("TTSSetParam"));
	if ( !m_TTSSetParam ){
		LOG4CPLUS_ERROR(log, "GetProc TTSSetParam Error.");
		lib.close(1);
		return false;
	}

	m_TTSLoadUserLib = ( Proc_TTSLoadUserLib) lib.get_func_addr(TEXT("TTSLoadUserLib"));
	if ( !m_TTSLoadUserLib )
	{
		LOG4CPLUS_ERROR(log, "GetProc TTSLoadUserLib Error.");
		lib.close(1);
		return false;
	}

	m_TTSUninitialize  = ( Proc_TTSUninitialize ) lib.get_func_addr(TEXT("TTSUninitialize"));
	if ( !m_TTSUninitialize)
	{		
		LOG4CPLUS_ERROR(log, "GetProc TTSUninitialize Error.");
		lib.close(1);
		return false;
	}

	m_TTSDisconnect = ( Proc_TTSDisconnect) lib.get_func_addr(TEXT("TTSDisconnect"));
	if ( !m_TTSDisconnect ){	
		LOG4CPLUS_ERROR(log, "GetProc TTSDisconnect Error.");
		lib.close(1);
		return false;
	}


	m_TTSConnect = ( Proc_TTSConnect) lib.get_func_addr(TEXT("TTSConnect"));
	if ( !m_TTSConnect){
		LOG4CPLUS_ERROR(log, "GetProc TTSConnect Error.");
		lib.close(1);
		return false;
	}
	
	m_TTSUnloadUserLib = ( Proc_TTSUnloadUserLib) lib.get_func_addr(TEXT("TTSUnloadUserLib"));
	if ( !m_TTSUnloadUserLib )
	{
		LOG4CPLUS_ERROR(log, "GetProc TTSUnloadUserLib Error.");
		lib.close(1);
		return false;
	}
	m_TTSSynthText2File = ( Proc_TTSSynthText2File) lib.get_func_addr(TEXT("TTSSynthText2File"));
	if ( !m_TTSSynthText2File )
	{
		LOG4CPLUS_ERROR(log, "GetProc TTSSynthText2File Error.");
		lib.close(1);
		return false;
	}

	m_TTSFormatMessage = ( Proc_TTSFormatMessage) lib.get_func_addr(TEXT("TTSFormatMessage"));
	if ( !m_TTSFormatMessage )
	{
		LOG4CPLUS_ERROR(log, "GetProc TTSFormatMessage Error.");
		lib.close(1);
		return false;
	}

	m_TTSClean = ( Proc_TTSClean) lib.get_func_addr(TEXT("TTSClean"));
	if ( !m_TTSClean )
	{
		LOG4CPLUS_ERROR(log, "GetProc TTSClean Error.");
		lib.close(1);
		return false;
	}
	return true;
}

void iFlyTTs::UninitTTSProc()
{
	if(lib.close(1) != 0){
		LOG4CPLUS_ERROR(log, "UninitTTSProc error.");
	}
}

bool iFlyTTs::Initialize(){

	TTSRETVAL ret ;
	
	if (m_TTSInitializeEx && (ret = m_TTSInitializeEx("intp60",NULL)) != TTSERR_OK)
	{
		LOG4CPLUS_ERROR(log, "Error in initializing TTS system, " << TTSGetErrorMsg(ret));	
		m_Alarm.PostAlarmMessage(TTSMod_Error_Init,AL_Fatal,TTSGetErrorMsg(ret),log);
		return false;
	}
	return true;
}

bool iFlyTTs::Uninitialize()
{
	TTSRETVAL ret; 		
	if (m_TTSUninitialize && (ret = m_TTSUninitialize()) != TTSERR_OK)
	{
		LOG4CPLUS_ERROR(log, "TTSUninitialize Error:" << TTSGetErrorMsg(ret));
		return false;
	}
	return true;
}

const std::string iFlyTTs::TTSGetErrorMsg(TTSINT32 code)
{
	TTSINT16 bufferLen = 1024;
	char buffer[1024] = "";
	m_TTSFormatMessage?m_TTSFormatMessage(code, buffer, &bufferLen):0;
	return std::string(buffer);
}
iFlyTTsInstance::iFlyTTsInstance(log4cplus::Logger parentLog,iFlyTTs & ttsmodule):m_bRun(false),m_TTSModule(ttsmodule)
	,m_instance(NULL),m_nMaxInBufSize(0)
{
	std::stringstream oss;
	oss << parentLog.getName() << ".Instance." << this; 
	this->log = log4cplus::Logger::getInstance(oss.str());
	memset(&ttsData, 0, sizeof(ttsData));
	TtsCallBacks.nNumCallbacks = 1;
	TtsCallBacks.pfnTTSProcessCB = iFlyTTs::SynthProcessProc;
	TtsCallBacks.pfnTTSEventCB = NULL;
	LOG4CPLUS_TRACE(log, "Construction");
}


iFlyTTsInstance::~iFlyTTsInstance(void)
{
	LOG4CPLUS_TRACE(log, "Destruction");
}

bool iFlyTTsInstance::Start()
{
	LOG4CPLUS_INFO(log, "Start");
	if (!isRunning())
	{
		m_bRun = true;
		start();
	}
	else{
		LOG4CPLUS_WARN(log, " Already running.");
	}
	return true;
}
void iFlyTTsInstance::Stop()
{
	if (!isRunning()){
		return;
	}
	m_bRun = false;
	LOG4CPLUS_INFO(log, "Stop");
}
void iFlyTTsInstance::run()
{
	LOG4CPLUS_TRACE(log, "run start");
	static log4cplus::Logger msglog = log4cplus::Logger::getInstance("MsgQueue");
	static log4cplus::Logger timelog = log4cplus::Logger::getInstance("TTSTime");
	CTTSMessage * msg = NULL;
	while(m_bRun){
		msg = NULL;
		m_TTSModule.m_RecMsgQueue.getData(msg,200);
		if(msg == NULL) continue;
		LOG4CPLUS_INFO(msglog, "CTTSMessage:" << m_TTSModule.m_RecMsgQueue.size());
		LOG4CPLUS_INFO(timelog, "Start transfer.");
		if (!Connect(msg))
		{
			LOG4CPLUS_ERROR(log, "Connect TTS Error,Sleep 2*1000 millis.");
			m_TTSModule.PutMessage(msg);
			log4cplus::helpers::sleepmillis(2*1000);
			continue;
		}
		SetParam(msg);

		m_Filename = msg->m_TtsFile;
		if(m_Filename[m_Filename.size()-1]!='/') m_Filename.append("/");
		m_Filename.append(msg->m_Serviceid);
		m_Filename.append("/");
		mkpath(m_Filename);
		m_Filename.append(msg->m_Sessionid);
		m_Filename.append("_");
		log4cplus::helpers::Time tem = log4cplus::helpers::Time::gettimeofday();
		m_Filename.append(tem.getFormattedTime("%Y-%m-%d-%H-%M-%S.%Q"));
		m_Filename.append(".wav");

		
		LOG4CPLUS_INFO(log,"TTS Context:" << msg->m_TransferTxt << " to fileName:" << m_Filename);
		
		std::string::size_type pos = 0;
		bool isError = false;
		while(pos < msg->m_TransferTxt.length()){
			unsigned long len = min(msg->m_TransferTxt.length() - pos, m_nMaxInBufSize);
			{
				ttsData.dwInBufSize = len;
			}
			ttsData.szInBuf = (TTSCHAR *)(msg->m_TransferTxt.c_str() + pos);
			pos += len;
			if (m_TTSModule.m_TTSSynthTextEx == NULL){
				isError = true;
				break;
			}
			TTSRETVAL ret = m_TTSModule.m_TTSSynthTextEx(m_instance, &ttsData, &TtsCallBacks, FALSE, (PTTSVOID)this);
			if (ret != TTSERR_OK){
				LOG4CPLUS_ERROR(log, iFlyTTs::TTSGetErrorMsg(ret) );
				isError = true;
				break;
			}
			
		}
		iFlyTTs::m_TTSClean ? m_TTSModule.m_TTSClean(this->m_instance):NULL;

		if (!isError)
		{
			LOG4CPLUS_TRACE(log,"TTS success.");
			if (msg->m_nAudioFmt == TTS_ADF_MP38K1C || msg->m_nAudioFmt == TTS_ADF_MP316K1C )
			{
				std::string convertcmd = "ffmpeg -i ";
				convertcmd.append(m_Filename);
				convertcmd.append(" -f mp3 -v 0 ");
				msg->m_nAudioFmt == TTS_ADF_MP38K1C ? convertcmd.append("-ar 8000 "):"";
				msg->m_nAudioFmt == TTS_ADF_MP316K1C ? convertcmd.append("-ar 16000 "):"";
				m_Filename = m_Filename.substr(0,m_Filename.length()-4) + ".mp3";
				convertcmd.append(m_Filename);
				LOG4CPLUS_DEBUG(log,"wav2mp3 cmd:" << convertcmd);
				system(convertcmd.c_str());
			}
			msg->m_TtsFile = m_Filename;
			m_TTSModule.m_ResultMsgQueue.addData(msg);
			LOG4CPLUS_INFO(msglog, "CTTSResultMessage:" << m_TTSModule.m_ResultMsgQueue.size());
		}else{
			DisConnect();
			m_TTSModule.m_Alarm.PostAlarmMessage(TTSMod_Error_TTS,AL_Important,msg->m_Sessionid,log);
			m_TTSModule.PutMessage(msg);
			LOG4CPLUS_ERROR(log, " sleep 2000 millis.");
			log4cplus::helpers::sleepmillis(2*1000);
		}
		LOG4CPLUS_INFO(timelog, "Stop transfer.");
	}
	DisConnect();
	LOG4CPLUS_TRACE(log, "run stop");
}

bool iFlyTTsInstance::Connect(const CTTSMessage * param)
{
	bool result = true;
	if (this->m_instance){ 
		return result;
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	std::stringstream oss;
	oss << std::hex << this;
	std::string user_name = oss.str();
	const char *product_name	= "iFly TTS";
	memset(&ttsConnect, 0, sizeof(TTSConnectStruct));
	ttsConnect.dwSDKVersion = IFLYTTS_SDK_VER;
	strcpy(ttsConnect.szUserName, user_name.c_str());
	strcpy(ttsConnect.szCompanyName, "Cloopen");
	strcpy(ttsConnect.szSerialNumber, m_TTSModule.m_SerialNumber.c_str());
	strcpy(ttsConnect.szProductName, product_name);
	strcpy(ttsConnect.szServiceUID, "intp60");
	ttsConnect.bSetParams = TRUE;
	ttsConnect.nCodePage = m_TTSModule.m_nTTSCode;

	m_instance = m_TTSModule.m_TTSConnect?m_TTSModule.m_TTSConnect(&ttsConnect):NULL;
	if (m_instance == NULL){	
		LOG4CPLUS_ERROR(log, iFlyTTs::TTSGetErrorMsg(ttsConnect.dwErrorCode));
		if (TTSGETERRCODE(ttsConnect.dwErrorCode) == TTSERR_NOLICENCE){
			m_TTSModule.m_Alarm.PostAlarmMessage(TTSMod_NoLicense,AL_Important,iFlyTTs::TTSGetErrorMsg(ttsConnect.dwErrorCode),log);
		}
		else {
			m_TTSModule.m_Alarm.PostAlarmMessage(TTSMod_Error_TTS,AL_Important,iFlyTTs::TTSGetErrorMsg(ttsConnect.dwErrorCode),log);
		}
		result = false;
	}

	if (result)
	{
		TTSINT32 len = sizeof(m_nMaxInBufSize);
		TTSRETVAL nRet = iFlyTTs::m_TTSGetParam ? m_TTSModule.m_TTSGetParam(m_instance, TTS_PARAM_INBUFSIZE, &m_nMaxInBufSize,&len):0;
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Get TTS system Parameter, " << iFlyTTs::TTSGetErrorMsg(nRet));
			m_TTSModule.m_Alarm.PostAlarmMessage(TTSMod_Error_TTS,AL_Important,iFlyTTs::TTSGetErrorMsg(nRet),log);
			result = false;
		}else{
			LOG4CPLUS_DEBUG(log, "INBUFSIZE:" << m_nMaxInBufSize);
		}
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return result;
}

bool iFlyTTsInstance::SetParam(const CTTSMessage * param)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (iFlyTTs::m_TTSSetParam)
	{
		
		TTSRETVAL nRet = m_TTSModule.m_TTSSetParam(m_instance, TTS_PARAM_VID, const_cast<int *>(&(param->m_nVID)), sizeof(param->m_nVID));
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Set TTS system Parameter Vid, " << iFlyTTs::TTSGetErrorMsg(nRet));
		}
		LOG4CPLUS_DEBUG(log,"nVid:" << param->m_nVID);
		

		
		nRet = m_TTSModule.m_TTSSetParam(m_instance, TTS_PARAM_SPEED, const_cast<int *>(&(param->m_nSpeed)), sizeof(param->m_nSpeed));
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Set TTS system Parameter Speed, " << iFlyTTs::TTSGetErrorMsg(nRet));
		}
		LOG4CPLUS_DEBUG(log,"nSpeed:" << param->m_nSpeed);
		

		
		nRet = m_TTSModule.m_TTSSetParam(m_instance, TTS_PARAM_VOLUME, const_cast<int *>(&(param->m_nVolume)), sizeof(param->m_nVolume));
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Set TTS system Parameter Volume, " << iFlyTTs::TTSGetErrorMsg(nRet));
		}
		LOG4CPLUS_DEBUG(log,"nVolume:" << param->m_nVolume);
		
		
		
		nRet = m_TTSModule.m_TTSSetParam(m_instance, TTS_PARAM_PITCH, const_cast<int *>(&(param->m_nPitch)), sizeof(param->m_nPitch));
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Set TTS system Parameter Pitch, " << iFlyTTs::TTSGetErrorMsg(nRet));
		}
		LOG4CPLUS_DEBUG(log,"nPitch:" << param->m_nPitch);
		

		
		nRet = m_TTSModule.m_TTSSetParam(m_instance, TTS_PARAM_BGSOUND, const_cast<int *>(&(param->m_nBGSound)), sizeof(param->m_nBGSound));
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Set TTS system Parameter BGSound, " << iFlyTTs::TTSGetErrorMsg(nRet));
		}
		LOG4CPLUS_DEBUG(log,"nBGSound:" << param->m_nBGSound);

		int code = m_TTSModule.m_nTTSCode;
		if (param->m_TxtEncode == "GB2312"){
			code = TTS_CP_GB2312;
		}
		else if (param->m_TxtEncode == "GBK"){
			code = TTS_CP_GBK;
		}
		else if (param->m_TxtEncode == "BIG5"){
			code = TTS_CP_BIG5;
		}
		else if (param->m_TxtEncode == "UNICODE"){
			code = TTS_CP_UNICODE;
		}
		else if(param->m_TxtEncode == "GB18030"){
			code = TTS_CP_GB18030;
		}
		else if (param->m_TxtEncode == "UTF-8"){
			code = TTS_CP_UTF8;
		}

		nRet = m_TTSModule.m_TTSSetParam(m_instance, TTS_PARAM_CODEPAGE, &code, sizeof(code));
		if(nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log,"Error in Set TTS system Parameter CodePage, " << iFlyTTs::TTSGetErrorMsg(nRet));
		}
		LOG4CPLUS_DEBUG(log,"Encode:" << param->m_TxtEncode);
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}

bool iFlyTTsInstance::DisConnect()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (m_instance)
	{
		TTSRETVAL nRet = m_TTSModule.m_TTSDisconnect(m_instance);
		m_instance = NULL;
		if (nRet != TTSERR_OK){
			LOG4CPLUS_ERROR(log, iFlyTTs::TTSGetErrorMsg(nRet));
		}
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return true;
}