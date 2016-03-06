#ifndef __LEVEL_1_SPLASH_SCENE_H__
#define __LEVEL_1_SPLASH_SCENE_H__

#include "cocos2d.h"

class Level1SplashScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void GoToGameScene(Ref * sender);

    // implement the "static create()" method manually
	CREATE_FUNC(Level1SplashScene);

private:
	

};

#endif // __SPLASH_SCENE_H__
