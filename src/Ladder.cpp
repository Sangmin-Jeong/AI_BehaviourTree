#include "Ladder.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "Util.h"

Ladder::Ladder()
{
	TextureManager::Instance().load("../Assets/textures/ladder.png", "ladder");

	auto size = TextureManager::Instance().getTextureSize("ladder");
	setWidth(40);
	setHeight(50);

	getTransform()->position = glm::vec2(400.0f, 300.0f);

	setType(LADDER);
	getRigidBody()->isColliding = false;
}

Ladder::~Ladder()
= default;

void Ladder::draw()
{
	TextureManager::Instance().draw("ladder",
		getTransform()->position.x, getTransform()->position.y, this, 0, 255, true);
}

void Ladder::update()
{
}

void Ladder::clean()
{
}
