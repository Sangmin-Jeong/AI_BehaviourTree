#include "Enemy.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "EventManager.h"
#include "Game.h"
#include "Util.h"


Enemy::Enemy() : m_animationState(ENEMY_WALK_L), m_animationSpeed(0.8f), isRight(false)
{
	TextureManager::Instance().load("../Assets/textures/Circle.png","circle");

	const auto size = TextureManager::Instance().getTextureSize("circle");

	setWidth(48);
	setHeight(48);
	getRigidBody()->bounds = glm::vec2(getWidth(), getHeight());
	getTransform()->position = glm::vec2(500.0f, 100.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting motion properties
	setCurrentHeading(0.0f);// current facing angle
	setCurrentDirection(glm::vec2(-1.0f, 0.0f)); // facing right
	m_maxSpeed = 5.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 1.0f; // a maximum number of pixels to add to the velocity each frame

	setLOSDistance(200.0f); // Length of the middle ray.
	setLOSColour(glm::vec4(1, 0, 0, 1));


	// Patrol
	m_patrol.push_back(glm::vec2(150, 430 ));
	m_patrol.push_back(glm::vec2(600, 430 ));
	m_waypoint = 0;

	setTargetPosition(m_patrol[m_waypoint]);

	setType(AGENT);
}

Enemy::~Enemy()
= default;

void Enemy::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;
}

void Enemy::update()
{
	m_move();
	m_checkBounds();

	// Control Hit animation 
	if (getAnimation("hurt").current_frame == 2)
	{
		if (isRight == false)
		{
			setAnimationState(ENEMY_IDLE_L);
		}
		else if (isRight == true)
		{
			setAnimationState(ENEMY_IDLE_R);
		}
		getAnimation("hurt").current_frame = 0;
	}
}

void Enemy::clean()
{
}

void Enemy::drawEnemyLOS()
{
	// For detection radius
	int radius = (int)getLOSDistance();
	Util::DrawCircle(getTransform()->position, radius, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	Util::DrawLine(getTransform()->position, getTransform()->position + getCurrentDirection() * getLOSDistance(), getLOSColour());
}

void Enemy::m_move()
{
	Seek();

	//const float dt = TheGame::Instance().getDeltaTime();

	//// compute the position term
	//const glm::vec2 initial_position = getTransform()->position;

	//// compute the velocity term
	//const glm::vec2 velocity_term = getRigidBody()->velocity * dt;

	//// compute the acceleration term
	//const glm::vec2 acceleration_term = getRigidBody()->acceleration * 0.5f;// *dt;


	//// compute the new position
	//glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	//getTransform()->position = final_position;

	//// add our acceleration to velocity
	//getRigidBody()->velocity += getRigidBody()->acceleration;

	//// clamp our velocity at max speed
	//getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, 5);
}

void Enemy::m_checkBounds()
{
}

void Enemy::m_reset()
{
}

float Enemy::getMaxSpeed() const
{
	return m_maxSpeed;
}

float Enemy::getTurnRate() const
{
	return m_turnRate;
}

float Enemy::getAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 Enemy::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

void Enemy::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Enemy::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void Enemy::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void Enemy::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position);
}

void Enemy::Seek()
{
	// Find next waypoint:
	if (Util::distance(m_patrol[m_waypoint], getTransform()->position) <= 10)
	{
		if (++m_waypoint == m_patrol.size()) m_waypoint = 0;
		setTargetPosition(m_patrol[m_waypoint]);
		if (m_waypoint == 0)
		{
			setAnimationState(ENEMY_WALK_L);
		}
		else
		{
			setAnimationState(ENEMY_WALK_R);
		}
	}

	//setDesiredVelocity(getTargetPosition());

	//const glm::vec2 steering_direction = getDesiredVelocity() - getCurrentDirection();

	//LookWhereYoureGoing(steering_direction);

	//getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();
}

void Enemy::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	//if (target_direction.x > getCurrentDirection().x)
	//{
	//	setCurrentHeading(0.0f);
	//}
	//else if (target_direction.x < getCurrentDirection().x) // Target is the left side of ship
	//{
	//	setCurrentHeading(180.0f);
	//}
	//else if (target_direction.y > getCurrentDirection().y) // Target is the top side of ship
	//{
	//	setCurrentHeading(90.0f);
	//}
	//else if (target_direction.y < getCurrentDirection().y) // Target is the bottom side of ship
	//{
	//	setCurrentHeading(270.0f);
	//}

	//float target_rotation = Util::signedAngle(getCurrentDirection(), target_direction) - 90;

	//const float turn_sensitivity = 3.0f;

	//if (getCollisionWhiskers()[0])
	//{
	//	setCurrentHeading(getCurrentHeading() + getTurnRate());
	//}
	//else if (getCollisionWhiskers()[2])
	//{
	//	setCurrentHeading(getCurrentHeading() - getTurnRate());
	//}
	//else if (abs(target_rotation) > turn_sensitivity)
	//{
	//	if (target_rotation > 0.0f)
	//	{
	//		setCurrentHeading(getCurrentHeading() + getTurnRate());
	//	}
	//	else if (target_rotation < 0.0f)
	//	{
	//		setCurrentHeading(getCurrentHeading() - getTurnRate());
	//	}
	//}

	//updateWhiskers(getWhiskerAngle());
}
