#pragma once
#ifndef __WEAPON__
#define __WEAPON__

#include "Agent.h"
#include "PlayerAnimationState.h"

class Weapon final : public Agent
{
public:
	Weapon(glm::vec2 p);
	~Weapon();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// getters and setters (accessors and mutators)
	float getMaxSpeed() const;
	float getTurnRate() const;
	float getAccelerationRate() const;
	glm::vec2 getDesiredVelocity() const;

	void setMaxSpeed(float speed);
	void setTurnRate(float angle);
	void setAccelerationRate(float rate);
	void setDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);

	void drawLOS();

	// Animation
	void setAnimationSheet();
	void setAnimationState(PlayerAnimationState s) { m_animationState = s; }
	void setAnimationSpeed(float s) { m_animationSpeed = s; }
	PlayerAnimationState getAnimationState() { return m_animationState; }

	void setIsMoving(bool b) { isMoving = b; }
	bool getIsMoving() { return isMoving; }

private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// Animation
	PlayerAnimationState m_animationState;
	float m_animationSpeed;
	bool isMoving;

	// private functions
	void m_move();
};


#endif /* defined (__WEAPON__) */