#include "Level_2Scene.h"
#include"Level_2_Splash_Scene .h"
#include "Definitions.h"
#include "MainMenuScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

USING_NS_CC;
#define SPLASH_SCREEN_SOUND_SFX "Super Happy Funtime.mp3"//sound for the splash screen
Scene* Level2SplashScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Level2SplashScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Level2SplashScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//this->scheduleOnce(schedule_selector(Level1SplashScene::GoToGameScene), DISPLAY_TIME_SPLASH_SCENE);//going to the main menu 

	auto backgroundSprite = Sprite::create("Level2_Instructions.png");//image for the splash screen 
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	backgroundSprite->setScale(0.75);
	this->addChild(backgroundSprite);

	SimpleAudioEngine::getInstance()->playEffect(SPLASH_SCREEN_SOUND_SFX);//splash scrren 

	auto ContinueGame = MenuItemImage::create("continue.png", "continue.png", CC_CALLBACK_1(Level2SplashScene::GoToLevel2Scene, this));
	ContinueGame->setPosition(Point(visibleSize.width / 1.5 + origin.x, visibleSize.height / 5 + origin.y));																									   // void goToGameScene();

	auto ContinueGameButton = Menu::create(ContinueGame, NULL);
	ContinueGameButton->setPosition(Point::ZERO);
	this->addChild(ContinueGameButton);

	return true;

}

void Level2SplashScene::GoToLevel2Scene(Ref *sender)
{
	auto scene = Level_2Scene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSATION_TIME, scene));

}
