#ifndef LayerLogin_h__
#define LayerLogin_h__
#include <string>
#include "cocos2d.h"


class LayerLogin: public cocos2d::Layer
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

protected:
	void	initUI();
	void menuCloseCallback(Ref* pSender);
};

#endif // LayerLogin_h__
