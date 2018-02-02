#include "LayerLogin.h"
#include "LoginController.h"
#include "LoginModel.h"

USING_NS_CC;

LayerLogin::LayerLogin()
{
	LoginModel::getInstance()->addObserver(this);
}

LayerLogin::~LayerLogin()
{
	LoginModel::getInstance()->removeObserver(this);
}
bool LayerLogin::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}


void LayerLogin::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();




	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(LayerLogin::menuCloseCallback, this));

	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
		float y = origin.y + closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label

	auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
	if (label == nullptr)
	{
		problemLoading("'fonts/Marker Felt.ttf'");
	}
	else
	{
		// position the label on the center of the screen
		label->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - label->getContentSize().height));

		// add the label as a child to this layer
		this->addChild(label, 1);
	}

	// add "HelloWorld" splash screen"
	auto sprite = Sprite::create("HelloWorld.png");
	if (sprite == nullptr)
	{
		problemLoading("'HelloWorld.png'");
	}
	else
	{
		// position the sprite on the center of the screen
		sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

		// add the sprite as a child to this layer
		this->addChild(sprite, 0);
	}
	LoginController::getInstance()->onServerInit();
}


void LayerLogin::menuCloseCallback(Ref* pSender)
{
	if (!LoginController::getInstance()->login())
	{
		KX_LOGDEBUG("send login cmd is failed!");
	}
}


/**********************************重写UI**************************************/

void LayerLogin::onEnter()
{
	Layer::onEnter();
}

void LayerLogin::onEnterTransitionDidFinish()
{
}


void LayerLogin::onExit()
{
	Layer::onExit();
}


void LayerLogin::updateSelf(void * data)
{
	ObserverParam* param = (ObserverParam*)data;

	switch (param->id)
	{
		case LOGIN_MODEL::CMD_LOGIN:
		{
			onRecvLoginData((const char*)param->updateData);
		}
		break;
		default:
			break;
	}
}

void LayerLogin::onRecvLoginData(const char* szMsg)
{
	KX_LOGDEBUG("登录成功!");
}
