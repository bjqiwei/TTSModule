#ifndef __MSGDEFINE_H__
#define __MSGDEFINE_H__

#include <string>
#include "tinyxml2.h"
#include <sstream>

#define  ERROR_BASE 190000
typedef enum {
	Http_BodyInvalidXML			= ERROR_BASE +1,
	Http_MissingRequestElement	= ERROR_BASE +2,
	Http_MissingServiceidElement= ERROR_BASE +3,
	Http_MissingSessionidElement= ERROR_BASE +4,
	Http_MissingTxtElement		= ERROR_BASE +5,
	Http_BodyInvalid			= ERROR_BASE +6,
	Http_InvalidResponseType	= ERROR_BASE +7,
	Http_MissingResponseIPElement= ERROR_BASE +8,
	TTSMod_Warnning_Closed		= ERROR_BASE +88,
	TTSMod_TimeOut_Transfer		= ERROR_BASE +100,
	TTSMod_Error_CreatePath		= ERROR_BASE +101,
	TTSMod_Error_CreatePath2	= ERROR_BASE +102,
	TTSMod_Error_Engine			= ERROR_BASE +103,
	TTSMod_Error_NUll_Path		= ERROR_BASE +104,
	TTSMod_Error_TTSThread		= ERROR_BASE +105,
	TTSMod_HttpBodyInvalid		= ERROR_BASE +106,
	TTSMod_Error_Version		= ERROR_BASE +109,
	TTSMod_Error_Init			= ERROR_BASE +110,
	TTSMod_Error_TTS			= ERROR_BASE +111,
	TTSMod_Error_ReleaseIns		= ERROR_BASE +113,
	TTSMod_Error_Socket			= ERROR_BASE +114,
	TTSMod_Error_HttpListen		= ERROR_BASE +201,
	TTSMod_NoLicense			= ERROR_BASE +1001,

} ErrorCode;


class CMessage
{
public:
	CMessage()
	{
	};
	virtual ~CMessage(){};

};

class CTTSMessage:public CMessage{
public:
	CTTSMessage():m_Respport(80),m_Resptype(0),m_nVID(0),m_nSpeed(0),m_nVolume(0),
		m_nPitch(0),m_nBGSound(0),m_nAudioFmt(0),m_Request(NULL){

	};
	virtual ~CTTSMessage(){

	};
	std::string m_Serviceid;
	std::string m_Sessionid;
	std::string m_TransferTxt;
	std::string m_TxtEncode;
	std::string m_TtsFile;
	std::string m_Respurl;
	std::string m_Respip;
	int m_Respport;
	int m_Resptype;
	int m_nVID;
	int m_nSpeed;
	int m_nVolume;
	int m_nPitch;
	int m_nBGSound;
	int m_nAudioFmt;
	void * m_Request;
	
public:
	static const std::string MakeHttpResponseWithErrorCode(ErrorCode err)
	{
		tinyxml2::XMLDocument xmlDoc;
		xmlDoc.InsertEndChild(xmlDoc.NewDeclaration());
		tinyxml2::XMLNode* eleRes = xmlDoc.InsertEndChild(xmlDoc.NewElement("Response"));
		tinyxml2::XMLNode* elePlayTTs = eleRes->InsertEndChild(xmlDoc.NewElement("RespPlaytts"));
		tinyxml2::XMLNode* statuscode = elePlayTTs->InsertEndChild(xmlDoc.NewElement("statuscode"));
		std::stringstream errStr;
		errStr << err;
		statuscode->InsertFirstChild(xmlDoc.NewText(errStr.str().c_str()));

		tinyxml2::XMLPrinter streamer;
		xmlDoc.Print( &streamer );
		return streamer.CStr();
	}

	static const std::string MakeDownLoadFileUrl(CTTSMessage * msg, const std::string & httpFileSererIp,
		const unsigned int httpFileServerPort, const std::string & systemFileServerPath){

		std::stringstream url;
		url << "http://" << httpFileSererIp << ":" <<  httpFileServerPort;
		url << "/" << msg->m_TtsFile.substr(systemFileServerPath.length());
		return url.str();
	}

	static const std::string MakeHttpResponseWithMessage(CTTSMessage * msg, const std::string & httpFileSererIp,
		const unsigned int httpFileServerPort, const std::string & systemFileServerPath)
	{
		tinyxml2::XMLDocument xmlDoc;
		xmlDoc.InsertEndChild(xmlDoc.NewDeclaration());
		tinyxml2::XMLNode* eleRes = xmlDoc.InsertEndChild(xmlDoc.NewElement("Response"));
		tinyxml2::XMLNode* elePlayTTs = eleRes->InsertEndChild(xmlDoc.NewElement("RespPlaytts"));
		tinyxml2::XMLNode* statuscode = elePlayTTs->InsertEndChild(xmlDoc.NewElement("statuscode"));
		statuscode->InsertFirstChild(xmlDoc.NewText("000000"));
		tinyxml2::XMLNode * sessionid = elePlayTTs->InsertEndChild(xmlDoc.NewElement("sessionid"));
		sessionid->InsertFirstChild(xmlDoc.NewText(msg->m_Sessionid.c_str()));
		tinyxml2::XMLNode * file = elePlayTTs->InsertEndChild(xmlDoc.NewElement("file"));

		std::string url = MakeDownLoadFileUrl(msg,httpFileSererIp,httpFileServerPort,systemFileServerPath);
		file->InsertFirstChild(xmlDoc.NewText(url.c_str()));

		tinyxml2::XMLPrinter streamer;
		xmlDoc.Print( &streamer );

		return streamer.CStr();
	}

};
#endif