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

void LoginModel::processLogin(int nMainCmd, int nSubCmd, char* szData)
{
	Head* head = reinterpret_cast<Head*>(szData);

	LOGIN_DATA* mData = reinterpret_cast<LOGIN_DATA*>(head->data());
	
	ObserverParam param;
	param.id = LOGIN_MODEL::CMD_LOGIN;
	param.self = this,
	param.updateData = (void*)mData;
	
	this->notifyObservers((void*)&param);
}
