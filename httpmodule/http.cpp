#include "http.h"
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <log4cplus/loggingmacros.h>
#include "../common/stringHelper.h"
#include "mongoose.h"



/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/



#ifndef WIN32
#define ASSERT assert
#define strnicmp  strncasecmp
#define stricmp   strcasecmp
#endif

THttp::THttp(const log4cplus::Logger parentLog):m_strContentType("text/plain")
{
}

THttp::~THttp()
{
}

const std::string & THttp::GetContentType()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log," get ContentType=" << m_strContentType);
#endif
    return m_strContentType; 
}

const std::string & THttp::GetContentData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get ContentData=" << m_strContentData);
#endif
    return m_strContentData;
}

const std::string & THttp::GetAgentData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get AgentData=" << m_strAgentData);
#endif
    return m_strAgentData;
}

const std::string & THttp::GetUserAgentData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get UserAgentData=" << m_strUserAgentData);
#endif
    return m_strUserAgentData;
}
const std::string & THttp::GetServerData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "get ServerData=" << m_strServerData);
#endif
	return m_strServerData;
}
const std::string & THttp::GetHost()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "get Host=" << m_strHost);
#endif
	return m_strHost;
}
const std::string & THttp::GetAccept()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "get Accept=" << m_strAccept);
#endif
	return m_strAccept;
}
const std::string & THttp::GetAuthenticateData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get AuthenticateData=" << m_strAuthenticateData);
#endif
	return m_strAuthenticateData;
}
const std::string & THttp::GetAuthorizationData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get AuthorizationData=" << m_strAuthorizationData);
#endif
    return m_strAuthorizationData;
}

const std::string & THttp::GetMobileNumData()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get MobileNumData=" << m_strMobileNumData);
#endif
    return m_strMobileNumData;
}

unsigned long THttp::GetContentLength()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get ContentLength=" << m_strContentData.length());
#endif
    return m_strContentData.length();
}

const std::string & THttp::GetEncode()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get Encode=" << m_strEncode);
#endif
	return m_strEncode;
}

void  THttp::SetContentType(const std::string & contentType)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set ContentType=" << contentType);
#endif
    m_strContentType = contentType;
    helper::string::trim(m_strContentType);
}

void  THttp::SetContentData(const std::string & data)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set ContentData=" << data );
#endif
    m_strContentData = data;
}

void THttp::SetContentData(const char * data, unsigned long len)
{
	return SetContentData(std::string(data,len));
}


void  THttp::SetAgentData(const std::string & strAgentData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set AgentData=" << strAgentData);
#endif
	m_strAgentData = strAgentData;
	helper::string::trim(m_strAgentData);
}


void  THttp::SetUserAgentData(const std::string & strUserAgentData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set UserAgentData=" << strUserAgentData);	
#endif
    m_strUserAgentData = strUserAgentData;
	helper::string::trim(m_strUserAgentData);
}

void  THttp::SetServerData(const std::string & strServerData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set ServerData=" << strServerData);
#endif
    m_strServerData = strServerData;
	helper::string::trim(m_strServerData);
}


void  THttp::SetHost(const std::string & strHost)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set Host=" << strHost);
#endif
    m_strHost = strHost;
    helper::string::trim(m_strHost);
}


void  THttp::SetAuthenticateData(const std::string & strAuthenticateData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set strAuthenticateData=" << strAuthenticateData);
#endif
    m_strAuthenticateData = strAuthenticateData;
	helper::string::trim(m_strAuthenticateData);
}


void  THttp::SetAuthorizationData(const std::string & strAuthorizaionData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set AutoorizationData=" << strAuthorizaionData);
#endif
    m_strAuthorizationData = strAuthorizaionData;
    helper::string::trim(m_strAuthorizationData); 
}


void  THttp::SetMobileNumData(const std::string & strMobileNumData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set MobileNumData=" << strMobileNumData);
#endif
    m_strMobileNumData = strMobileNumData;
	helper::string::trim(m_strMobileNumData);
}

void THttp::SetEncode(const std::string & strEncode)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set Encode=" << strEncode);
#endif
	m_strEncode = strEncode;
	helper::string::trim(m_strEncode);
}

void THttp::SetAccept(const std::string & strAccept)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set Accept=" << strAccept);
#endif
    m_strAccept = strAccept;
    helper::string::trim(m_strAccept);
}

void THttp::AppendHeader(std::string & outputData, const std::string & name, const std::string &value)const
{
	outputData.append(name);
	outputData.append(value);
	outputData.append("\r\n");
}
int THttp::EncodeMessage(std::string & outputData) const
{
    //set date
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"EncodeMessage.");
#endif
    char strDate[64]={0};
    time_t lt = time(NULL);
    strftime(strDate, 64, "%a %b  %d %X %Y" ,localtime(&lt));

    AppendHeader(outputData,"Date:",strDate);

	if(!m_strAccept.empty()) {
		AppendHeader(outputData, "Accept:", m_strAccept);
    }

	if(!m_strAuthenticateData.empty()){
		AppendHeader(outputData, "WWW-Authenticate:", m_strAuthenticateData);
    }

	if(!m_strAuthorizationData.empty()) {
		AppendHeader(outputData, "Authorization:", m_strAuthorizationData);
	}

	if(!m_strHost.empty()){
		AppendHeader(outputData, "Host:", m_strHost);
    }

	if(!m_strAgentData.empty()){
		AppendHeader(outputData, "Agent:", m_strAgentData);
    }

	if(!m_strServerData.empty()){
		AppendHeader(outputData, "Server:", m_strServerData);
    }

    //set connection
	AppendHeader(outputData, "Connection:", "close");

    //set content type
    if(!m_strContentData.empty())
    {
        if(!m_strContentType.empty()){
			AppendHeader(outputData, "Content-Type:", m_strContentType);
        }
        std::stringstream oss;
		oss << m_strContentData.length();
        AppendHeader(outputData,"Content-Length:", oss.str());
    }
    else
    {
        AppendHeader(outputData,"Content-Length:","0");
    }
	AppendHeader(outputData, "", "");

    
    if(!m_strContentData.empty())
    {
		outputData.append(m_strContentData);
    }
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "Encode result:" << outputData);
#endif
    return 0;
}
int THttp::DecodeMessage(const std::string & inputData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"DecodeMessage:" << inputData);
#endif
    std::string::size_type pos =inputData.find("\r\n\r\n");
    if(pos == std::string::npos){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log, inputData << ", not find \\r\\n\\r\\n");
#endif
        return -1;
	}
    pos += 4;
	SetContentData(inputData.substr(pos));
 
    char header[1024];
    int  count=0;
    for(std::string::size_type i=0;i<inputData.length();i++)
    {
        if(inputData[i] == '\r')
        {
            if(inputData.compare(i,2,"\r\n")==0)
            {
                header[count]=0;
            
                if(strnicmp(header,"content-type:",13)==0)
                {
                    SetContentType(header+13);
                }
				else if(strnicmp(header,"Agent:",6)==0)
                {
                    SetAgentData(header+6);
                }
				else if(strnicmp(header,"User-Agent:",11)==0)
                {
                    SetUserAgentData(header+11);
                }
				else if(strnicmp(header,"Authorization:",14)==0)
                {
                    SetAuthorizationData(header+14);
                }
				else if(strnicmp(header,"X-Up-Calling-Line-ID:",21)==0)
                {
                    SetMobileNumData(header+21);
                }
                else if(strnicmp(header,"content-length:",15)==0)
                {
                }
            }
            if(inputData.compare(i, 4, "\r\n\r\n")!=0)
            {
                count=0;
                i+=1;
                continue;
            }
            break;
        }
        header[count++]=inputData[i];
    }

    // printf("the len = [%d]\n", m_nContentLen);
//    if(*body)

    return 0;
}

helper::CLock * THttpRequest::m_Connlock = NULL;

void THttpRequest::CreateConnectLock(){
	if (THttpRequest::m_Connlock == NULL){
		THttpRequest::m_Connlock = new helper::CLock();
	}
}

void THttpRequest::DeleteConnectLock()
{
	if (THttpRequest::m_Connlock){
		delete THttpRequest::m_Connlock ;
		THttpRequest::m_Connlock = NULL;
	}
}
THttpRequest::THttpRequest(const log4cplus::Logger parentLog):THttp(parentLog),m_nMethod(HTTP_METHOD_UNKNOWN)
	,m_ResponseData(NULL),m_HttpConn(NULL),m_ConFre(0)
{
	std::stringstream oss;
	oss << parentLog.getName() << ".THttpRequest";
	this->log = log4cplus::Logger::getInstance(oss.str());
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "Construction");
#endif
}

THttpRequest::~THttpRequest()
{
	helper::AutoLock autoLock(m_Connlock);
	mg_connection * conn = reinterpret_cast<mg_connection *>(this->m_HttpConn);
	if (conn){
		conn->connection_param = NULL;
	}
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "Destruction");
#endif
}

void  THttpRequest::SetMethod(HTTPMETHOD method)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set Method=" << method);
#endif
    assert(method == HTTP_METHOD_GET || method == HTTP_METHOD_PUT || method==HTTP_METHOD_DELETE|| method==HTTP_METHOD_POST);
    m_nMethod=method;
}

void  THttpRequest::SetURI(const std::string & uri)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set URI=" << uri);
#endif
	m_strURI = uri;
}

void THttpRequest::SetURI(const char * uri, unsigned long len)
{
	return SetURI(std::string(uri,len));
}

void THttpRequest::SetResponseContent(const char * content , unsigned long len)
{
	return SetResponseContent(std::string(content,len));
}

void THttpRequest::SetResponseContent(const std::string & content)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set ResponseContent=" << content);
#endif
	m_strResponse = content;
}

int  THttpRequest::Response(const THttpResponse & response)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "response:" << &response);
#endif
	m_ResponseData = &response;
	return DoRespose();
}

void THttpRequest::SetConnection(void * conn)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log, "set Connectionnse:" << conn);
#endif
	this->m_HttpConn = conn;
}
int THttpRequest::DoRespose()
{
	helper::AutoLock autoLock(m_Connlock);
	mg_connection * conn = reinterpret_cast<mg_connection *>(this->m_HttpConn);
	size_t len = 0;
	if (conn)
	{
		std::string response;
		this->m_ResponseData->Encode(response);
#ifdef HTTPLOG
		LOG4CPLUS_INFO(log,"Send response data:" << response);
#endif
		len = mg_write(conn,response.c_str(),response.length());
		return 0;
	}
	return -1;
}
const std::string & THttpRequest::GetResponse()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get Response=" << &m_ResponseData);
#endif
	return m_strResponse;
}
HTTPMETHOD  THttpRequest::GetMethod()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get Mehthod=" <<m_nMethod);
#endif
    return m_nMethod;
}

const std::string & THttpRequest::GetURI()const 
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get URI=" << m_strURI);
#endif
    return m_strURI;
}

int  THttpRequest::Encode(std::string & outputData)const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"Encode.");
#endif
    static const std::string  MethodName[4]={"GET","PUT","DELETE","POST"};

    if(m_nMethod == HTTP_METHOD_UNKNOWN){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log, "HTTP_METHOD_UNKNOWN");
#endif
		return -1;
	}

    if(m_strURI.empty()){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log, "empty url");
#endif
        return -2;
	}

    //set request line
   outputData.append(MethodName[m_nMethod]);
   outputData.append(" ");
   outputData.append(m_strURI);
   outputData.append(" HTTP/1.1\r\n");

    //set body
    EncodeMessage(outputData);

    return 0;
}

int  THttpRequest::Decode(const std::string & inputData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"Decode.");
#endif

    HTTPMETHOD    method;
    char   uri[512]="";

    char strType[256]="";
    char strVersion[256]="";
    sscanf(inputData.c_str(),"%255s %511s %255s",strType,uri,strVersion);
    if(strcmp(strType,"GET") ==0)
    {
        method=HTTP_METHOD_GET;
    }
    else if(strcmp(strType,"PUT") ==0)
    {
        method=HTTP_METHOD_PUT;
    }
    else if(strcmp(strType,"DELETE") ==0)
    {
        method=HTTP_METHOD_DELETE;
    }
	else if(strcmp(strType,"POST") ==0)
    {
        method=HTTP_METHOD_POST;
    }
    else
    {
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log, "Unknown HTTP METHOD:" << strType);
#endif
        return -3;
    }

//     if(strcmp(strVersion,"HTTP/1.1") !=0)  //手机发出的消息，经过WAP网关后，HTTP消息的版本号有时候会变为1.0
//         return -4;
   
 
    if(DecodeMessage(inputData) !=0){
        return -5;
	}

    SetMethod(method);
    SetURI(uri);
    return 0;
}


struct TResponseStatus
{
    int  code;
    const char * desc;
};

const TResponseStatus responseStats[] = \
{
  /* Informational 1xx */
  {100, "Continue"},
  {101, "Switching Protocols"},

  /* Successful 2xx */
  {200, "OK"},
  {201, "Created"},
  {202, "Accepted"},
  {203, "Non-Authoritative Information"},
  {204, "No Content"},
  {205, "Reset Content"},
  {206, "Partial Content"},

  /* Redirection 3xx */
  {300, "Multiple Choices"},
  {301, "Moved Permanently"},
  {302, "Found"},
  {303, "See Other"},
  {304, "Not Modified"},
  {305, "Use Proxy"},
  // 306 Unused
  {307, "Temporary Redirect"},
 
  /* Client Error 4xx */
  {400, "Bad Request"},
  {401, "Unauthorized"},
  {402, "Payment Required"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},
  {406, "Not Acceptable"},
  {407, "Proxy Authentication Required"},
  {408, "Request Timeout"},
  {409, "Conflict"},
  {410, "Gone"},
  {411, "Length Required"},
  {412, "Precondition Failed"},
  {413, "Request Entity Too Large"},
  {414, "Request-URI Too Long"},
  {415, "Unsupported Media Type"},
  {416, "Requested Range Not Satisfiable"},
  {417, "Expectation Failed"},

  /* Server Error 5xx */
  {500, "Internal Server Error"},
 // {501, "Not Implemented"},
  {501, "Billing Failed"},
  {502, "Bad Gateway"},
  {503, "Service Unavailable"},
  {504, "Gateway Timeout"},
  {505, "HTTP Version Not Supported"},
  {511, "Illegal User Status"},
  {512, "Exceed Max Downloads Limit"},
  {513, "Exceed Max Billing Money this Period"}
};

const char * getStatusDescription(int code)
{
    for (unsigned int i=0; i < sizeof(responseStats) / sizeof(responseStats[0]); i++)
    {
        if (responseStats[i].code == code)
        {
            return responseStats[i].desc; 
        }
    }
    return NULL;
}

THttpResponse::THttpResponse(const log4cplus::Logger parentLog):THttp(parentLog),m_nStatusCode(200)
{
#ifdef HTTPLOG
	std::stringstream oss;
	oss << parentLog.getName() << ".THttpResponse." << this;
	this->log = log4cplus::Logger::getInstance(oss.str());
	LOG4CPLUS_TRACE(log, "Construction");
#endif
}

THttpResponse::~THttpResponse()
{
#ifdef HTTPLOG
    LOG4CPLUS_TRACE(log, "Destruction");
#endif
}

int  THttpResponse::Encode(std::string & outputData)const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"Encode.");
#endif
    const char * desc=getStatusDescription(m_nStatusCode);
    if(desc ==NULL){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log, "StatusCode:" << m_nStatusCode);
#endif
        return -1;
	}
	std::stringstream oss;
	oss << "HTTP/1.1 " << m_nStatusCode << " " << desc << "\r\n";
	outputData.append(oss.str());
	
    EncodeMessage(outputData);
    return 0;
}

int  THttpResponse::Decode(const std::string & inputData)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"Decode：" << inputData);
#endif
    if(inputData.empty()){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log,"empty string.");
#endif
        return -1;
	}
    
    char strVersion[256]={0};
    int  statusCode=0;
    char strDesc[256]={0};
	sscanf(inputData.c_str(),"%255s %4d %255s\r\n",strVersion,&statusCode,strDesc);
    
//     if(strcmp(strVersion,"HTTP/1.1") !=0)   //手机发出的消息，经过WAP网关后，HTTP消息的版本号有时候会变为1.0
//         return -2;

    const char * t=getStatusDescription(statusCode);
    if(t ==NULL){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log, "unknown statusCode:" << statusCode);
#endif
        return -3;
	}

//     if(strcmp(strDesc,t)!=0)
//         return -4;

    if(inputData.find("\r\n\r\n") == std::string::npos){
#ifdef HTTPLOG
		LOG4CPLUS_ERROR(log,"not find \\r\\n\\r\\n");
#endif
        return -5;
	}

    if(DecodeMessage(inputData) !=0){
        return -6;
	}

    SetStatusCode(statusCode);
    return 0;
}

void THttpResponse::SetStatusCode(int statusCode)
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"set StatusCode=" << statusCode);
#endif
    m_nStatusCode=statusCode;
}
int THttpResponse::GetStatusCode()const
{
#ifdef HTTPLOG
	LOG4CPLUS_TRACE(log,"get StatusCode=" << m_nStatusCode);
#endif
    return m_nStatusCode;
}
