﻿#include "SocketCommunicator.h"

#include "Socket/message.h"
#include "MyJson/MyJson.h"
#include "Command.h"
#include "EnvConfig.h"

//tea密钥
UnInt32_t SocketCommunicator::mKey[] = {0x01, 0x02, 0x03, 0x04};

SocketCommunicator::SocketCommunicator()
{
	mSocketClient = NULL;
	mAutoConnectCount = 0;
	mTea.setKey(mKey);

	mIsConnected = false;
	mIsAutoReconnect = true;
	mHeartBeatCount = 0;
	mSendMsgCache.clear();

}

SocketCommunicator::~SocketCommunicator()
{
	clear();
}

SocketCommunicator * SocketCommunicator::getInstance()
{
	static SocketCommunicator socketCommunicator;
	return &socketCommunicator;
}

void SocketCommunicator::startSocket()
{
	printf("============startSocket============"); 
	
	mSocketClient = new SocketClient();
	mSocketClient->startSocket("127.0.0.1", 1000);

	// 消息队列主循环;
	//Director::getInstance()->getScheduler()->schedule(schedule_selector(SocketCommunicator::update), this, 0, false);
}

void SocketCommunicator::stopSocket()
{
	if (mSocketClient != nullptr)
	{
		printf("============stopSocket============");
		delete mSocketClient;
		mSocketClient = nullptr;
	}
}

void SocketCommunicator::startHeartbeat()
{
	printf("============startHeartbeat============");

	// 心跳次数重置
	mHeartBeatCount = 0;

	// 启动心跳检测;
	//Director::getInstance()->getScheduler()->schedule(schedule_selector(SocketCommunicator::heartbeat), this, HEART_BEAT_INTERVAL, false );
}

void SocketCommunicator::closeHeartbeat()
{
	printf("============closeHeartbeat============");

	// 暂停心跳检测
	//Director::getInstance()->getScheduler()->unschedule(schedule_selector(SocketCommunicator::heartbeat), this);
}

void SocketCommunicator::heartbeat(float dt)
{
	mHeartBeatCount++;

	if (mHeartBeatCount <= MAX_HEART_BEAT_REPLAY)
	{
		//log("============sendHeartbeat:%d============", mHeartBeatCount);
		this->sendMessage(nNET_CMD_HEART_BEAT, nNET_CMD_HEART_BEAT, "");
	}
	else
	{
		// 暂停心跳检测
		closeHeartbeat();

		//3次心跳超时判定失去连接，待下次发送消息时启动重连
		mIsConnected = false;
		stopSocket();
	}
}

void SocketCommunicator::reconnection()
{
	//LayerWaiting::showWaiting();

	//启动重连需要关闭超时定时器
	closeMsgTimeOut();

	mIsConnected = false;
	mIsAutoReconnect = true;

	mAutoConnectCount++;

	// 最大重连次数判定;
	if (mAutoConnectCount > MAX_AUTOREPLAYCOUNT)
	{
		//LayerWaiting::hideWaiting();

		//重置重连标记
		mAutoConnectCount = 0;
		mIsAutoReconnect = false;

		// 弹窗提示玩家重连
		//CustomPop::show("网络连接失败，请重试", CC_CALLBACK_2(SocketCommunicator::callBackReconnect, this));
		//CustomPop::show("网络连接失败，请重试", CC_CALLBACK_2(SocketCommunicator::callBackReconnect, this));
		callBackReconnect(this, RET_OK);
	}
	else
	{
		printf("============reconnection:%d============", mAutoConnectCount);

		stopSocket();
		startSocket();
	}
}

void SocketCommunicator::sendMessage( int nMainCmd,int nSubCmd, const char* szData )
{
	printf("------Send Socket Message %d:%d:%s------", nMainCmd, nSubCmd, szData);
	mTea.encrypt(szData, strlen(szData));
	const char* data = mTea.getBuffer();
	size_t dataLen = mTea.getBufferSize();
	Message* pMsg = Message::createMessage(data, dataLen, nMainCmd, nSubCmd);
	mTea.clear();

	// nMainCmd，心跳消息不添加到缓存队列中
	if (nMainCmd != nNET_CMD_HEART_BEAT && nSubCmd != nNET_CMD_HEART_BEAT)
	{
		Message* pMsgClone = pMsg->clone();
		MQ_addMessage(nMainCmd, pMsgClone);
	}

	//获取socket连接状态，并将消息添加到socket发送队列中
	SOCKET_CONNECT_STATE connectState = SOCKET_CONNECT_STATE::NOT_CONNECTED;
	if(mSocketClient)
	{
		connectState = mSocketClient->getConnectState();
		mSocketClient->pushSendMessage(pMsg);
	}
	
	//心跳消息不做处理
	if (nMainCmd != nNET_CMD_HEART_BEAT && nSubCmd != nNET_CMD_HEART_BEAT)
	{
		//根据socket连接状态，处理超时或重连
		switch (connectState)
		{
		case CONNECTED:
			//处于连接状态，则启动消息超时定时器
			startMsgTimeOut();
			break;
		case NOT_CONNECTED:
			//处于未连接状态则启动重连
			reconnection();
			break;
		case TO_CONNECT:
		case CONNECTING:
			break;
		default:
			break;
		}
	}
}

void SocketCommunicator::sendMsgCache()
{
	if (mSocketClient && !mSendMsgCache.empty())
	{
		for (auto iter = mSendMsgCache.begin(); iter != mSendMsgCache.end(); ++iter)
		{
			// 发送一个副本，否则缓存消息在发送成功后会被析构;
			mSocketClient->pushSendMessage(iter->second->clone());
		}
		//LayerWaiting::showWaiting();
		startMsgTimeOut();
	}
}

void SocketCommunicator::startMsgTimeOut()
{
	closeMsgTimeOut();
	//Director::getInstance()->getScheduler()->schedule(schedule_selector(SocketCommunicator::msgTimeOut), this, 0, 0, MSG_TIME_OUT, false);
}

void SocketCommunicator::closeMsgTimeOut()
{
	//Director::getInstance()->getScheduler()->unschedule(schedule_selector(SocketCommunicator::msgTimeOut), this);
}

void SocketCommunicator::msgTimeOut(float dt)
{
	if (!mSendMsgCache.empty())
	{
		//有缓存消息，判定超时并弹窗选择是否重发
		//LayerWaiting::hideWaiting();
	}
	closeMsgTimeOut();
}

void SocketCommunicator::update(float dt)
{
	if (!mSocketClient)
	{
		return;
	}

	Message* pMsg = mSocketClient->popReceivedMessage();

	if (pMsg)
	{
		int mainCmd = ByteBuffer::bytesToInt(pMsg->mainCmd);
		int subCmd = ByteBuffer::bytesToInt(pMsg->subCmd);

		//收到任何消息，心跳计数都清零
		mHeartBeatCount = 0;

		// 心跳数据包;
		if (mainCmd == nNET_CMD_HEART_BEAT && subCmd != nNET_CMD_HEART_BEAT)
		{
			printf("============receive heartbeat============");
			return;
		}

		// 清掉当前消息缓存;
		MQ_removeMessage(mainCmd);

		// 解包;
		mTea.decrypt(pMsg->data, ByteBuffer::bytesToInt(pMsg->length) - 4);
		char* data = mTea.getBuffer();
		int len = mTea.getBufferSize();

		//解压gzip	
		unsigned char* szOutData = nullptr;
		//ZipUtils zipUtils;
		int err = 0;// zipUtils.inflateMemoryWithHint((unsigned char *)data, len, &szOutData, 100 * len);
		//if (0 >= err)
		//{
		//	delete szOutData;
		//	delete pMsg;
		//	return;
		//}

		// 派发消息;
		printf("============receive msg:id=%d============", mainCmd);
		szOutData[err] = '\0';
		recvMessage(mainCmd, subCmd, (char *)szOutData);
		mTea.clear();

		// 收到重新登陆;
		// 此时需处理断线重连后的缓存消息重发;
		if (mainCmd == nNET_CMD_RECONNECT)
		{
			sendMsgCache();
		}

		delete szOutData;
		delete pMsg;
	}

	if (mSocketClient)
	{
		SOCKET_CONNECT_STATE connectState = mSocketClient->getConnectState();

		if (connectState == CONNECTED)
		{
			if (!mIsConnected)
			{
				printf("============net connect success============");

				//LayerWaiting::hideWaiting();

				mIsConnected = true;
				
				//连接成功后，关闭自动连接标记，并重置连接次数
				mIsAutoReconnect = false;
				mAutoConnectCount = 0;

				//启动心跳定时器
				startHeartbeat();

				//发送登录请求
				//LoginController::getInstance()->loginRole();
			}
		}
		else if (connectState == NOT_CONNECTED)
		{
			mIsConnected = false;
			closeHeartbeat();

			if (mIsAutoReconnect)
			{
				//启动重连
				reconnection();
			}
			else
			{
				//否则直接关闭连接
				stopSocket();
			}
		}
	}	
}

void SocketCommunicator::MQ_addMessage(int nCmdId, Message* pMsg)
{
	if (nullptr != pMsg)
	{
		// 干掉之前发的;
		auto iter = mSendMsgCache.find(nCmdId);
		if (iter != mSendMsgCache.end())
		{
			delete iter->second;
			mSendMsgCache.erase(iter);
		}
		mSendMsgCache.insert(std::pair<int, Message*>(nCmdId, pMsg));

		// 菊花出现;
		//LayerWaiting::showWaiting();
	}
}

void SocketCommunicator::MQ_removeMessage(int nCmdId)
{
	if (!mSendMsgCache.empty())
	{
		auto iter = mSendMsgCache.find(nCmdId);
		if (iter != mSendMsgCache.end())
		{
			delete iter->second;
			mSendMsgCache.erase(iter);
		}
	}

	if (mSendMsgCache.empty())
	{
		//没有缓存消息，说明客户端已经接收到所有的消息，可以取消超时定时器，并关闭模态菊花
		closeMsgTimeOut();
		//LayerWaiting::hideWaiting();
	}
}

void SocketCommunicator::clearMsgCache()
{
	for (auto iter = mSendMsgCache.begin(); iter != mSendMsgCache.end(); iter++)
	{
		delete iter->second;
	}
	mSendMsgCache.clear();
}

void SocketCommunicator::callBackReconnect(AbstractCommunicator* pSender, RetType type)
{
	switch (type)
	{
	case RET_OK:
	case RET_CANCEL:
	case RET_CLOSE:
		reconnection();
		break;
	default:
		break;
	}
}

void SocketCommunicator::callBackTimeOut(AbstractCommunicator* pSender, RetType type)
{
	switch (type)
	{
	case RET_OK:
		if (mIsConnected)
		{
			//处于连接状态，则重发缓存消息
			sendMsgCache();
		}
		else
		{
			//未连接状态，则直接重连，重连成功后自动发送缓存消息
			reconnection();
		}
		break;
	case RET_CANCEL:
	case RET_CLOSE:
		//取消重发则直接清除缓存消息
		clearMsgCache();
		break;
	default:
		break;
	}
}

void SocketCommunicator::clear()
{
	printf("============SocketCommunicator::clear============");

	// 终止消息主循环、心跳、超时定时器;
	//Director::getInstance()->getScheduler()->unschedule(schedule_selector(SocketCommunicator::update), this);
	closeHeartbeat();
	closeMsgTimeOut();

	// 清理socket;
	stopSocket();

	// 清空消息队列;
	clearMsgCache();

	// 清理相关状态;
	mIsConnected = false;
	mIsAutoReconnect = true;
	mAutoConnectCount = 0;
	mHeartBeatCount = 0;
}
