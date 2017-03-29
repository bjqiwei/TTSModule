// HttpServer.cpp: implementation of the HttpServer class.
//
//////////////////////////////////////////////////////////////////////

#include "httpserver.h"
#include <sstream>
#include <log4cplus/loggingmacros.h>
#include <string>
#include <string.h>

int HttpServer::mg_evhandler(struct mg_connection *conn, enum mg_event ev) {
	//char *buf = (char *) conn->connection_param;
	static log4cplus::Logger msglog = log4cplus::Logger::getInstance("MsgQueue");
	HttpServerThread * This = reinterpret_cast<HttpServerThread *>(conn->server_param);
	int result = MG_FALSE;

	switch (ev) {
	case MG_POLL:{
			//LOG4CPLUS_INFO(This->log, "MG_POLL:" << conn->remote_ip << ":" << conn->remote_port);
		}
		break;
	case MG_CONNECT:{
			THttpRequest * request = reinterpret_cast<THttpRequest *>(conn->connection_param);
			if (conn->status_code != 0){
				LOG4CPLUS_WARN(This->log, "MG_CONNECT:" <<conn->remote_ip << ":" << conn->remote_port << " status:"<< conn->status_code);
			}
			else if(request){
				LOG4CPLUS_TRACE(This->log, "MG_CONNECT:" <<conn->remote_ip << ":" << conn->remote_port << " status:"<< conn->status_code);
				std::string requestData; 
				request->Encode(requestData);
				mg_write(conn,requestData.c_str(), requestData.length());
			}
			else{
				LOG4CPLUS_ERROR(This->log,"connection:" << conn << " request is null.");
			}
			result = MG_TRUE;
		}
		break;
	case MG_AUTH:{
			This->m_SocketNum++;
			helper::AutoLock autoLock(&This->server->m_AllowIpLock);
			if (This->server->m_AllowIp.find(conn->remote_ip)!= This->server->m_AllowIp.end()){
				LOG4CPLUS_TRACE(This->log, "MG_AUTH:" << conn->remote_ip << ":" << conn->remote_port );
				result = MG_TRUE;
			}else{
				LOG4CPLUS_WARN(This->log, conn->remote_ip << ":" << conn->remote_port << "IP refuse access.");
			}
		}
		break;
	case MG_REQUEST:
		{
			LOG4CPLUS_INFO(This->log, "MG_REQUEST:" << conn->remote_ip << ":" << conn->remote_port << ", content:" << std::string(conn->content,conn->content_len));
			if (strcmp(conn->uri,"/txttransfer") !=0){ 
				result = MG_FALSE;
				break;
			}
			
			THttpRequest * request = new THttpRequest(This->log);
			request->SetURI(conn->uri);
			for(int i = 0; i < conn->num_headers; i++){
				if(strcmp(conn->http_headers[i].name,"Encode") ==0 ){
					request->SetEncode(conn->http_headers[i].value);
				}
			}
			request->SetContentData(conn->content, conn->content_len);
			request->SetConnection(conn);
			conn->connection_param = request;
			This->server->m_HttpRequest.addData(request);
			LOG4CPLUS_DEBUG(msglog,"HttpRequest:" << This->server->m_HttpRequest.size());
			result = MG_MORE;
		}
		break;
	case MG_REPLY:{
			LOG4CPLUS_TRACE(This->log, "MG_REPLY:" << conn->remote_ip << ":" << conn->remote_port << " status:"<< conn->status_code
				<< " content:" << std::string(conn->content,conn->content_len));
			THttpRequest * request = reinterpret_cast<THttpRequest *>(conn->connection_param);
			//THttpResponse response(This->log);
			//response.Decode(std::string(conn->content, conn->content_len));
			request->SetResponseContent(conn->content, conn->content_len);
			//This->m_MsgQueue.addData(request);
			delete request;
			conn->connection_param = NULL;
			result = MG_FALSE;
		}
		break;
	case MG_RECV:
		{
			LOG4CPLUS_TRACE(This->log, "MG_RECV:" << conn->remote_ip << ":" << conn->remote_port << ", content:" << std::string(conn->content,conn->content_len) );
		}
		break;
	case MG_CLOSE:
		{
			if (conn->status_code == 0){
				LOG4CPLUS_TRACE(This->log, "MG_CLOSE:" <<conn->remote_ip << ":" << conn->remote_port << " status:"<< conn->status_code);
			}
			else{
				LOG4CPLUS_ERROR(This->log, "MG_CLOSE:" <<conn->remote_ip << ":" << conn->remote_port << " status:"<< conn->status_code);
			}
			This->m_SocketNum--;
			if (conn->remote_port == 0){
				delete reinterpret_cast<THttpRequest*>(conn->connection_param);
				conn->connection_param = NULL;
			}
			helper::AutoLock autoLock(THttpRequest::m_Connlock);
			THttpRequest * request = reinterpret_cast<THttpRequest*>(conn->connection_param);
			if (request)
			{
				conn->connection_param = NULL;
				request->SetConnection(NULL);
			}
			
		}
		break;
	case MG_WS_HANDSHAKE:
		{
			LOG4CPLUS_INFO(This->log, "MG_WS_HANDSHAKE:" << conn->remote_ip << ":" << conn->remote_port );
		}
		break;
	case MG_WS_CONNECT:
		{
			LOG4CPLUS_INFO(This->log, "MG_WS_CONNECT:" << conn->remote_ip << ":" << conn->remote_port );
		}
		break;
	case MG_HTTP_ERROR:
		{
			LOG4CPLUS_INFO(This->log, "MG_HTTP_ERROR:" << conn->status_code);
		}
		break;
	default:
		break;
	}

	return result;
}

HttpServerThread::HttpServerThread(HttpServer * _server):server(_server),m_mgServer(NULL),
#ifdef NS_DISABLE_SOCKETPAIR 
	m_SocketNum(1) 
#else 
	m_SocketNum(2) 
#endif
{
	std::stringstream oss;
	oss << "HttpServerThread." << this;
	this->log = log4cplus::Logger::getInstance(oss.str());
	m_mgServer = mg_create_server(this,HttpServer::mg_evhandler);
	if (m_mgServer == NULL)
	{
		LOG4CPLUS_ERROR(log,"mg_create_server failed.");
	}
	LOG4CPLUS_TRACE(log,"Construction");
}

HttpServerThread::~HttpServerThread()
{
	mg_destroy_server(&m_mgServer);
	LOG4CPLUS_TRACE(log, "Destruction");
}

void HttpServerThread::run()
{
	LOG4CPLUS_INFO(log, "Started.");
	while(this->server->m_Run){
		mg_poll_server(this->m_mgServer, 500);
		THttpRequest * request = NULL;


#ifdef WIN32
		while (this->m_SocketNum < FD_SETSIZE && this->server->m_SendMsgQueue.getData(request,0))
#else
		while (this->server->m_SendMsgQueue.getData(request,0))
#endif
		{
			struct mg_connection * client;
			request->m_ConFre++;
			if((client = mg_connect(m_mgServer, request->GetHost().c_str())) == NULL)
			{
				LOG4CPLUS_ERROR(log, "connect " << request->GetHost()<< " Error");
				if (request->m_ConFre < 10)
				{
					this->server->m_SendMsgQueue.addData(request);
				}else{
					LOG4CPLUS_ERROR(log,  request->m_ConFre << " times connect error, delete this request.");
					delete request;
				}

				break;
			}
			else{
				LOG4CPLUS_TRACE(log, "connect " << request->GetHost());
				this->m_SocketNum++;
				client->connection_param = request;
			}
		}
	}
	LOG4CPLUS_INFO(log, "Stoped.");
}

HttpServer::HttpServer():m_Run(false),m_mgServer(NULL),m_threadNum(1)
{
	this->log = log4cplus::Logger::getInstance("HttpServer");
	m_mgServer = mg_create_server(this,HttpServer::mg_evhandler);
	if (m_mgServer == NULL)
	{
		LOG4CPLUS_ERROR(log,"mg_create_server failed.");
	}
	LOG4CPLUS_TRACE(log,"Construction");
}

HttpServer::~HttpServer()
{
	StopHttpServer();
	mg_destroy_server(&m_mgServer);
	THttpRequest * requet = NULL;
	while(this->m_SendMsgQueue.getData(requet,0)){
		delete requet;
	}

	while(this->m_HttpRequest.getData(requet,0)){
		delete requet;
	}

	LOG4CPLUS_TRACE(log, "Destruction");
}




bool HttpServer::StartHttpServer(const std::string & ip, unsigned short serverport)
{
	bool result = true;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (m_mgServer == NULL){
		LOG4CPLUS_ERROR(log,"mgserver not initlization.");
		result = false;
	}
	else if (!m_Run){
		LOG4CPLUS_INFO(log,"ServerIP:" << ip << ", Port is:" << serverport);
		std::stringstream oss;
		oss << ip<< ":" << serverport;
		//mg_set_option(m_mgServer, "document_root", "."); 
		const char * err;
		err = mg_set_option(m_mgServer, "listening_port", oss.str().c_str());  // Open port 8080
		if (err){
			LOG4CPLUS_ERROR(log, err);
			result = false;
		}
		this->m_Run = true;
		for (unsigned long i =0 ; i < m_threadNum ; i++)
		{
			HttpServerThread * inst = new HttpServerThread(this);
			mg_copy_listeners(this->m_mgServer,inst->m_mgServer);
			inst->start();
			m_ThreadInstance.push_back(inst);
		}
	}
	else{
		LOG4CPLUS_WARN(log, " Already running.");
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return result;
	
}
void HttpServer::StopHttpServer()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	m_Run = false;
	for (std::list<HttpServerThread * >::const_iterator it = m_ThreadInstance.begin();
		it != m_ThreadInstance.end(); ++it)
	{
		(*it)->join();
		delete (*it);
	}
	m_ThreadInstance.clear();
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void HttpServer::SetAllowIp(const std::vector<std::string> & allowIp)
{
	helper::AutoLock autoLock(&m_AllowIpLock);
	m_AllowIp.clear();
	for (std::vector<std::string>::const_iterator it = allowIp.begin(); it != allowIp.end(); ++it)
	{
		LOG4CPLUS_INFO(log,"AllowIP:" << *it);
		m_AllowIp.insert(std::make_pair(*it,0));
	}
}

bool HttpServer::GetHttpRequest(THttpRequest *& request, unsigned long dwMilliseconeds)
{
	static log4cplus::Logger msglog = log4cplus::Logger::getInstance("MsgQueue");
	bool ret = m_HttpRequest.getData(request,dwMilliseconeds);
	if(ret) LOG4CPLUS_INFO(msglog, "HttpRequest:" << this->m_HttpRequest.size());
	return ret;
}

bool HttpServer::PutHttpSendMsg2Queue(THttpRequest * pmsg)
{
	bool ret = m_SendMsgQueue.addData(pmsg);
	LOG4CPLUS_DEBUG(pmsg->GetLog(), __FUNCTION__ << ":" << std::hex << pmsg << ", Queue.size:" << m_SendMsgQueue.size());
	return ret;
}


HttpFileServer::HttpFileServer():m_Run(false),m_mgServer(NULL)
{
	std::stringstream oss;
	oss << "HttpFileServer." << this;
	this->log = log4cplus::Logger::getInstance(oss.str());
	m_mgServer = mg_create_server(NULL,NULL);
	if (m_mgServer == NULL){
		LOG4CPLUS_ERROR(log,"mg_create_server failed.");
	}
	LOG4CPLUS_TRACE(log,"Construction");
}

HttpFileServer::~HttpFileServer()
{
	StopHttpServer();
	mg_destroy_server(&m_mgServer);
	LOG4CPLUS_TRACE(log, "Destruction");
}


void HttpFileServer::run()
{
	LOG4CPLUS_INFO(log, "Started.");
	while(m_Run){
		mg_poll_server(this->m_mgServer, 500);
	}
	LOG4CPLUS_INFO(log, "Stoped.");
}

bool HttpFileServer::StartHttpServer(const std::string & ip, unsigned short serverport,const std::string & document_root)
{
	bool result = true;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (m_mgServer == NULL){
		LOG4CPLUS_ERROR(log,"mgserver not initlization.");
		result = false;
	}
	else if (!isRunning()){
		LOG4CPLUS_INFO(log,"ServerIP:" << ip << ", Port is:" << serverport << ",document_root:" << document_root);
		std::stringstream oss;
		oss << ip<< ":" << serverport;
		mg_set_option(m_mgServer, "document_root", document_root.c_str()); 
		const char * err;
		err = mg_set_option(m_mgServer, "listening_port", oss.str().c_str());  // Open port 8080
		if (err){
			LOG4CPLUS_ERROR(log, err);
			result = false;
		}
		this->m_Run = true;
		this->start();
	}
	else{
		LOG4CPLUS_WARN(log, " Already running.");
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return result;
	
}
void HttpFileServer::StopHttpServer()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (!isRunning())
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
		return ;
	}
	m_Run = false;
	this->join();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void HttpFileServer::SetAllowIp(const std::vector<std::string> & allowIp)
{
	for (std::vector<std::string>::const_iterator it = allowIp.begin(); it != allowIp.end(); ++it)
	{
		m_AllowIp.insert(std::make_pair(*it,0));
	}
}