#include "GameScene.h"

b2World* world;
b2Vec2 gravity;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(GameScene::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);
    
    auto homeButton = MenuItemImage::create("gameButton.png", "gameButton.png",
                                            CC_CALLBACK_1(GameScene::homeButtonCallback, this));
    homeButton->setPosition(100,200);
    auto homeButtonMenu = Menu::create(homeButton, NULL);
    homeButtonMenu->setPosition(Point::ZERO);
    this->addChild(homeButtonMenu);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    gravity.Set(0.0f, -(9.8f * 32.0f));
    bool doSleep = true;
    
    world = new b2World(gravity);
    world->SetAllowSleeping(doSleep);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::touchBegan, this);
    getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 100);
    
    schedule(schedule_selector(GameScene::tick));
    
    return true;
}

void GameScene::homeButtonCallback(cocos2d::Ref* pSender)
{
    Director::getInstance()->popScene();
}

bool GameScene::touchBegan(Touch* touch, Event* event)
{
    GameScene::createStar(touch->getLocation());
    return true;
}

void GameScene::createStar(Point p)
{
    auto starSprite = Sprite::create("star.png");
    starSprite->setPosition(p);
    
    this->addChild(starSprite, 0);
    
    b2BodyDef starBodyDef;
    starBodyDef.type = b2_dynamicBody;
    starBodyDef.position = b2Vec2(starSprite->getPositionX(),starSprite->getPositionY());
    starBodyDef.userData = starSprite;
    
    auto starBody = world->CreateBody(&starBodyDef);
    b2CircleShape circle;
    circle.m_radius = 0.6 * 32;
    
    b2FixtureDef starFixtureDef;
    starFixtureDef.shape = &circle;
    starFixtureDef.density = 1.0f;
    starFixtureDef.friction = 0.6f;
    starFixtureDef.restitution = 0.8f;
    
    starBody->CreateFixture(&starFixtureDef);
}

void GameScene::tick(float dt)
{
    int velocityIterations = 8;
    int positionIterations = 3;
    
    world->Step(dt, velocityIterations, positionIterations);
    
    for(auto b = world->GetBodyList(); b; b = b->GetNext())
    {
        if(b->GetUserData() != NULL)
        {
            auto myActor = (Sprite*) b->GetUserData();
            myActor->setPosition(Point(b->GetPosition().x, b->GetPosition().y));
            myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
            
            if(myActor->getPositionY() < -32 || myActor->getPositionX() < -32 ||
               myActor->getPositionX() > Director::getInstance()->getVisibleSize().width + 32)
            {
                removeChild(myActor);
                world->DestroyBody(b);
            }
        }
    }
}


void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
