#include "PlayerHealth.h"
#include <Windows.h>
#include "TextureManager.h"


PlayerHealth::PlayerHealth()
{
	TextureManager::Instance().load("../Assets/textures/heart.png", "life");
	TextureManager::Instance().load("../Assets/textures/heart.png", "life2");
	TextureManager::Instance().load("../Assets/textures/heart.png", "life3");
	TextureManager::Instance().load("../Assets/textures/heart.png", "life4");

	const auto size = TextureManager::Instance().getTextureSize("life");

	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(30.0f, 30.0f);

	setPlayerHealth(4);

}

PlayerHealth::~PlayerHealth()
= default;

void PlayerHealth::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	switch (getPlayerHealth())
	{
	case 1:
		TextureManager::Instance().draw("life", x, y, 0, 255, true);
		break;
	case 2:
		TextureManager::Instance().draw("life", x, y, 0, 255, true);
		TextureManager::Instance().draw("life2", x*2, y, 0, 255, true);
		break;
	case 3:
		TextureManager::Instance().draw("life", x, y, 0, 255, true);
		TextureManager::Instance().draw("life2", x*2, y, 0, 255, true);
		TextureManager::Instance().draw("life3", x*3, y, 0, 255, true);
		break;
	case 4:
		TextureManager::Instance().draw("life", x, y, 0, 255, true);
		TextureManager::Instance().draw("life2", x*2, y, 0, 255, true);
		TextureManager::Instance().draw("life3", x*3, y, 0, 255, true);
		TextureManager::Instance().draw("life4", x*4, y, 0, 255, true);
		break;
	}

}

void PlayerHealth::update()
{

	hitCount++;

}

void PlayerHealth::clean()
{
}

void PlayerHealth::m_move()
{
	
}

void PlayerHealth::m_checkBounds()
{
}

void PlayerHealth::m_reset()
{
}

void PlayerHealth::m_hit()   // for invincible time
{
	if (hitCount++ >= maxCount)
	{
		setPlayerHealth(getPlayerHealth() - 1);
		maxCount = 180;
		hitCount = 0;
	}
}
