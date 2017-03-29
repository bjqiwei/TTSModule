// HttpServer.h: interface for the HttpServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <log4cplus/logger.h>
#include <log4cplus/thread/threads.h>
#include "CEventBuffer.h"
#include "mongoose.h"
#include "http.h"
#include <map>
#include <list>

class HttpServer;
class HttpServerThread : public log4cplus::thread::AbstractThread
{
public:
	explicit HttpServerThread(HttpServer *);
	virtual ~HttpServerThread();
	virtual void run();
protected:
	HttpServer * server;
	mg_server * m_mgServer;
	log4cplus::Logger log;
	size_t m_SocketNum;
	friend class HttpServer;
};

class HttpServer
{
public:
	HttpServer();
	virtual ~HttpServer();
	bool StartHttpServer(const std::string & ip ,unsigned short port);
	void StopHttpServer();
	bool GetHttpRequest(THttpRequest *& request, unsigned long dwMilliseconeds);
	void SetAllowIp(const std::vector<std::string> & allowIp);
	bool PutHttpSendMsg2Queue(THttpRequest * pmsg);
	const log4cplus::Logger & GetLog() { return log;}; 
protected:
	log4cplus::Logger log;
	volatile bool m_Run;
	mg_server * m_mgServer;
	static int mg_evhandler(struct mg_connection *conn, enum mg_event ev);
	helper::CEventBuffer<THttpRequest * > m_HttpRequest;
	std::map<std::string,int>m_AllowIp;
	//httpclient
	helper::CEventBuffer<THttpRequest *> m_SendMsgQueue;
	std::list<HttpServerThread * > m_ThreadInstance;
	unsigned int m_threadNum;
	helper::CLock m_AllowIpLock;
	friend class HttpServerThread;
};

class HttpFileServer : public log4cplus::thread::AbstractThread
{
public:
	HttpFileServer();
	virtual ~HttpFileServer();
	bool StartHttpServer(const std::string & ip ,unsigned short port, const std::string & document_root);
	void StopHttpServer();
	void SetAllowIp(const std::vector<std::string> & allowIp);
private:
	virtual void run();
private:
	log4cplus::Logger log;
	volatile bool m_Run;
	mg_server * m_mgServer;
	std::map<std::string,int>m_AllowIp;
};
#endif // !defined(AFX_HTTPSERVER_H__F2E0F7A7_2673_47C4_B35E_2C85BDD65500__INCLUDED_)
