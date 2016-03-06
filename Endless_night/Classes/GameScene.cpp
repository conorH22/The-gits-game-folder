#include "GameScene.h"//bringing in the game scene
#include "SimpleAudioEngine.h"//iporting the audio engine
#include "MainMenuScene.h"
#include "Definitions.h"
#include "Level_2Scene.h"
#include "GameOverScene.h"
//#include "loseScene.h"
#include "Level_1_Splash_Scene.h"
#include "Level_2_Splash_Scene .h"
#include "Box2D\Box2D.h"
#include <chrono>
#include <thread>
using namespace CocosDenshion; // namespace for audio engine 
using namespace cocos2d;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

USING_NS_CC;

#define BACKGROUND_MUSIC_SFX "main-game-theme.mp3"//sound init for music
#define TOWER_SHOOTING_SFX "grenade.mp3" //sound init for music
#define DEATH_SOUND_SFX "whip.mp3"//sound for the enemy death 
#define COCOS2D_DEBUG 1

enum class PhysicsCategory
{
	None = 0,
	Monster = (1 << 0),    // 1
	Collision = (1 << 1),
	Projectile = (1 << 1), // 2
	Player = (1 << 1),
	//All = PhysicsCategory::Monster | PhysicsCategory::Projectile // 3
};

Scene* GameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();//creating the scene with added physcis engine 
	scene->getPhysicsWorld()->setGravity(Vec2(5, -5));//setting the gravity to fall in whaterver way via x/y coordnate 
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);//red box around colisions

	// 'layer' is an autorelease object
	auto layer = GameScene::create();//creating the game layer 

	// add layer as a child to scene
	scene->addChild(layer);//adding the layer to the scene 

	// return the scene
	return scene;//returning the scene so it can be made 
}

// on "init" you need to initialize your instance
bool GameScene::init()//initing the game so the scene can be made 
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}//code like this for consisinsty 
	// 2
	auto origin = Director::getInstance()->getVisibleOrigin();//setting up the origin 
	auto winSize = Director::getInstance()->getVisibleSize();// as well as the window size or the visible size as well 

		// 3
	auto backgroundSprite = Sprite::create("backgroundCastle.png");// creating the background and adding a sprite
	// setting the postition of the sprite on screen  using the size of the window
	backgroundSprite->setPosition(Point(winSize.width / 2 + origin.x, winSize.height / 2 + origin.y));
	this->addChild(backgroundSprite);///adding the bacground to the scene
	// 4
	_player = Sprite::create("Knight.png");//creating the player, player is made in the header file 
	_player->setPosition(Vec2(winSize.width * 0.1, winSize.height * 0.2));//setting the players location 
	
	auto playerContentSize = _player->getContentSize();
	auto selfContentSize = this->getContentSize();
	//_player->setPosition(Vec2(selfContentSize.width + monsterContentSize.width / 2, randomY));

	auto playerBody = PhysicsBody::createBox(Size(winSize.width, winSize.height),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
	playerBody->setDynamic(false);
	playerBody->setCategoryBitmask((int)PhysicsCategory::Player);
	playerBody->setCollisionBitmask((int)PhysicsCategory::None);
	playerBody->setContactTestBitmask((int)PhysicsCategory::Monster);
	//playerBody->setContactTestBitmask((int)PhysicsCategory::Player);
	_player->setPhysicsBody(playerBody);
	this->addChild(_player);//adding the player to the scene



	//adding monsters randomly at 1 second intervial 
	//srand((unsigned int)time(nullptr));
	this->schedule(schedule_selector(GameScene::addMonster), 1);
	this->scheduleUpdate();
	//this->schedule(schedule_selector(GameScene::GoToGameOverScene), 20.0f);

	//getting the mouse click form the player
	auto eventListener = EventListenerTouchOneByOne::create();
	eventListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, _player);

	// second tower will go here, have to get tbe collisions working for the aim


	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
	//lisnter for drop and drag towers to defend whiteKnight
	/*auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBeganTower, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMovedTower, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEndedTower, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, towerSprite);
	*/
	//playing the background music 
	SimpleAudioEngine::getInstance()->playBackgroundMusic(BACKGROUND_MUSIC_SFX, true);

	// button to go back to the main menu 
	auto menu = MenuItemImage::create("menu.png", "menuClicked.png", CC_CALLBACK_1(GameScene::GoToMainMenuScene, this));
	menu->setPosition(Point(winSize.width * 0.1, winSize.height * 0.9));// change the size of the image in your recouce folder to maxamise efficinty 

	auto backToMenu = Menu::create(menu, NULL);
	backToMenu->setPosition(Point::ZERO);
	this->addChild(backToMenu);

	const float ScoreFontSize = 24;
	const float  ScorePostitionX = 24;
	const float ScorePostitionY = 12;
	score = 0;

	__String *tempScore = __String::createWithFormat("%i", score);

	scoreLabel = Label::create(tempScore->getCString(), "fonts/Marker felt.ttf", winSize.height* SCORE_FONT_SIZE);
	scoreLabel->setColor(Color3B::RED);
	scoreLabel->setAnchorPoint(ccp(0, 1));
	scoreLabel->setPosition(winSize.width / 2 + origin.x, winSize.height * SCORE_FONT_SIZE);

	this->addChild(scoreLabel, 1000);

	//this->schedule(schedule_selector(GameScene::addColision), 1);
	//this->scheduleUpdate();


	return true;// returning that all is ok as is a bool(booean class)



}//end is init()

void GameScene::addMonster(float dt)
{

	auto origin = Director::getInstance()->getVisibleOrigin();//setting up the origin 
	auto winSize = Director::getInstance()->getVisibleSize();// as well as the window size or the visible size as well 
	//creating monster 1
	auto monster = Sprite::create("Shadow1.png");//making the enemy 
	//giving the monster some attributes 
	auto monsterSize = monster->getContentSize();
	auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width, monsterSize.height),
											  PhysicsMaterial(0.1f, 1.0f, 0.0f));
											  physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Monster);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Player);
	monster->setPhysicsBody(physicsBody); 
//animations 
	Vector<SpriteFrame*> animFrames(4);

	for (int i = 1; i < 5; i++)
	{
		std::stringstream ss;
		ss << "Shadow" << i << ".png";
		String str = ss.str();
		auto frame = SpriteFrame::create(ss.str(), Rect(0, 0, 60, 105));
		animFrames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(animFrames, 0.4f);
	auto animate = Animate::create(animation);
	monster->runAction(RepeatForever::create(animate));

	// giving the monster some movement and coordnates
	auto monsterContentSize = monster->getContentSize();
	auto selfContentSize = this->getContentSize();
	int minY = monsterContentSize.height / 2;
	int maxY = selfContentSize.height - monsterContentSize.height / 2;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	//monster->setPosition(Vec2(selfContentSize.width / 3  + monsterContentSize.width / 5 ,randomY));
	monster->setPosition(Vec2(selfContentSize.width + monsterContentSize.width / 2, randomY));
	this->addChild(monster);//adding enemy to the layer 

	int minDuration = 1000.0;
	int maxDuration = 50000;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	//auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width *2, randomY));
	auto actionMove = MoveTo::create(randomDuration, Vec2(winSize.width * 0.1, winSize.height * 0.2));
	auto actionRemove = RemoveSelf::create();
	monster->runAction(Sequence::create(actionMove, actionRemove, nullptr));
	// finish monster 1

	//creating monster 2
	
	
	// finished creating the second monster
}
void GameScene::addMonster2(float dt)
{
	auto monster2 = Sprite::create("BadGuy1.png");
	auto monster2Size = monster2->getContentSize();
	auto physicsBody2 = PhysicsBody::createBox(Size(monster2Size.width, monster2Size.height),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
	physicsBody2->setDynamic(true);
	physicsBody2->setCategoryBitmask((int)PhysicsCategory::Monster);
	physicsBody2->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody2->setContactTestBitmask((int)PhysicsCategory::Projectile);
	monster2->setPhysicsBody(physicsBody2);

	// giving the monster some movement and coordnates
	auto monster2ContentSize = monster2->getContentSize();
	auto selfContentSize2 = this->getContentSize();
	int minY2 = monster2ContentSize.height / 2;
	int maxY2 = selfContentSize2.height - monster2ContentSize.height / 2;
	int rangeY2 = maxY2 - minY2;
	int randomY2 = (rand() % rangeY2) + minY2;
	monster2->setPosition(Vec2(selfContentSize2.width + monster2ContentSize.width / 2, randomY2));
	this->addChild(monster2);//adding enemy to the layer 

	int minDuration2 = 9.0;
	int maxDuration2 = 60.0;
	int rangeDuration2 = maxDuration2 - minDuration2;
	int randomDuration2 = (rand() % rangeDuration2) + minDuration2;

	auto actionMove2 = MoveTo::create(randomDuration2, Vec2(-monster2ContentSize.width / 2, randomY2));
	auto actionRemove2 = RemoveSelf::create();
	monster2->runAction(Sequence::create(actionMove2, actionRemove2, nullptr));
}

bool GameScene::onTouchBegan(Touch * touch, Event *unused_event)

{
	
	// 2
	//setting up the vecs and what they are doing 
	Vec2 touchLocation = touch->getLocation();
	Vec2 offset = touchLocation - _player->getPosition();

	//sleep_for(nanoseconds(1));
	//sleep_until(system_clock::now() + seconds(1));
	
	


	// 3
	if (offset.x < 0) //offset is the area at which the "bullet" will fire 
	{
		
		return true;
	}

	// 4
	auto projectile = Sprite::create("Spear.png");//making the projectile 
	projectile->setPosition(_player->getPosition());
	this->addChild(projectile);//adding it to the layer 

	

	//setting the phycis of the projectile 
	auto projectileSize = projectile->getContentSize();
	auto physicsBody = PhysicsBody::createCircle(projectileSize.width / 2);
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Projectile);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Monster);
	projectile->setPhysicsBody(physicsBody);
	offset.normalize();

	auto shootAmount = offset * 2000;

	// 6
	auto realDest = shootAmount + projectile->getPosition();

	// 7
	auto actionMove = MoveTo::create(2.0f, realDest);
	auto actionRemove = RemoveSelf::create();
	projectile->runAction(Sequence::create(actionMove, actionRemove, nullptr));


	// sound plays once the player clicks the screen 
	SimpleAudioEngine::getInstance()->playEffect(TOWER_SHOOTING_SFX);//tower shooting sound 

	auto delay = DelayTime::create(10);
	auto delaySequence = Sequence::create(delay, delay->clone(), delay->clone(),
		delay->clone(), nullptr);

	projectile->runAction(Sequence::create(delay, actionMove, actionRemove, nullptr));

	return true;
}

/*
bool GameScene::onContactBeganPlayer(PhysicsContact &contact)
{
	auto nodePlayer = contact.getShapeA()->getBody()->getNode();//could be enemy or visa veras 
	auto nodeEnemy = contact.getShapeB()->getBody()->getNode();//could be projectile or visa versa 
	
	

	//if (nodeEnemy)
	//nodePlayer->removeFromParent();//remove the enemy 
	CCLOG("Removed");
	SimpleAudioEngine::getInstance()->playEffect(DEATH_SOUND_SFX);//enemy dying sound
	if(nodeEnemy)
	nodeEnemy->removeFromParent();//remove the projectile 
	CCLOG("point added");
	Lives--;



	__String * tempLives = __String::createWithFormat("%i", lives);
	livesLabel->setString(tempLives->getCString());
	//if score reaches 10 new level or end game scene with transmitions to gameOverscene or new scene 

	if (lives<= 0)
	{
		auto scene = LoseScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSATION_TIME, scene));
		
	}

	return true;
}
*/
bool GameScene::onContactBegan(PhysicsContact &contact)
{
	auto nodeEnemy = contact.getShapeA()->getBody()->getNode();//could be enemy or visa veras 
	auto nodeProjectile = contact.getShapeB()->getBody()->getNode();//could be projectile or visa versa 



	if (nodeEnemy)
		nodeEnemy->removeFromParent();//remove the enemy 
	CCLOG("Removed");
	SimpleAudioEngine::getInstance()->playEffect(DEATH_SOUND_SFX);//enemy dying sound
	nodeProjectile->removeFromParent();//remove the projectile 
	CCLOG("point added");
	score++;



	__String * tempScore = __String::createWithFormat("%i", score);
	scoreLabel->setString(tempScore->getCString());
	//if score reaches 10 new level or end game scene with transmitions to gameOverscene or new scene 

	if (score == 1)
	{
		auto scene = Level2SplashScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSATION_TIME, scene));

	}

	return true;
}

bool GameScene::onTouchBeganTower(Touch* touch, cocos2d::Event* event)
{
	//method is not needed but is implemented to use ontouchmovedtower
	return true;
}

void GameScene::onTouchMovedTower(Touch* touch, cocos2d::Event* event)
{
	/*if (sprite->getBoundingBox().containsPoint(touch->getLocation())
	{
		sprite->setPostion(sprite->getPostion() + touch->getDelta());
	}*/
}

void GameScene::onTouchEndedTower(Touch* touch, cocos2d::Event* event)
{   // tower sprite goes here
	/*if (Sprite->getBoundingBox().containsPoint(touch->getLocation()))
	{
		log("sprite is drop event");
	}*/
}

void GameScene::SetIsScored()
{
	scored = true;
}

bool GameScene::GetIsScored()
{
	return scored;
}



void GameScene::menuCloseCallback(Ref* pSender)// setting up the close button "quit"
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0); // origallny in the code 
#endif

}

void GameScene::GoToMainMenuScene(Ref *sender)
{
	auto scene = MainMenuScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSATION_TIME, scene));
}

//void GameScene::GoToGameOverScene(float dt)
//{
//auto scene = GameOverScene::createScene();
//Director::getInstance()->replaceScene(TransitionFade::create(TRANSATION_TIME, scene));
//}

