#include "HealthBar.h"

HealthBar::HealthBar(SDL_FRect pos, int maxHealth) : m_health(maxHealth)
{
	m_dst = pos;
	m_bar = pos;

}

void HealthBar::Update()
{
	Render();
}

void HealthBar::Render()
{
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 0, 255);
	SDL_RenderFillRectF(Renderer::Instance().getRenderer(), &getDst());

	double percent = (m_health / 100.0);

	m_bar.w = 50 * percent; // z-min/max-min -> % in a range
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255 * (1 - percent), 255 * percent, 0, 255);
	SDL_RenderFillRectF(Renderer::Instance().getRenderer(), &m_bar);

	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawRectF(Renderer::Instance().getRenderer(), &m_bar);
}

void HealthBar::TakeDamage(int amount)
{
	m_health -= amount;
	if (m_health < 0) m_health = 0;
	if (m_health > 100) m_health = 100;
}
