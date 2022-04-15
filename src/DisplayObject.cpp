#include "DisplayObject.h"

DisplayObject::DisplayObject()
= default;

DisplayObject::~DisplayObject()
= default;

Scene * DisplayObject::getParent() const
{
	return m_pParentScene;
}

void DisplayObject::setParent(Scene * parent)
{
	m_pParentScene = parent;
}

uint32_t DisplayObject::getLayerIndex() const
{
	return m_layerIndex;
}
uint32_t DisplayObject::getLayerOrderIndex() const
{
	return m_layerOrderIndex;
}

void DisplayObject::setLayerIndex(const uint32_t new_index, const uint32_t new_order)
{
	m_layerIndex = new_index;
	m_layerOrderIndex = new_order;
}

SpriteSheet* DisplayObject::getSpriteSheet()
{
	return m_pSpriteSheet;
}

Animation& DisplayObject::getAnimation(const std::string& name)
{
	return m_pAnimations[name];
}

void DisplayObject::setSpriteSheet(SpriteSheet* sprite_sheet)
{
	m_pSpriteSheet = sprite_sheet;
}

void DisplayObject::setAnimation(const Animation& animation)
{
	if (!m_animationExists(animation.name))
	{
		m_pAnimations[animation.name] = animation;
	}
}

bool DisplayObject::m_animationExists(const std::string& id)
{
	return m_pAnimations.find(id) != m_pAnimations.end();
}
