#ifndef __LEVEL_2_SCENE_H__ // used so that hte header will only be used once per compile
#define __LEVEL_2_SCENE_H__

#include "cocos2d.h"// brining in cocos funcality 
#include "Box2D/Box2D.h"
#include "GameScene.h"
using namespace cocos2d; // namespace for cococs so you dont have to use cocos2d:: every time 

class Level_2Scene : public cocos2d::Layer
{

private:
	Sprite* _player;
	Sprite* _player2;
	Sprite* gameOverBox;
	unsigned int score;
	cocos2d::Label * scoreLabel;
	cocos2d::Label * livesLabel;
	int towerHp;
	bool gameOver;
	bool gameEnded;
public:
	static cocos2d::Scene* createScene();// creating the scene from here 
	virtual bool init();// creating the bool init calss 
	void addMonster(float dt);// creating the monster class
	bool onTouchBegan(Touch * touch, Event *unused_event);
	bool onContactBegan(PhysicsContact &contact);
	bool onContactBeganEndGame(PhysicsContact &contact);
	void menuCloseCallback(cocos2d::Ref* pSender);
	void doGameOver();
	bool scored;
	void SetIsScored();
	bool GetIsScored();
	void GoToMainMenuScene(Ref *sender);
	void GoToGameOverScene();

	CREATE_FUNC(Level_2Scene);

};

#endif // __LEVEL_2SCENE_H__