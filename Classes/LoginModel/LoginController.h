#ifndef LoginController_h__
#define LoginController_h__

#include "Socket/ICommListener.h"
#include "KXServer.h"

// 连接服务器配置
struct ConnectServerConf
{
	int serverGroup;
	int port;
	std::string host;
};

class LoginController : public ICommListener
{
public:

	LoginController();
	~LoginController();

	static LoginController* getInstance();
	static void destroy();

	//服务器模块初始化
	virtual bool onServerInit();

	bool login();

	//启动心跳
	void startHeartbeat();

	//消息接收接口
	virtual void onRecvMessage(int nMainCmd, int nSubCmd, char* szData);

private:

	//单实例
	static LoginController* ms_pInstance;
	ConnectServerConf*  m_ServerCon;
};

#endif // LoginController_h__
