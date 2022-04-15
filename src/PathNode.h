#pragma once
#ifndef __PATH__NODE__
#define __PATH__NODE__
#include "Agent.h"

class PathNode final : public Agent
{
public:
	// constructors
	PathNode();

	// destructor
	~PathNode();

	// life cycle functions
	void draw() override;
	void update() override;
	void clean() override;
private:

};

#endif /* defined (__PATH__NODE__) */