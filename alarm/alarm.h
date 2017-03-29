#ifndef _ALARM_H_
#define _ALARM_H_
#include <log4cplus/logger.h>
#include "httpserver.h"
#include "config.h"
#include "msgdefine.h"

//�澯����
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
//�澯�����Ϣ 0 -- 20000
#define RespMsgBaseID      10000
enum AlarmMsg_Type
{
	MsgType_Alarm=0,						//0 �澯��Ϣ
	MsgType_HeartBeat,						//1 ����������
	MsgType_ExchangeMaster,					//2 �л��澯����
	MsgType_TPAS_Alarm,						//3 ������ҵ���澯
	MsgType_Send2Master_HeartBeat,			//4 ���澯�����澯����������
	MsgType_Send2Modules_HeartBeat,			//5 ������ģ�鷢��������
	//��Ӧ
	MsgType_Resp_Alarm=RespMsgBaseID,		//10000 ��������Ϣ
	MsgType_Resp_HeartBeat,					//10001 ��������Ӧ��Ϣ
	MsgType_Resp_ExchangeMaster,			//10002 �л���Ӧ��Ϣ
	MsgType_Resp_TPAS_Alarm,

	//����
	MsgType_Resp_SendSMS=RespMsgBaseID*2,	//���Ͷ���
	MsgType_Resp_TransferTTS,				//����TTSת��
	MsgType_Resp_IVRCall,					//����IVR����

	MsgType_End

};
#define AlarmMsg_Create       0   //�����澯��Ϣ
#define AlarmMsg_Cancel       1   //ȡ���澯��Ϣ

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


