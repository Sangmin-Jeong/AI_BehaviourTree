#pragma once
#ifndef __PLAYERHEALTH__
#define __PLAYERHEALTH__
#include "Sprite.h"

class PlayerHealth : public Sprite
{
public:
	PlayerHealth();
	~PlayerHealth();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;



	// setters
	static void setPlayerHealth(int l) { m_playerHealth = l; }

	// getters
	static int getPlayerHealth() { return m_playerHealth; }

	// human life
	static inline int m_playerHealth;
	static inline int hitCount, maxCount;
	static void m_hit();

private:
	void m_move();
	void m_checkBounds();
	void m_reset();
};

#endif 