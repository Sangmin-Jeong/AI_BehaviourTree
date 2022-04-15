#include "EnemyHealth.h"
#include <Windows.h>
#include "TextureManager.h"


EnemyHealth::EnemyHealth()
{
	TextureManager::Instance().load("../Assets/textures/EnemyHeart.png", "Enemylife");
	TextureManager::Instance().load("../Assets/textures/EnemyHeart.png", "Enemylife2");
	TextureManager::Instance().load("../Assets/textures/EnemyHeart.png", "Enemylife3");
	TextureManager::Instance().load("../Assets/textures/EnemyHeart.png", "Enemylife4");

	const auto size = TextureManager::Instance().getTextureSize("life");

	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(30.0f, 30.0f);

	setEnemyHealth(4);

}

EnemyHealth::~EnemyHealth()
= default;

void EnemyHealth::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	switch (getEnemyHealth())
	{
	case 1:
		TextureManager::Instance().draw("Enemylife", x*23, y, 0, 255, true);
		break;
	case 2:
		TextureManager::Instance().draw("Enemylife", x * 23, y, 0, 255, true);
		TextureManager::Instance().draw("Enemylife2", x * 24, y, 0, 255, true);
		break;
	case 3:
		TextureManager::Instance().draw("Enemylife", x * 23, y, 0, 255, true);
		TextureManager::Instance().draw("Enemylife2", x * 24, y, 0, 255, true);
		TextureManager::Instance().draw("Enemylife3", x * 25, y, 0, 255, true);
		break;
	case 4:
		TextureManager::Instance().draw("Enemylife", x * 23, y, 0, 255, true);
		TextureManager::Instance().draw("Enemylife2", x * 24, y, 0, 255, true);
		TextureManager::Instance().draw("Enemylife3", x * 25, y, 0, 255, true);
		TextureManager::Instance().draw("Enemylife4", x * 26, y, 0, 255, true);
		break;
	}

}

void EnemyHealth::update()
{

	hitCount++;

}

void EnemyHealth::clean()
{
}

void EnemyHealth::m_move()
{
	
}

void EnemyHealth::m_checkBounds()
{
}

void EnemyHealth::m_reset()
{
}

void EnemyHealth::m_hit()   // for invincible time
{
	if (hitCount++ >= maxCount)
	{
		setEnemyHealth(getEnemyHealth() - 1);
		maxCount = 180;
		hitCount = 0;
	}
}
