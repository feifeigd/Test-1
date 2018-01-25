#include "SceneLogin.h"
#include "LayerLogin.h"
#include "LoginController.h"

using namespace cocos2d;

SceneLogin::SceneLogin()
{

}

SceneLogin::~SceneLogin()
{

}

bool SceneLogin::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLayerLogin = LayerLogin::create();
	this->addChild(m_pLayerLogin);

	return true;
}

void SceneLogin::onEnter()
{
	Scene::onEnter();

}

void SceneLogin::onExit()
{
	Scene::onExit();
}

