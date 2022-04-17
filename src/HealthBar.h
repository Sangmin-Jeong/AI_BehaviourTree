#pragma once
#ifndef  _HEALTHBAR_H_
#define  _HEALTHBAR_H_
#include "GameObject.h"
#include "Agent.h"
#include <SDL/include/SDL_rect.h>

class HealthBar : public GameObject
{
private:
	SDL_FRect m_bar;
	// using m_dst from gameobject as background bar
	int m_health;

public:
	HealthBar(SDL_FRect pos, int maxHealth);
	void Update();
	void Render();
	void TakeDamage(int amount);
};


#endif // ! _HEALTHBAR_H_