#ifndef LayerLogin_h__
#define LayerLogin_h__
#include <string>
#include "cocos2d.h"
#include "Observer/Observer.h"

class LayerLogin: public cocos2d::Layer , Observer
{
public:
	LayerLogin();
	virtual ~LayerLogin();
	CREATE_FUNC(LayerLogin);

public:

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();

	virtual void onRecvLoginData(int nResult, const char* szMsg);

	virtual void updateSelf(void * data);
protected:
	void	initUI();
	void menuCloseCallback(Ref* pSender);
};

#endif // LayerLogin_h__
