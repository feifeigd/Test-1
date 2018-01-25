#ifndef __GAMEMODULE_H__
#define __GAMEMODULE_H__

/*
	接收从服务端的消息,转发到各个controller中
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

	// 处理收到的数据
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);
    // 处理出现的错误
    virtual void processError(IKxComm *target);
    // 处理事件
    virtual void processEvent(int eventId, IKxComm* target);

private:
	int mHeartBeatCount;
};

#endif 
