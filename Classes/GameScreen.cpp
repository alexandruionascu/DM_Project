#include "GameScreen.h"
#include "MoneyBag.h"
#include "EndLevelScreen.h"


USING_NS_CC;

int GameScreen::levelNumber = 0;

Scene* GameScreen::createScene(int levelNumner) {
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	
	// 'layer' is an autorelease object
	auto layer = GameScreen::create();

	// add layer as a child to scene
	scene->addChild(layer);

	GameScreen::levelNumber = levelNumber;

	// return the scene
	return scene;
}

/* void GameScreen::addBox(int i, int j) {
	auto box = Director::getInstance()->getTextureCache()->textureForKey("box.png");
	auto boxSprite = Sprite::createWithTexture(box);
	boxSprite->setPosition(i, j);
	boxSprite->setContentSize(Size(50, 50));
	background->addChild(boxSprite, 0);
} */

// on "init" you need to initialize your instance
bool GameScreen::init() {

	if (!Layer::init()) {
		return false;
	}



	level = new Level(1);
	renderer = new WorldRenderer(this, level);
	renderer->render();
	
	
	
	/*
	Size visibleSize = Director::getInstance()->getVisibleSize();

	
	Texture2D *bgTexture = Director::getInstance()->getTextureCache()->addImage("road.jpg");
	Director::getInstance()->getTextureCache()->addImage("box.png");
	
	const Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };

	// add "HelloWorld" splash screen"
	int scale = 5;
	background = Sprite::createWithTexture(bgTexture, CCRectMake(0, 0, scale * visibleSize.width, scale * visibleSize.height));
	

	background->getTexture()->setTexParameters(&tp);
	// position the sprite on the center of the screen
	background->setPosition(scale * visibleSize.width / 2, scale * visibleSize.height / 2);

	// add the sprite as a child to this layer
	this->addChild(background, 1);
	
	stance = "left";
	heroSprite = Sprite::create("police_car.png");
	heroSprite->setPosition(Vec2(scale * visibleSize.width / 2 + origin.x, scale * visibleSize.height / 2 + origin.y));
	heroSprite->setScale(0.25f);
	background->addChild(heroSprite, 0);


	for (int i = 0; i <= 20; i++) {
		addBox(100 * i, 0);
	}
	*/

	auto eventListener = EventListenerKeyboard::create();


	eventListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {

		if (keys.find(keyCode) == keys.end()){
			keys[keyCode] = std::chrono::high_resolution_clock::now();
		}


		if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) {
			if (level->getHero()->getMoney() >= level->getRequiredMoney()) {
				auto moneyBag = new MoneyBag("money_bag.png", renderer->getHeroSprite()->getPosition().x, renderer->getHeroSprite()->getPosition().y, 1, 1);
				moneyBag->render(renderer->getBackground());
				level->getHero()->setMoney(level->getHero()->getMoney() - level->getRequiredMoney());
			}
			
		}
	};

	eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){
		keys.erase(keyCode);
	};


	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, renderer->getHeroSprite());

	//adds contact event listener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScreen::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener,
		this);

	schedule(CC_SCHEDULE_SELECTOR(GameScreen::tick), 0.3f);
	
	this->scheduleUpdate();

	
	return true;
}

bool GameScreen::onContactBegin(PhysicsContact& contact) {

	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	//prevent police and money to collide
	if (contact.getShapeA()->getContactTestBitmask() == 0x02 && contact.getShapeB()->getContactTestBitmask()== 0x05) {
		return false;
	}

	if (contact.getShapeB()->getContactTestBitmask() == 0x02 && contact.getShapeA()->getContactTestBitmask() == 0x05) {
		return false;
	}

	//hero and money contact
	if (contact.getShapeA()->getContactTestBitmask() == 0x01 && contact.getShapeB()->getContactTestBitmask() == 0x05) {
		float newX, newY;
		int x = random(1, (int)100 * level->getScaleX());
		int y = random(1, (int)100 * level->getScaleY());
		level->convert(x, y, newX, newY);
		contact.getShapeB()->getBody()->getNode()->setPosition(Vec2(newX, newY));
		level->getHero()->setMoney(level->getHero()->getMoney() + 1);
	}

	if (contact.getShapeB()->getContactTestBitmask() == 0x01 && contact.getShapeA()->getContactTestBitmask() == 0x05) {
		float newX, newY;
		int x = random(1, (int)100 * level->getScaleX());
		int y = random(1, (int)100 * level->getScaleY());
		level->convert(x, y, newX, newY);
		contact.getShapeA()->getBody()->getNode()->setPosition(Vec2(newX, newY));
		level->getHero()->setMoney(level->getHero()->getMoney() + 1);
	}

	//if money bag and police car contact
	if (contact.getShapeA()->getContactTestBitmask() == 0x02 && contact.getShapeB()->getContactTestBitmask() == 0x06) {
	

		level->erasePoliceCar(contact.getShapeA()->getBody()->getNode()->getTag());
		
		contact.getShapeA()->getBody()->getNode()->removeFromParent();
		contact.getShapeB()->getBody()->getNode()->removeFromParent();
	}

	if (contact.getShapeB()->getContactTestBitmask() == 0x02 && contact.getShapeA()->getContactTestBitmask() == 0x06) {

		level->erasePoliceCar(contact.getShapeB()->getBody()->getNode()->getTag());

		contact.getShapeB()->getBody()->getNode()->removeFromParent();
		contact.getShapeA()->getBody()->getNode()->removeFromParent();
	}

	//if hero and police colide

	if (contact.getShapeA()->getContactTestBitmask() == 0x01 && contact.getShapeB()->getContactTestBitmask() == 0x02) {
		level->setLife(level->getLife() - 1);
	}

	if (contact.getShapeB()->getContactTestBitmask() == 0x01 && contact.getShapeA()->getContactTestBitmask() == 0x02) {
		level->setLife(level->getLife() - 1);
	}


	if (level->getLife() <= 0) {
		auto endLevelScreen = EndLevelScreen::createScene(false);
		Director::getInstance()->replaceScene(endLevelScreen);
	}

	if (level->getPoliceCars().size() == 0) {

		auto endLevelScreen = EndLevelScreen::createScene(true);
		Director::getInstance()->replaceScene(endLevelScreen);
	}

	

	/*if (contact.getShapeA()->getCollisionBitmask() == 0x01) {
		contact.getShapeA()->getBody()->getNode()->getParent()->runAction(FadeIn::create(0.5f));
		contact.getShapeA()->getBody()->getNode()->setPosition(contact.getShapeA()->getBody()->getNode()->getPosition().x + 50, contact.getShapeB()->getBody()->getNode()->getPosition().y + 50);
		nodeA->setRotation(90);
	}
	else contact.getShapeB()->getBody()->getNode()->runAction(FadeIn::create(0.3f));
	contact.getShapeA()->getBody()->getNode()->setPosition(2000, 2000);
	*/

	//bodies can collide
	return true;
}

void GameScreen::tick(float dt) {

}

bool GameScreen::isKeyPressed(EventKeyboard::KeyCode code) {
	// Check if the key is currently pressed by seeing it it's in the std::map keys
	// In retrospect, keys is a terrible name for a key/value paried datatype isnt it?
	if (keys.find(code) != keys.end())
		return true;
	return false;
}

double GameScreen::keyPressedDuration(EventKeyboard::KeyCode code) {
	if (!isKeyPressed(EventKeyboard::KeyCode::KEY_CTRL))
		return 0;  // Not pressed, so no duration obviously

	// Return the amount of time that has elapsed between now and when the user
	// first started holding down the key in milliseconds
	// Obviously the start time is the value we hold in our std::map keys
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::high_resolution_clock::now() - keys[code]).count();
}

void GameScreen::update(float delta) {

	auto heroSprite = renderer->getHeroSprite();

	Camera::getDefaultCamera()->setPosition(heroSprite->getPosition());

	int rotateSpeed = 300;
	int amount = 1200;

	auto position = heroSprite->getPosition();

	if (isKeyPressed(EventKeyboard::KeyCode::KEY_A)) {
		heroSprite->setRotation((int)(heroSprite->getRotation() - rotateSpeed * delta) % 360);
	}

	
	if (isKeyPressed(EventKeyboard::KeyCode::KEY_D)) {
		heroSprite->setRotation((int)(heroSprite->getRotation() + rotateSpeed * delta) % 360);
	}

	
	float angle = -1.0f * heroSprite->getRotation();
	position.x += (amount - 50 * level->getHero()->getMoney())  * cos(angle * atan(1) * 4 / 180) * delta;
	position.y += (amount - 50 * level->getHero()->getMoney()) * sin(angle * atan(1) * 4 / 180) * delta;

	


	heroSprite->setPosition(position);
	
	float policeDelta = random(0,200) + random(0,100);
	auto policeCars = level->getPoliceCars();
	int policeSpeed = 1000;
	for (int i = 0; i < policeCars.size(); i++) {
		auto policePosition = policeCars[i]->getPoliceSprite()->getPosition(); 
		float angle = atan2(position.y - policePosition.y, position.x-policePosition.x );
		policePosition.x += (policeSpeed - policeDelta) * cos(angle) * delta;
		policePosition.y += (policeSpeed - policeDelta) * sin(angle) * delta;
		
		policeCars[i]->getPoliceSprite()->setPosition(policePosition);
		policeCars[i]->getPoliceSprite()->setRotation(-angle * 180 / (atan(1) * 4));
	}
	

	Camera::getDefaultCamera()->setPosition(heroSprite->getPosition());

	
		
		
}

map<cocos2d::EventKeyboard::KeyCode, chrono::high_resolution_clock::time_point> GameScreen::keys;
	

