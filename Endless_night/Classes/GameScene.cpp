#include "GameScene.h"//bringing in the game scene
#include "SimpleAudioEngine.h"//iporting the audio engine
#include "MainMenuScene.h"
#include "Definitions.h"
#include "Level_2Scene.h"
#include "GameOverScene.h"
#include "Waypoints.h"
#include "EndGameScene.h"


using namespace CocosDenshion; // namespace for audio engine 
using namespace cocos2d;


USING_NS_CC;

#define BACKGROUND_MUSIC_SFX "main-game-theme.mp3"//sound init for music
#define TOWER_SHOOTING_SFX "grenade.mp3" //sound init for music
#define DEATH_SOUND_SFX "whip.mp3"//sound for the enemy death 
#define COCOS2D_DEBUG 1

enum class PhysicsCategory
{
	None = 0,
	Enemy = (1 << 0),    // 1
	Projectile = (1 << 1),// 2
	gameOverBox = (1 << 1),
	Player = (1 << 1),
};

Scene* GameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();//scene created with physics
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));//setting the gravity to fall in whaterver way via x/y coordnate 
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);//red box around colisions

	// 'layer' is an autorelease object
	auto layer = GameScene::create();//creating the game layer 

	// add layer as a child to scene
	scene->addChild(layer);//adding the layer to the scene 

	// return the scene
	return scene;//returning the scene so it can be made 
}
/*
GameScene::~GameScene()
{
	_waypoints->release();
}*/
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
	
	towerfireRate = 1;
	towerDamage = 10;

	enemyScore = 0;
	gameOver = false;

															 
															 // 3
	auto backgroundSprite = Sprite::create("backgroundCastle.png");// creating the background and adding a sprite
	// setting the postition of the sprite on screen  using the size of the window
	backgroundSprite->setPosition(Point(winSize.width / 2 + origin.x, winSize.height / 2 + origin.y));
	this->addChild(backgroundSprite);//adding the bacground to the scene

	//auto tiledMap = ("Castle Backgroud.tmx");
	//std::string file = "Castle Background.tmx";
	//auto str = String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename(file.c_str()).c_str());
	//_tileMap = TMXTiledMap::createWithXML(str->getCString(), "");
	//_background = _tileMap->layerNamed("Castle Ground");
	//_background2 = _tileMap->layerNamed("Castle Wall");
	//_background3 = _tileMap->layerNamed("Castle Objects");
	//this->addChild(_tileMap);
	gameOverBox = Sprite::create("castle.png");//creating sprite  for collisions when enemy hit after player on map
	gameOverBox->setPosition(Vec2(winSize.width * 0.05, winSize.height * 0.5)); //seting postion for collision box
	auto _gameoverboxPos = gameOverBox->getContentSize();

	auto physicsGameOverBox = PhysicsBody::createBox(Size(40,720),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
//	auto physicsGameOverBox = PhysicsBody::createBox(Size(_gameoverboxPos.width, _gameoverboxPos.height),
	//	PhysicsMaterial(0.1f, 1.0f, 0.0f));

	//setting up the physics 
	// 2
	physicsGameOverBox->setDynamic(false);
	// 3
	physicsGameOverBox->setCategoryBitmask((int)PhysicsCategory::gameOverBox);
	physicsGameOverBox->setCollisionBitmask((int)PhysicsCategory::None); //1
	physicsGameOverBox->setContactTestBitmask((int)PhysicsCategory::Enemy);
	gameOverBox->setPhysicsBody(physicsGameOverBox); //when added to screen the player dissaperars
	gameOverBox->setVisible(false); //image of castle turhed off here makes the image transparnt

	this->addChild(gameOverBox);//adding the gameoverbox to the scene
	// 4
	_player = Sprite::create("cannon.png");//creating the player, player is made in the header file 
	_player->setPosition(Vec2(winSize.width * 0.1, winSize.height * 0.6));//setting the players location 
	auto playerSize = _player->getContentSize();

	auto physicsPlayer = PhysicsBody::createBox(Size(playerSize.width, playerSize.height),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));

	//setting up the physics 
	// 2
	physicsPlayer->setDynamic(false);
	// 3
	physicsPlayer->setCategoryBitmask((int)PhysicsCategory::Player);
	physicsPlayer->setCollisionBitmask((int)PhysicsCategory::None);
	physicsPlayer->setContactTestBitmask((int)PhysicsCategory::Enemy);
	_player->setPhysicsBody(physicsPlayer); //when added to screen the player dissaperars
	
	this->addChild(_player);//adding the player to the scene // not used towerfireRate

	//adding monsters randomly at 1 second intervial 
	srand((unsigned int)time(nullptr));
	this->schedule(schedule_selector(GameScene::addMonster),1 );

	//getting the mouse click form the player
	auto eventListener = EventListenerTouchOneByOne::create();
	eventListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, _player);

	//second tower will go here, have to get tbe collisions working for the aim
	//contact listener for enemy collision with player
	/*
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBeganEndGame, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
   */
	auto contactListenerB = EventListenerPhysicsContact::create();
	contactListenerB->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListenerB, this);
	
	//playing the background music 
	SimpleAudioEngine::getInstance()->playBackgroundMusic(BACKGROUND_MUSIC_SFX, true);

	// button to go back to the main menu 
	auto menu = MenuItemImage::create("menu.png", "menuClicked.png", CC_CALLBACK_1(GameScene::GoToMainMenuScene, this));
	menu->setPosition(Point(winSize.width * 0.1, winSize.height * 1));// change the size of the image in your recouce folder to maxamise efficinty 

	auto backToMenu = Menu::create(menu, NULL);
	backToMenu->setPosition(Point::ZERO);
	this->addChild(backToMenu);

	const float ScoreFontSize = 16;
	const float  ScorePostitionX = 24;
	const float ScorePostitionY = 12;
	score = 0;

	__String *tempScore = __String::createWithFormat("Score:%i", score);

	scoreLabel = Label::create(tempScore->getCString(), "fonts/Marker felt.ttf", winSize.height* SCORE_FONT_SIZE);
	scoreLabel->setColor(Color3B::RED);
	scoreLabel->setAnchorPoint(ccp(0, 1));
	scoreLabel->setPosition(winSize.width / 2 + origin.x, winSize.height * SCORE_FONT_SIZE);

	this->addChild(scoreLabel, 1000);
	const float livesFontSize = 16;
	const float  livesPostitionX = 24;
	const float livesPostitionY = 24;
	//lives = 3;
	gameEnded = false;
	towerHp = 15;

	__String *tempLives = __String::createWithFormat("Lives:%d", towerHp);

	livesLabel = Label::create(tempLives->getCString(), "fonts/Marker felt.ttf", winSize.height* LIVES_FONT_SIZE);
	livesLabel->setColor(Color3B::RED);
	livesLabel->setAnchorPoint(ccp(0, 1));
	livesLabel->setPosition(winSize.width / 3 + origin.x, winSize.height * LIVES_FONT_SIZE);

	this->addChild(livesLabel, 10);
	// returning that all is ok as is a bool(booean class)
				// saving game data for high score
	//CCUserDefault::sharedUserDefault()->setIntegerForKey("score", score); // breaks game needs work
	//CCUserDefault::sharedUserDefault()->flush();
	//this->loadTowerPositions(); // breaks game not working yet
	//waypoints
	this->addWayPoints();

return true;// returning that all is ok as is a bool(booean class)
}//end is init()

void GameScene::addMonster(float dt)
{
	maxHp = 60;
	currentHp = maxHp;
	enemyDamage = 30;
	auto monster = Sprite::create("monster.png");//making the enemy 

	//giving the monster some attributes 
	auto monsterSize = monster->getContentSize();
	auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width, monsterSize.height),
		PhysicsMaterial(0.0, 0.0f, 0.0f)); //weight, bouncy, sharpness

	//setting up the physics 
	// 2
	physicsBody->setDynamic(true);
	// 3
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Enemy);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);
	//physicsBody->setContactTestBitmask((int)PhysicsCategory::Player);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::gameOverBox);
	




	monster->setPhysicsBody(physicsBody);// adding monster to the physics engine so it can be colided 
	// 1
	// giving the monster some movement and coordnates
	auto monsterContentSize = monster->getContentSize();
	auto selfContentSize = this->getContentSize();
	int minY = monsterContentSize.height / 2;
	int maxY = selfContentSize.height - monsterContentSize.height / 2;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;

	monster->setPosition(Vec2(selfContentSize.width + monsterContentSize.width / 2, randomY));
	this->addChild(monster);//adding enemy to the layer 

	// 2
	int minDuration = 10.0;
	int maxDuration = 11.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;
	// currentHp -= actionRemove;
	//if (currentHp <= 0)
	//{
	//	auto actionRemove = RemoveSelf::create();
	//}
	// 3
	//moving and taking off when collided 
	auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width / 2, randomY));
	auto actionRemove = RemoveSelf::create();
	monster->runAction(Sequence::create(actionMove, actionRemove, nullptr));
}

bool GameScene::onTouchBegan(Touch * touch, Event *unused_event)
{
	// 2
	//setting up the vecs and what they are doing 
	Vec2 touchLocation = touch->getLocation();
	Vec2 offset = touchLocation - _player->getPosition();



	// 3
	if (offset.x < 0) //offset is the area at which the "bullet" will fire 
	{
		return true;
	}

	// 4
	auto projectile = Sprite::create("cannonball.png");//making the projectile 
	projectile->setPosition(_player->getPosition());
	this->addChild(projectile);//adding it to the layer 



	//setting the phycis of the projectile 
	auto projectileSize = projectile->getContentSize();
	auto physicsBody = PhysicsBody::createCircle(projectileSize.width / 2);
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Projectile);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Enemy);
	projectile->setPhysicsBody(physicsBody);



	// 5
	offset.normalize();
	auto shootAmount = offset * 1000;

	// 6
	auto realDest = shootAmount + projectile->getPosition();

	// 7
	auto actionMove = MoveTo::create(2.0f, realDest);
	auto actionRemove = RemoveSelf::create();
	projectile->runAction(Sequence::create(actionMove, actionRemove, nullptr));


	// sound plays once the player clicks the screen 
	SimpleAudioEngine::getInstance()->playEffect(TOWER_SHOOTING_SFX);//tower shooting sound 
	return true;
}


bool GameScene::onContactBegan(PhysicsContact &contact)
{
	auto nodeProjectile = contact.getShapeA()->getBody()->getNode();//could be enemy or visa veras 
	auto nodeEnemy = contact.getShapeB()->getBody()->getNode();//could be projectile or visa versa 


	nodeEnemy->removeFromParent();//remove the enemy  and cleans up accord to doc
	CCLOG("Removed");
	SimpleAudioEngine::getInstance()->playEffect(DEATH_SOUND_SFX);//enemy dying sound
	nodeProjectile->removeFromParent();//remove the projectile 
	CCLOG("point added");
	score++;


	__String * tempScore = __String::createWithFormat("Score:%i", score);
	scoreLabel->setString(tempScore->getCString());
	//if score reaches 10 new level or end game scene with transmitions to gameOverscene or new scene 

	if (score == 10)
	{
		auto scene = Level_2Scene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
	}

	return true;
}
/*  //enemy collision between enemy and gameOverbox
bool GameScene::onContactBeganEndGame(PhysicsContact &contact)
{
	auto endgameBox = contact.getShapeA()->getBody()->getNode(); //enemy collides off game overbox
	auto monst = contact.getShapeB()->getBody()->getNode(); //endgame box collides with enemy

	monst->removeFromParentAndCleanup(false);//remove the enemy 
	SimpleAudioEngine::getInstance()->playEffect(DEATH_SOUND_SFX);//enemy dying sound
	//endgameBox->removeFromParent();//removes 
	
	towerHp--;
	__String * tempLives = __String::createWithFormat("Lives:%i", towerHp);
	livesLabel->setString(tempLives->getCString());
	if (towerHp <= 0)
	{
		this->doGameOver();
	}	
	return true;

}*/
 void GameScene::loadTowerPositions()
{
	CCArray* towerPositions = CCArray::createWithContentsOfFile("TowersPosition.xml");
	CCObject* obj;
	CCARRAY_FOREACH(towerPositions, obj)
	{
		//Converting to actual obj type
		CCDictionary* towerPos = (CCDictionary*)obj;

		//Creating sprite at position
		CCSprite* towerBase = CCSprite::create("open_spot.png");
		CCPoint p = ccp(towerPos->valueForKey("x")->intValue(),
			towerPos->valueForKey("y")->intValue());
		towerBase->setPosition(p);

		//Adding to layer
		this->addChild(towerBase);

		//Adding to array
		towerBases.addObject(towerBase);
	}
}
void GameScene::SetIsScored()
{
	scored = true;
}

bool GameScene::GetIsScored()
{
	return scored;
}

bool GameScene::GetLives()
{
	return livesLeft;
}
void GameScene::SetLives()
{
	livesLeft = true;
}

void GameScene::getHpDamage()
{
	towerHp--;
	__String * tempLives = __String::createWithFormat("Lives:%i", towerHp);
	livesLabel->setString(tempLives->getCString());
	if (towerHp <= 0)
	{
		this->doGameOver();
	}

}
void GameScene::doGameOver()
{
	if (!gameEnded) {
		auto scene = EndGameScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
	}
}
void GameScene::addWayPoints()
{
	_waypoints = Array::create();
	_waypoints->retain();

	Waypoints* point1 = Waypoints::nodeWithTheGame(this, Point(240, 70));
	_waypoints->addObject(point1);

	Waypoints* point2 = Waypoints::nodeWithTheGame(this, Point(80, 70));
	_waypoints->addObject(point2);

	Waypoints* point3 = Waypoints::nodeWithTheGame(this, Point(70, 220));
	_waypoints->addObject(point3);

	Waypoints* point4 = Waypoints::nodeWithTheGame(this, Point(810, 230));
	_waypoints->addObject(point4);

	Waypoints* point5 = Waypoints::nodeWithTheGame(this, Point(820, 423));
	_waypoints->addObject(point5);

	Waypoints* point6 = Waypoints::nodeWithTheGame(this, Point(-90, 240));
	_waypoints->addObject(point6);
}
/*
bool GameScene::canBuyTower()
{
	//return(playerGold - TOWER_COST >= 0);
}
*/
void GameScene::awardGold(int gold)
{
	//playerGold += gold;
	//ui_gold_lbl->setString(CCString::createWithFormat("GOLD %d", playerHP)->getCString());
}

bool checkCirclesCollide(CCPoint center1, float radius1, CCPoint center2, float radius2)
{
	float distance = ccpDistance(center1, center2);
	return distance <= radius1 + radius2;
}


void GameScene::menuCloseCallback(Ref* pSender)// setting up the close button "quit"
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0); // origallny in the code 
#endif

}
//
void GameScene::GoToMainMenuScene(Ref *sender)
{
	auto scene = MainMenuScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
/*
void GameScene::GoToGameOverScene(float dt)
{
	//auto scene = GameOverScene::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}*/