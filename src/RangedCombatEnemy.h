#pragma once
#ifndef __RANGED_COMBAT_ENEMY__
#define __RANGED_COMBAT_ENEMY__

#include "DecisionTree.h"
#include "Enemy.h"
#include "Scene.h"

class RangedCombatEnemy final : public Enemy
{
public:
	RangedCombatEnemy(Scene* scene);
	~RangedCombatEnemy();

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

	// New action overrides
	void Patrol();
	void MoveToPlayer();
	void Flee();
	void MovetoCover();
	void Attack();
	void WaitBehindCover();
	void MoveToRange();
	void MoveToLOS();
	// more later...

	//const DecisionTree* getTree() { return m_tree; }

private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;
	// new for Lab 7
	std::vector<glm::vec2> m_patrol;
	int m_waypoint;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private functions
	void m_move();

	// Decision tree
	/*DecisionTree* m_tree;*/
	void m_buildTree() override;

	int m_fireCtr, m_fireCtrMax;
	Scene* m_pScene;

};
#endif /* defined (__RANGED_COMBAT_ENEMY__) */
