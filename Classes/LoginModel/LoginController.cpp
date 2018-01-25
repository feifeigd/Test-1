﻿#include "LoginController.h"
#include "LoginModel.h"

#include "GameModule.h"
#include "helper/BufferTool.h"
#include "server/Protocol.h"
#include "GameNetworkNode.h"

#include "log/LogManager.h"
#include "log/LogFileHandler.h"
#include "log/LogConsoleHandler.h"

LoginController* LoginController::ms_pInstance = nullptr;

LoginController::LoginController()
{
	GameNetworkNode::getInstance()->addListener(this);
	KX_LOGDEBUG("++++++++++++ logincontroller created ++++++++++++");
}

LoginController::~LoginController()
{
	GameNetworkNode::getInstance()->removeListener(this);
	KX_LOGDEBUG("++++++++++++ logincontroller destroy ++++++++++++");
}

LoginController* LoginController::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new LoginController();
	}
	return ms_pInstance;
}

bool LoginController::onServerInit()
{
	LogManager::getInstance()->setShowTime(true);
	LogManager::getInstance()->setShowDate(true);
	LogManager::getInstance()->addHandler(1, new LogConsoleHandler());
	LogFileHandler* pFileHandle = new LogFileHandler();
	pFileHandle->setFilePath("../bin/");
	pFileHandle->setFileName("TestClientOut");
	pFileHandle->setFastModel(false);
	LogManager::getInstance()->addHandler(2, pFileHandle);

	m_ServerCon = new ConnectServerConf();
	m_ServerCon->serverGroup = 1;
	m_ServerCon->host = "127.0.0.1";//"103.44.145.245"; //"a635459675.55555.io";// 
	m_ServerCon->port = 8888;// 25173;

#if KX_PLATFORM_WIN32 != KX_TARGET_PLATFORM
	// 域名解析成ip
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct hostent *host = gethostbyname(m_ServerCon->host.c_str());
	if (!host)
	{
		puts("Get IP address error!");
		system("pause");
		exit(0);
	}
	//别名
	for (int i = 0; host->h_aliases[i]; i++)
	{
		printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
	}
	//地址类型
	printf("Address type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
	//IP地址
	for (int i = 0; host->h_addr_list[i]; i++)
	{
		printf("IP addr %d: %s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
#endif

	if (GameNetworkNode::getInstance()->connectToServer((char *)m_ServerCon->host.c_str(), m_ServerCon->port))
	{
		KX_LOGDEBUG("ClientTest Connect to IP=%s Port=%d successful", m_ServerCon->host.c_str(), m_ServerCon->port);
	}

	return true;
}

bool LoginController::login()
{
	BufferData* buffer = newBufferData(MAIN_CMD::CMD_LOGIN_SERVER, LOGIN_CMD::CMD_C2S_LOGIN);

	LOGIN_DATA da = LOGIN_DATA();
	da.accountId = 123456;
	buffer->writeData(da);

	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer->getDataLength();
	head->uid = 0;//服务器用ID
	bool isSuccessful = false;
	if (GameNetworkNode::getInstance()->sendData(buffer->getBuffer(), buffer->getDataLength()) > 0)
	{
		KX_LOGDEBUG("CGameNetworkNode::sendData to server successful!");
		isSuccessful = true;
	}
	deleteBufferData(buffer);
	return isSuccessful;
}


void LoginController::onRecvMessage(int nMainCmd, int nSubCmd, char* szData)
{
	if (nMainCmd != MAIN_CMD::CMD_LOGIN_SERVER)
	{
		KX_LOGDEBUG("mainCmd !=  MAIN_CMD::CMD_LOGIN_SERVER");
		return;
	}
	switch (nMainCmd)
	{
		case LOGIN_CMD::CMD_S2C_LOGIN:
		{
			LoginModel::getInstance()->processLogin(szData);

			startHeartbeat();
		}
		break;
		default:
			break;
	}
}

void LoginController::startHeartbeat()
{
	GameNetworkNode::getInstance()->startHeartbeat();
}
