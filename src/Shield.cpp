#include "Shield.h"
#include "SoundManager.h"
#include "TextureManager.h"

Shield::Shield(glm::vec2 position) : m_health(75)
{
	TextureManager::Instance().load("../Assets/textures/shield1.png", "shield1");
	TextureManager::Instance().load("../Assets/textures/shield2.png", "shield2");
	TextureManager::Instance().load("../Assets/textures/shield3.png", "shield3");

	auto size = TextureManager::Instance().getTextureSize("shield1");
	setWidth(size.x);
	setHeight(size.y);

	getTransform()->position.x = position.x;
	getTransform()->position.y = position.y;

	setType(OBSTACLE);
	getRigidBody()->isColliding = false;

	
}

Shield::~Shield()
= default;

void Shield::draw()
{
	if (getHealth() == 75)
	{
		TextureManager::Instance().draw("shield1",
			getTransform()->position.x, getTransform()->position.y, this, 0, 255, true);
	}
	else if (getHealth() == 50)
	{
		TextureManager::Instance().draw("shield2",
			getTransform()->position.x, getTransform()->position.y, this, 0, 255, true);
	}
	else if (getHealth() == 25)
	{
		TextureManager::Instance().draw("shield3",
			getTransform()->position.x, getTransform()->position.y, this, 0, 255, true);
	}

}

void Shield::update()
{
}

void Shield::clean()
{
}
