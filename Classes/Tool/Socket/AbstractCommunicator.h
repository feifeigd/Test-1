#ifndef AbstractCommunicator_h__
#define AbstractCommunicator_h__

#include "ICommListener.h"

class AbstractCommunicator
{

public:

	AbstractCommunicator();
	virtual ~AbstractCommunicator();

public:

	//添加消息监听器
	virtual void addListener(ICommListener* pListener);

	//移除消息监听器
	virtual void removeListener(ICommListener* pListener);

	//接收消息，并向监听者下发
	virtual void recvMessage(int nMainCmd, int nSubCmd, char* szData);

	//发送消息
	virtual void sendMessage(int nMainCmd, int nSubCmd, const char* szData);

protected:

	const char*  constructErrorMessage(int nMainCmd, int nSubCmd, char* szError);

protected:

	//监听器队列
	std::vector<ICommListener*> mVecListener;

};

#endif //AbstractCommunicator_h__