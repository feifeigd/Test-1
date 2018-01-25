#include "LoginModel.h"
#include "server/Protocol.h"
#include "server/Head.h"

LoginModel* LoginModel::m_pInstance = nullptr;

LoginModel::LoginModel(void)
{
}

LoginModel::~LoginModel(void)
{
}

LoginModel* LoginModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new LoginModel;
	}
	return m_pInstance;
}

void LoginModel::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void LoginModel::updateNotify()
{
	
	// 更新至UI;
	// 只提供消息通知，具体内容由UI主动来取;
	//ObserverParam param;
	//param.id = OBS_PARAM_TYPE_SERVER_ZONE_LIST;
	//int nMaxCount = m_vcServerList.rbegin()->m_nZoneIndex;
	//param.updateData = (void*)(&nMaxCount);
	//this->notifyObservers((void*)&param);
}

void LoginModel::processLogin( char* szData)
{
	Head* head = reinterpret_cast<Head*>(szData);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int id = head->uid;

	LOGIN_DATA* mData = reinterpret_cast<LOGIN_DATA*>(head->data());
}
