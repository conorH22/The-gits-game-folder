#ifndef __LEVEL3SPLASH_SCENE_H__
#define __LEVEL3SPLASH_SCENE_H__

#include "cocos2d.h"

class Level3SplashScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void GoToLevel3Scene(Ref * sender);
	// implement the "static create()" method manually
	CREATE_FUNC(Level3SplashScene);

private:


};

#endif // __LEVEL3SPLASH_SCENE_H__

