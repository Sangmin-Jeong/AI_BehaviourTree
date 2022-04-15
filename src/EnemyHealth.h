#pragma once
#ifndef __ENEMYHEALTH__
#define __ENEMYHEALTH__
#include "Sprite.h"

class EnemyHealth : public Sprite
{
public:
	EnemyHealth();
	~EnemyHealth();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;



	// setters
	static void setEnemyHealth(int l) { m_EnemyHealth = l; }

	// getters
	static int getEnemyHealth() { return m_EnemyHealth; }

	// human life
	static inline int m_EnemyHealth;
	static inline int hitCount, maxCount;
	static void m_hit();

private:
	void m_move();
	void m_checkBounds();
	void m_reset();
};

#endif 