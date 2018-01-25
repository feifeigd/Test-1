#ifndef __GAMEMODULE_H__
#define __GAMEMODULE_H__

/*
	���մӷ���˵���Ϣ,ת��������controller��
*/

#include "helper/BaseModule.h"
#include <map>
#include <vector>

using namespace std;


class GameModule : public BaseModule
{
public:
;

	GameModule(void);
	~GameModule(void);    

	// �����յ�������
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);
    // ������ֵĴ���
    virtual void processError(IKxComm *target);
    // �����¼�
    virtual void processEvent(int eventId, IKxComm* target);

private:
	int mHeartBeatCount;
};

#endif 
