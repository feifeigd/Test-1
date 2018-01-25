#include "HttpCommunicator.h"
#include "network/HttpRequest.h"
#include "MyJson/MyJson.h"
#include "Command.h"
#include "EnvConfig.h"
#include <ostream>
#include <cmath>
#include <cctype>



HttpCommunicator::HttpCommunicator()
{

}

HttpCommunicator::~HttpCommunicator()
{
	//HttpClient::destroyInstance();
}

HttpCommunicator* HttpCommunicator::getInstance()
{
	static HttpCommunicator httpCommunicator;
	return &httpCommunicator;
}
/*

void HttpCommunicator::sendMessage(int nCmdID, const char* szData, const char* url, bool showWaiting)
{
	if (showWaiting)
	{
		//LayerWaiting::showWaiting();
	}

	//建立连接
	HttpRequest* request = new HttpRequest();
	request->setUrl(url);
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(this, httpresponse_selector(HttpCommunicator::onHttpResponse));

	//待发送数据
	string str(szData);
	str = "data=" + str;
	const char* postData = str.c_str();
	//此处strlen需注意
	request->setRequestData(postData, strlen(postData));	  
	//缓存重发消息
	HTTPREQUESTMESSAGE* pMessage = new HTTPREQUESTMESSAGE();
	pMessage->pHttpRequest = request;
	pMessage->sendTimes ++;
	request->retain();
	m_mapRequest.insert(pair<int, HTTPREQUESTMESSAGE*>(nCmdID, pMessage));
	//设置MsgId
	ostringstream oss;
	oss << nCmdID;
	request->setTag(oss.str().c_str());
	HttpClient::getInstance()->send(request);
	MINGLOG("sendHttpMessage: nCmdId=%d, dataLen=%d, data=%s", nCmdID, strlen(szData), szData);
	request->release();
}

void HttpCommunicator::onHttpResponse(HttpClient *sender, HttpResponse *response) 
{ 
	if (!response) return;

	const char* tag = response->getHttpRequest()->getTag();
	MINGLOG("%s completed", tag);

	int statusCode = response->getResponseCode();
	MINGLOG("HttpResponse code: %d", statusCode);
	
	//处理重发
	auto iter = m_mapRequest.find(atoi(tag));
	if (iter != m_mapRequest.end())
	{
		HTTPREQUESTMESSAGE* pMessage = iter->second;
		if (!response->isSucceed())
		{ 
			MINGLOG("response failed");
			MINGLOG("error buffer: %s", response->getErrorBuffer());
			if (pMessage->sendTimes >= MAX_HTTP_REQUEST_SEND_TIMES)//重发3次不成功
			{
				m_mapRequest.erase(iter);
				delete pMessage;
				//构造错误消息
				MINGLOG("net connect error");
				constructErrorMessage(nNET_CMD_NET_HTTP_ERROR, "net http error");

				//LayerWaiting::hideWaiting();
				HttpClient::destroyInstance();
			}
			else//重发消息
			{
				pMessage->sendTimes ++;
				HttpClient::getInstance()->send(pMessage->pHttpRequest);
				MINGLOG("sendHttpMessage: nCmdId=%d, sendTimes=%d", iter->first, pMessage->sendTimes);
				//pMessage->pHttpRequest->release();
			}
			return; 
		} 
		m_mapRequest.erase(iter);
		delete pMessage;		
	}

	//派发消息
	std::vector<char> *buffer = response->getResponseData();
	string str = "";
	for (size_t i = 0; i < buffer->size(); i++)
	{
		str += buffer->at(i);
	}

	//LayerWaiting::hideWaiting();
	
	recvMessage(atoi(tag), str.c_str());

	//HttpClient::destroyInstance();

	
} 
*/

