#ifndef _HTTP_H_
#define _HTTP_H_

#include <string>
#include <log4cplus/logger.h>
#include "lock.h"


typedef enum{
	HTTP_METHOD_UNKNOWN = -1,
	HTTP_METHOD_GET   =   0,
	HTTP_METHOD_PUT    =  1,
	HTTP_METHOD_DELETE  = 2,
	HTTP_METHOD_POST    = 3,
}HTTPMETHOD;

class THttp
{
protected:
    THttp(const log4cplus::Logger parentLog);
    virtual ~THttp();
public:    
	void  SetContentType(const std::string & contentType);
	void  SetContentData(const std::string & data);
	void  SetContentData(const char * data, unsigned long len);
	void  SetAgentData(const std::string & strAgentData);
	void  SetUserAgentData(const std::string & strUserAgentData);
	void  SetServerData(const std::string & strServerData);
	void  SetHost(const std::string & strHost);
	void  SetAccept(const std::string & strAccept);
	void  SetAuthenticateData(const std::string & strAuthenticate);
	void  SetAuthorizationData(const std::string & strAuthorization);
	void  SetMobileNumData(const std::string & strMobileNum);
	void  SetEncode(const std::string & strEncode);

	const std::string & GetContentType()const;
	const std::string & GetContentData()const;
	const std::string & GetAgentData()const;
	const std::string & GetUserAgentData()const;
	const std::string & GetServerData()const;
	const std::string & GetHost()const;
	const std::string & GetAccept()const;
	const std::string & GetAuthenticateData()const;
	const std::string & GetAuthorizationData()const;
	const std::string & GetMobileNumData()const;
	unsigned long	  GetContentLength()const;
	const std::string & GetEncode()const;

protected:
	int   EncodeMessage(std::string & outputData)const;
	void  AppendHeader(std::string & outputData, const std::string & name, const std::string &value)const;
	int   DecodeMessage(const std::string & inputData);

	std::string m_strContentType;
	std::string m_strContentData;
	std::string m_strAgentData;
	std::string m_strUserAgentData;
	std::string m_strServerData;
	std::string m_strHost;
	std::string m_strAuthenticateData;
	std::string m_strAuthorizationData;
	std::string m_strMobileNumData;
	std::string m_strAccept;
	std::string m_strEncode;
	log4cplus::Logger log;

};
class THttpResponse;
class THttpRequest : public THttp
{
public:
    THttpRequest(const log4cplus::Logger parentLog);
    virtual ~THttpRequest();

    void  SetMethod(HTTPMETHOD method);
    void  SetURI(const std::string & uri);
	void  SetURI(const char * uri , unsigned long len);
	void  SetConnection(void * conn);
	void  SetResponseContent(const char * content , unsigned long len);
	void  SetResponseContent(const std::string & content);
	int  Response(const THttpResponse & response);
    
    HTTPMETHOD   GetMethod() const;
    const std::string & GetURI()const;
	const std::string & GetResponse()const;

    int   Encode(std::string & outputData) const;
    int   Decode(const std::string & inputData);
	const log4cplus::Logger & GetLog(){ return log;};
	int m_ConFre;
private:
    HTTPMETHOD   m_nMethod;
    std::string m_strURI;
	std::string m_strResponse;
	const THttpResponse * m_ResponseData;
protected:
	virtual int DoRespose();
	void * m_HttpConn;
	static helper::CLock *m_Connlock;
	friend class HttpServer;
public:
	static void CreateConnectLock();
	static void DeleteConnectLock();
};

class THttpResponse : public THttp
{
public:
    THttpResponse(const log4cplus::Logger parentLog);
    virtual ~THttpResponse();

    void  SetStatusCode(int statusCode);
    int   GetStatusCode() const;

    int   Encode(std::string & outputData)const;
    int   Decode(const std::string & inputData);
private:
    int   m_nStatusCode;
};
#endif

