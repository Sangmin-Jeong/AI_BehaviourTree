#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "EnemyAnimationState.h"
#include "Agent.h"

class Enemy : public Agent
{
public:
	Enemy();
	~Enemy();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// Animation
	void setAnimationSheet();
	void setAnimationState(EnemyAnimationState s) { m_animationState = s; }
	void setAnimationSpeed(float s) { m_animationSpeed = s; }
	EnemyAnimationState getAnimationState() { return m_animationState; }

	void drawEnemyLOS();

	// getters and setters (accessors and mutators)
	float getMaxSpeed() const;
	float getTurnRate() const;
	float getAccelerationRate() const;
	glm::vec2 getDesiredVelocity() const;

	void setMaxSpeed(float speed);
	void setTurnRate(float angle);
	void setAccelerationRate(float rate);
	void setDesiredVelocity(glm::vec2 target_position);

	// Patrol
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);

private:
	void m_move();
	void m_checkBounds();
	void m_reset();

	// Animation
	EnemyAnimationState m_animationState;
	float m_animationSpeed;

	// Patrol
	std::vector<glm::vec2> m_patrol;
	int m_waypoint;
	glm::vec2 m_desiredVelocity;

	// private movement variables
	bool isRight;
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;

};


#endif /* defined (__Enemy__) */