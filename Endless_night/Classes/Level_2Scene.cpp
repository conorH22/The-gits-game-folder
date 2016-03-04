#include "Level_2Scene.h"//bringing in the game scene
#include "SimpleAudioEngine.h"//iporting the audio engine
#include "MainMenuScene.h"
#include "Definitions.h"
#include "Level_3_scene.h"
#include "Level3SplashScene.h"
#include "GameOverScene.h"
#include "LooseScene.h"



using namespace CocosDenshion; // namespace for audio engine 
using namespace cocos2d;


USING_NS_CC;

#define BOSS_MUSIC_SFX "Boss.mp3"//sound init for music
#define TOWER_SHOOTING_SFX "grenade.mp3" //sound init for music
#define DEATH_SOUND_SFX "whip.mp3"//sound for the enemy death 
#define COCOS2D_DEBUG 1

enum class PhysicsCategory
{
	None = 0,
	Monster = (1 << 0),    // 1
	Projectile = (1 << 1), // 2
	gameOverBox = (1 << 1),
	Player = (1 << 1),
	//All = PhysicsCategory::Monster | PhysicsCategory::Projectile | PhysicsCategory::Monster |PhysicsCategory::gameOverBox // 3
	
};

Scene* Level_2Scene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();//creating the scene with added physcis engine 
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));//setting the gravity to fall in whaterver way via x/y coordnate 
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);//red box around colisions

	// 'layer' is an autorelease object
	auto layer = Level_2Scene::create();//creating the game layer 

	// add layer as a child to scene
	scene->addChild(layer);//adding the layer to the scene 

	// return the scene
	return scene;//returning the scene so it can be made 
}

// on "init" you need to initialize your instance
bool Level_2Scene::init()//initing the game so the scene can be made 
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

	gameOver = false;
	auto backgroundSprite = Sprite::create("backgroundCastle.png");// creating the background and adding a sprite
	// setting the postition of the sprite on screen  using the size of the window
	backgroundSprite->setPosition(Point(winSize.width / 2 + origin.x, winSize.height / 2 + origin.y));
	this->addChild(backgroundSprite);///adding the bacground to the scene
	// 4
	_player = Sprite::create("Knight.png");//creating the player, player is made in the header file 
	_player->setPosition(Vec2(winSize.width * 0.1, winSize.height * 0.5));//setting the players location 
	this->addChild(_player);//adding the player to the scene

	gameOverBox = Sprite::create("castle.png");//creating sprite  for collisions when enemy hit after player on map
	gameOverBox->setPosition(Vec2(winSize.width * 0.05, winSize.height * 0.5)); //seting postion for collision box
	auto _gameoverboxPos = gameOverBox->getContentSize();

	auto physicsGameOverBox = PhysicsBody::createBox(Size(40, 720),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
	//	auto physicsGameOverBox = PhysicsBody::createBox(Size(_gameoverboxPos.width, _gameoverboxPos.height),
	//	PhysicsMaterial(0.1f, 1.0f, 0.0f));

	//setting up the physics 
	// 2
	physicsGameOverBox->setDynamic(true);
	// 3
	physicsGameOverBox->setCategoryBitmask((int)PhysicsCategory::gameOverBox);
	physicsGameOverBox->setCollisionBitmask((int)PhysicsCategory::None); //1
	physicsGameOverBox->setContactTestBitmask((int)PhysicsCategory::Monster);
	gameOverBox->setPhysicsBody(physicsGameOverBox); //when added to screen the player dissaperars
	gameOverBox->setVisible(false); //image of castle turhed off here makes the image transparnt

	this->addChild(gameOverBox);//adding the gameoverbox to the scene
								// 4

	//adding monsters randomly at 1 second intervial 
	//srand((unsigned int)time(nullptr));
	this->schedule(schedule_selector(Level_2Scene::addMonster), 0.7);

	//this->schedule(schedule_selector(GameScene::GoToGameOverScene), 20.0f);

	//getting the mouse click form the player
	auto eventListener = EventListenerTouchOneByOne::create();
	eventListener->onTouchBegan = CC_CALLBACK_2(Level_2Scene::onTouchBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, _player);

	// second tower will go here, have to get tbe collisions working for the aim


	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Level_2Scene::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
/*
	auto contactListenerB = EventListenerPhysicsContact::create();
	contactListenerB->onContactBegin = CC_CALLBACK_1(Level_2Scene::onContactBeganEndGame, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListenerB, this);
	*/
	//playing the background music 
	SimpleAudioEngine::getInstance()->playBackgroundMusic(BOSS_MUSIC_SFX, true);

	// button to go back to the main menu 
	// button to go back to the main menu 
	auto menu = MenuItemImage::create("menu.png", "menuClicked.png", CC_CALLBACK_1(Level_2Scene::GoToMainMenuScene, this));
	menu->setPosition(Point(winSize.width * 0.1, winSize.height * 1));// change the size of the image in your recouce folder to maxamise efficinty 


	auto backToMenu = Menu::create(menu, NULL);
	backToMenu->setPosition(Point::ZERO);
	this->addChild(backToMenu);

	const float ScoreFontSize = 24;
	const float  ScorePostitionX = 24;
	const float ScorePostitionY = 12;
	score = 0;

	__String *tempScore = __String::createWithFormat("Score:%i", score);

	scoreLabel = Label::create(tempScore->getCString(), "fonts/Marker felt.ttf", winSize.height* SCORE_FONT_SIZE);
	scoreLabel->setColor(Color3B::RED);
	scoreLabel->setAnchorPoint(ccp(0, 1));
	scoreLabel->setPosition(winSize.width / 2 + origin.x, winSize.height * SCORE_FONT_SIZE);

	this->addChild(scoreLabel, 1000);
	gameEnded = false;
	towerHp = 5;

	__String *tempLives = __String::createWithFormat("Lives:%d", towerHp);

	livesLabel = Label::create(tempLives->getCString(), "fonts/Marker felt.ttf", winSize.height* LIVES_FONT_SIZE);
	livesLabel->setColor(Color3B::RED);
	livesLabel->setAnchorPoint(ccp(0, 1));
	livesLabel->setPosition(winSize.width / 3 + origin.x, winSize.height * LIVES_FONT_SIZE);

	this->addChild(livesLabel, 10);
	return true;// returning that all is ok as is a bool(booean class)



}//end is init()

void Level_2Scene::addMonster(float dt)
{
	auto monster = Sprite::create("Shadow1.png");//making the enemy 

	//giving the monster some attributes 
	auto monsterSize = monster->getContentSize();
	auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width, monsterSize.height),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));

	//setting up the physics 
	// 2
	physicsBody->setDynamic(true);
	// 3
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Monster);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);




	monster->setPhysicsBody(physicsBody);// adding monster to the physics engine so it can be colided 
	
	Vector<SpriteFrame*> animFrames(4);

	for (int i = 1; i < 5; i++)
	{
		std::stringstream ss;
		ss << "Shadow" << i << ".png";
		String str = ss.str();
		auto frame = SpriteFrame::create(ss.str(), Rect(0, 0, 60, 105));
		animFrames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
	auto animate = Animate::create(animation);
	monster->runAction(RepeatForever::create(animate));
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
	int minDuration = 3.0;
	int maxDuration = 30.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	//moving and taking off when collided 
	auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width / 2, randomY));
	auto actionRemove = RemoveSelf::create();
	monster->runAction(Sequence::create(actionMove, actionRemove, nullptr));
}

bool Level_2Scene::onTouchBegan(Touch * touch, Event *unused_event)
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


bool Level_2Scene::onContactBegan(PhysicsContact &contact)
{
	auto nodeEnemy = contact.getShapeA()->getBody()->getNode();//could be enemy or visa veras 
	auto nodeProjectile = contact.getShapeB()->getBody()->getNode();//could be projectile or visa versa 

	if(nodeEnemy)
	nodeEnemy->removeFromParent();//remove the enemy 
	CCLOG("removed");
	SimpleAudioEngine::getInstance()->playEffect(DEATH_SOUND_SFX);//enemy dying sound
	nodeProjectile->removeFromParent();//remove the projectile 
	CCLOG("point added");
	score++;


	__String * tempScore = __String::createWithFormat("Score:%i", score);
	scoreLabel->setString(tempScore->getCString());
	//if score reaches 10 new level or end game scene with transmitions to gameOverscene or new scene 

	if (score == 1)
	{
		auto scene = Level3SplashScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
	}

	return true;
}
/*
bool Level_2Scene::onContactBeganEndGame(PhysicsContact &contact)
{
	auto monst = contact.getShapeA()->getBody()->getNode(); //enemy collides off game overbox
	auto enemyGameBox = contact.getShapeB()->getBody()->getNode(); //endgame box collides with enemy

	monst->removeFromParent();//remove the enemy 
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

}
*/
void Level_2Scene::SetIsScored()
{
	scored = true;
}

bool Level_2Scene::GetIsScored()
{
	return scored;
}
void Level_2Scene::doGameOver()
{
	if (!gameEnded) {
		auto scene = LooseScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
	}
}


void Level_2Scene::menuCloseCallback(Ref* pSender)// setting up the close button "quit"
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0); // origallny in the code 
#endif

}

void Level_2Scene::GoToMainMenuScene(Ref *sender)
{
	auto scene = MainMenuScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

//void GameScene::GoToGameOverScene(float dt)
//{
//auto scene = GameOverScene::createScene();
//Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
//}