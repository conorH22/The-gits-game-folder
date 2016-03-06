#ifndef __GAME_SCENE_H__ // used so that hte header will only be used once per compile
#define __GAME_SCENE_H__

#include "cocos2d.h"// brining in cocos funcality 
#include "Box2D/Box2D.h"
using namespace cocos2d; // namespace for cococs so you dont have to use cocos2d:: every time 

class GameScene : public cocos2d::Layer
{

private:
	Sprite* _player;
	Sprite* _player2;
	unsigned int score;
	//unsigned int count;
	unsigned int health;
	cocos2d::Label * scoreLabel;

	

public:
	static cocos2d::Scene* createScene();// creating the scene from here 
	virtual bool init();// creating the bool init calss 
	void addColision(float dt);
	void addMonster(float dt);// creating the monster class
	void addMonster2(float dt);
	void addPlayer();
	bool onTouchBegan(Touch * touch, Event *unused_event);
	bool onTouchBeganTower(Touch* touch, Event* event);
	void onTouchMovedTower(Touch* touch, Event* event);
	void onTouchEndedTower(Touch* touch, Event* event);
	bool onContactBegan(PhysicsContact &contact);
	
	void menuCloseCallback(cocos2d::Ref* pSender);
	bool scored;
	void SetIsScored();
	bool GetIsScored();
	void GoToMainMenuScene(Ref *sender);
	void GoToGameOverScene();
	//bool checkBoxCollision(CCSprite* box1, CCSprite* box2);
	CREATE_FUNC(GameScene);
	
};

#endif // __GAME_SCENE_H__