#pragma once
#ifndef __SPACE_SHIP__
#define __SPACE_SHIP__

#include "PlayerAnimationState.h"
#include "Sprite.h"

class Player final : public Sprite
{
public:
	Player();
	~Player();

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

	void setCanDown(bool b) { m_CanDown = b; }
	void setCanUp(bool b) { m_CanUp = b; }

	bool getCanDown() { return m_CanDown; }
	bool getCanUp() { return m_CanUp; }

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);

	void drawLOS();

	// Animation
	void setAnimationSheet();
	void setAnimationState(PlayerAnimationState s) { m_animationState = s; }
	void setAnimationSpeed(float s) { m_animationSpeed = s; }
	PlayerAnimationState getAnimationState() { return m_animationState; }

private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;
	bool m_CanDown;
	bool m_CanUp;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// Animation
	PlayerAnimationState m_animationState;
	float m_animationSpeed;

	// private functions
	void m_move();
};


#endif /* defined (__SPACE_SHIP__) */