#pragma once
#include "GameObject.h"
#include "cocos2d.h"
#include "Hero.h"
#include<vector>
#include<string>

using namespace std;

class Level {
	public:
		Level(int levelNumber);
		~Level();
		int getScaleX();
		int getScaleY();
		string getBackgroundTexturePath();
		string getHeroTexturePath();
		Hero* getHero();

	private:
		int levelNumber;
		int scaleX;
		int scaleY;
		vector<GameObject*> objects;
		string backgroundTexturePath;
		string heroTexturePath;
		Hero* hero;
};

