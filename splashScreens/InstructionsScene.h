#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "cocos2d.h"

class InstructionsScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(InstructionsScene);

private:
	void goToGameScene(float dt);

};

#endif // __SPLASH_SCENE_H__

