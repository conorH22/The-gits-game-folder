#ifndef __LEVEL2SPLASH_SCENE_H__
#define __LEVEL2SPLASH_SCENE_H__

#include "cocos2d.h"

class Level2SplashScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void GoToLevel2Scene(Ref * sender);
	// implement the "static create()" method manually
	CREATE_FUNC(Level2SplashScene);

private:


};

#endif // __LEVEL2SPLASH_SCENE_H__


