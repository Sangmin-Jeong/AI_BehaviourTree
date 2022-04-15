#pragma once
#ifndef __INVIS_InvisObstacle__
#define __INVIS_InvisObstacle__
#include "DisplayObject.h"

class InvisObstacle final : public DisplayObject
{
public:
	// constructors
	InvisObstacle();
	
	// destructor
	~InvisObstacle();
	
	// life cycle functions
	void draw() override;
	void update() override;
	void clean() override;
private:
	
};

#endif /* defined (__InvisObstacle__) */