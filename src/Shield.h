#pragma once
#ifndef __SHIELD__
#define __SHIELD__
#include "DisplayObject.h"

class Shield final : public DisplayObject
{
public:
	// constructors
	Shield(glm::vec2 position);
	
	// destructor
	~Shield();
	
	// life cycle functions
	void draw() override;
	void update() override;
	void clean() override;

	int getHealth() { return m_health; }
	void setHealth(int h) { m_health = h; }

private:

	int m_health;
};

#endif /* defined (__SHIELD__) */