#ifndef _ALARM_H_
#define _ALARM_H_
#include <log4cplus/logger.h>
#include "httpserver.h"
#include "config.h"
#include "msgdefine.h"

//告警级别
enum AlarmLevel
{
	AL_Fatal		=0x0,
	AL_Critical		=0x1,
	AL_Important	=0x2,
	AL_Common		=0x3,
	AL_Notify		=0x4,
	AL_DEBUG		=0x5,
	AL_END
};
//告警相关消息 0 -- 20000
#define RespMsgBaseID      10000
enum AlarmMsg_Type
{
	MsgType_Alarm=0,						//0 告警消息
	MsgType_HeartBeat,						//1 监听心跳包
	MsgType_ExchangeMaster,					//2 切换告警中心
	MsgType_TPAS_Alarm,						//3 第三方业务侧告警
	MsgType_Send2Master_HeartBeat,			//4 备告警给主告警发送心跳包
	MsgType_Send2Modules_HeartBeat,			//5 给各个模块发送心跳包
	//响应
	MsgType_Resp_Alarm=RespMsgBaseID,		//10000 心跳包消息
	MsgType_Resp_HeartBeat,					//10001 心跳包响应消息
	MsgType_Resp_ExchangeMaster,			//10002 切换响应消息
	MsgType_Resp_TPAS_Alarm,

	//其他
	MsgType_Resp_SendSMS=RespMsgBaseID*2,	//发送短信
	MsgType_Resp_TransferTTS,				//发送TTS转换
	MsgType_Resp_IVRCall,					//发送IVR呼叫

	MsgType_End

};
#define AlarmMsg_Create       0   //创建告警消息
#define AlarmMsg_Cancel       1   //取消告警消息

class TAlarm
{
public:
	TAlarm(HttpServer &httpClient);
	virtual~TAlarm();
	void Start(const std::string & systemIP, const std::string & systemEntiryId,
		const std::string & alarmCenterIp, int alarmCenterPort);
	void Stop();
	void PostAlarmMessage(ErrorCode errId, AlarmLevel lev, const std::string & des, const log4cplus::Logger & log) const ;
private:
	std::string CreateAlarmMsg(ErrorCode alarmid,AlarmLevel lev, const std::string & des) const;
private:
	log4cplus::Logger log;
	HttpServer &m_HttpClient;
	std::string m_LocalIP;
	std::string m_EntityID;
	std::string m_AlarmCenterIP;
	int	m_AlarmCenterPort;
	static const std::string m_ModuleName;
	static const std::string m_Url;
};

#endif


