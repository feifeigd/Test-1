#include "GameModule.h"
#include "GameNetworkNode.h"
#include "server/Protocol.h"

GameModule::GameModule(void)
{
}

GameModule::~GameModule(void)
{
}

void GameModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int id = head->uid;

	if (nMainCmd == MAIN_CMD::CMD_HEARTBEART && nSubCmd == MAIN_CMD::CMD_HEARTBEART)
	{
		KX_LOGDEBUG("================= heartbeart!==================");
		return;
	}
	//��Ϣת�Ӹ�CGameNetworkNode�ַ�����
	GameNetworkNode::getInstance()->processLogic(nMainCmd, nSubCmd, buffer);

}

void GameModule::processError(IKxComm *target)
{

}

void GameModule::processEvent(int eventId, IKxComm* target)
{

}
