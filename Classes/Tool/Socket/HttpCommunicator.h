#ifndef HttpCenter_h__
#define HttpCenter_h__

#include "AbstractCommunicator.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"

const int MAX_HTTP_REQUEST_SEND_TIMES = 3;

typedef class HTTPRequestMessage
{
public:
	HTTPRequestMessage()
	{
		sendTimes = 0;
	//	pHttpRequest = NULL;
	}
	~HTTPRequestMessage()
	{
	//	if (pHttpRequest != NULL)
	//	{
	//		pHttpRequest->release();
	//	}
	}
	int sendTimes;
	//HttpRequest* pHttpRequest;
}HTTPREQUESTMESSAGE;



class HttpCommunicator : public AbstractCommunicator
{

public:

	HttpCommunicator();
	virtual ~HttpCommunicator();

public:

	//获取单实例
	static HttpCommunicator* getInstance();

	//发送Http消息
	void sendMessage(int nCmdID, const char* szData, const char* url, bool showWaiting = true);

	//Http回调相应
	//void onHttpResponse(HttpClient *sender, HttpResponse *response);


private:

	std::map<int, HTTPREQUESTMESSAGE*> m_mapRequest; 
};


#endif //HttpCenter_h__