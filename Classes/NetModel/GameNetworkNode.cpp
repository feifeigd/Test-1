#include "GameNetworkNode.h"
#include "GameModule.h"
#include <string.h>
#include "helper/BufferTool.h"
#include "server/Protocol.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

GameNetworkNode::GameNetworkNode(void)
: m_bUserClose(false)
, m_bEnabledHb(false)
, m_fMaxHbTime(10.0f)
, m_fTickNet(0.0f)
, m_pPoller(NULL)
, m_pGameModule(NULL)
{
}


GameNetworkNode* GameNetworkNode::create()
{ 
	GameNetworkNode *pRet = new(std::nothrow) GameNetworkNode();
	if (pRet && pRet->init())
	{
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

GameNetworkNode::~GameNetworkNode(void)
{
    KXSAFE_RELEASE(m_pPoller);
}

GameNetworkNode *GameNetworkNode::m_pInstance = NULL;

GameNetworkNode *GameNetworkNode::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new GameNetworkNode;
        if (!m_pInstance->init())
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    return m_pInstance;
}

void GameNetworkNode::destroy()
{
    if (NULL != m_pInstance)
    {
        m_pInstance->release();
    }
}

bool GameNetworkNode::init()
{
    m_pPoller = new KxSelectPoller();
	m_pGameModule = new GameModule();
    return true;
}

void GameNetworkNode::onEnter()
{
	Node::onEnter();
}

void GameNetworkNode::onExit()
{
	Node::onExit();
}

void GameNetworkNode::update(float dt)
{
    if (m_bEnabledHb)
    {
        // 心跳发送, 如果n秒内没有数据交互, 这时才发送心跳包
        m_fTickNet += dt;
        if (m_fTickNet >= m_fMaxHbTime)
        {
			m_fTickNet = 0;
            heartbeatScheduler(dt);
        }
    }
}

void GameNetworkNode::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
    Node::visit(renderer, parentTransform, parentFlags);
   if (NULL != m_pPoller)
   {
       // 轮询网络
       m_pPoller->poll();
   }
}

bool GameNetworkNode::connectToServer(const char *ip, int port, EServerConnType connType, 
	KXSOCK_VERSION version /*= KXV_IPV4*/, std::function<void(bool)> callBack /*= nullptr*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter != m_mapServerConns.end())
    {
        return false;
    }

    KxTCPConnector *pConnector = new KxTCPConnector();
	if (!pConnector->init(version))
    {
        KX_LOGDEBUG("error: m_pConnector init faile %d", pConnector->getCommId());
		pConnector->release();
        return false;
    }

	if (SERVER_CONN_SESSION == connType)
	{
		pConnector->setModule(m_pGameModule);
	}

    ServerConn conn;
    conn.SockVersion = version;
    conn.Connector = pConnector;
    conn.ConnectCallback = callBack;
    conn.ConnIP = ip;
    conn.Port = port;

    m_mapServerConns[connType] = conn;

    if (!pConnector->isConnecting()&& !pConnector->connect(ip, port))
    {
		KX_LOGDEBUG("error: m_pConnector connect faile %d", pConnector->getCommId());
		pConnector->release();
        return false;
    }

    m_pPoller->addCommObject(pConnector, pConnector->getPollType());
    
    m_bUserClose = false;
    return true;
}

int GameNetworkNode::sendData(char* buffer, unsigned int len, EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
    KxTCPConnector*pConnector1 = getConnector(connType);
	if (nullptr != pConnector1 && !pConnector1->isConnecting())
    {
		return pConnector1->sendData(buffer, len);
    }
    return -1;
}

bool GameNetworkNode::reconnectToServer(EServerConnType connType, std::function<void(bool)> callBack /*= nullptr*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter == m_mapServerConns.end())
    {
        return false;
    }

    iter->second.ConnectCallback = callBack;
    KxTCPConnector *pConnector = iter->second.Connector;
    if (nullptr != pConnector)
    {
        iter->second.ConnectingTimes += 1;
        // 正在重连中，别催我
        if (pConnector->isConnecting() && iter->second.ConnectingTimes < 8)
        {
			KX_LOGDEBUG("reconnectToServer But Connector isConnecting");
            return true;
        }
    }
    iter->second.ConnectingTimes = 0;

	KxTCPConnector* pNewConnector = new KxTCPConnector();
    if (!pNewConnector->init(iter->second.SockVersion))
    {
		pConnector->release();
        return false;
    }

	if (SERVER_CONN_SESSION == connType)
	{
		pNewConnector->setModule(m_pGameModule);
	}

    // 先移除,再赋值
    if (nullptr != pConnector)
    {
        if (pConnector->getPoller() != nullptr)
        {
            pConnector->getPoller()->removeCommObject(pConnector);
        }
        pConnector->close();
		pConnector->release();
    }

    if (!pNewConnector->connect(iter->second.ConnIP.c_str(), iter->second.Port))
    {
		pConnector->release();
        return false;
    }

    m_pPoller->addCommObject(pNewConnector, pNewConnector->getPollType());
    iter->second.Connector = pNewConnector;

    m_bUserClose = false;
	return true;
}

void GameNetworkNode::setIsHeartbeat(bool bEnabled) 
{
    m_bEnabledHb = bEnabled; 
}

void GameNetworkNode::setHeartbeatTime(float fHbTime)
{
    m_fMaxHbTime = fHbTime;
}

void GameNetworkNode::closeConnect()
{
    m_bUserClose = true;
    map<int, ServerConn>::iterator iter = m_mapServerConns.begin();
    for (; iter != m_mapServerConns.end(); ++iter)
    {
        KxTCPConnector* pConnector = iter->second.Connector;
        if (nullptr != pConnector)
        {
            if (pConnector->getPoller() != nullptr)
            {
                pConnector->getPoller()->removeCommObject(pConnector);
            }
            pConnector->close();
			pConnector->release();
        }
    }

    m_mapServerConns.clear();
}

KxTCPConnector *GameNetworkNode::getConnector(EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter != m_mapServerConns.end())
    {
        return iter->second.Connector;
    }

    return nullptr;
}


ServerConn* GameNetworkNode::getServerConn(EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter != m_mapServerConns.end())
    {
        return &(iter->second);
    }

    return nullptr;
}

void GameNetworkNode::heartbeatScheduler(float dt)
{
	Head head1;
	head1.MakeCommand(MAIN_CMD::CMD_HEARTBEART, MAIN_CMD::CMD_HEARTBEART);
	head1.length = sizeof(head1);
	head1.uid = 0;
	sendData(reinterpret_cast<char*>(&head1), sizeof(head1));
}

void GameNetworkNode::registeNetworkCommunicate(void *data)
{
    m_fTickNet = 0;
}

void GameNetworkNode::startHeartbeat()
{
	KX_LOGDEBUG("============startHeartbeat============");
	setIsHeartbeat(true);
	this->scheduleUpdate();
}

void GameNetworkNode::processLogic(int nMainCmd, int nSubCmd, char* buffer)
{
	//分发消息 到controller
	recvMessage(nMainCmd, nSubCmd, buffer);
}
