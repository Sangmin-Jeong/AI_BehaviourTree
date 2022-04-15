#include "InvisObstacle.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "Util.h"

InvisObstacle::InvisObstacle()
{
	TextureManager::Instance().load("../Assets/textures/pf.png", "InvisObstacle");

	auto size = TextureManager::Instance().getTextureSize("InvisObstacle");
	setWidth(size.x);
	setHeight(size.y);

	getTransform()->position = glm::vec2(400.0f, 300.0f);

	setType(OBSTACLE);
	getRigidBody()->isColliding = false;
}

InvisObstacle::~InvisObstacle()
= default;

void InvisObstacle::draw()
{
	//TextureManager::Instance().draw("InvisObstacle",
	//	getTransform()->position.x, getTransform()->position.y, this, 0, 255, true);
}

void InvisObstacle::update()
{
}

void InvisObstacle::clean()
{
}
