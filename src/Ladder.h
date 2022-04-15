#pragma once
#ifndef __LADDER__
#define __LADDER__
#include "DisplayObject.h"

class Ladder final : public DisplayObject
{
public:
	// constructors
	Ladder();
	
	// destructor
	~Ladder();
	
	// life cycle functions
	void draw() override;
	void update() override;
	void clean() override;
private:
	
};

#endif /* defined (__LADDER__) */