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

	//��������
	HttpRequest* request = new HttpRequest();
	request->setUrl(url);
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(this, httpresponse_selector(HttpCommunicator::onHttpResponse));

	//����������
	string str(szData);
	str = "data=" + str;
	const char* postData = str.c_str();
	//�˴�strlen��ע��
	request->setRequestData(postData, strlen(postData));	  
	//�����ط���Ϣ
	HTTPREQUESTMESSAGE* pMessage = new HTTPREQUESTMESSAGE();
	pMessage->pHttpRequest = request;
	pMessage->sendTimes ++;
	request->retain();
	m_mapRequest.insert(pair<int, HTTPREQUESTMESSAGE*>(nCmdID, pMessage));
	//����MsgId
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
	
	//�����ط�
	auto iter = m_mapRequest.find(atoi(tag));
	if (iter != m_mapRequest.end())
	{
		HTTPREQUESTMESSAGE* pMessage = iter->second;
		if (!response->isSucceed())
		{ 
			MINGLOG("response failed");
			MINGLOG("error buffer: %s", response->getErrorBuffer());
			if (pMessage->sendTimes >= MAX_HTTP_REQUEST_SEND_TIMES)//�ط�3�β��ɹ�
			{
				m_mapRequest.erase(iter);
				delete pMessage;
				//���������Ϣ
				MINGLOG("net connect error");
				constructErrorMessage(nNET_CMD_NET_HTTP_ERROR, "net http error");

				//LayerWaiting::hideWaiting();
				HttpClient::destroyInstance();
			}
			else//�ط���Ϣ
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

	//�ɷ���Ϣ
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

