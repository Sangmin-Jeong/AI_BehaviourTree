#include "Weapon.h"
#include "EventManager.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

Weapon::Weapon(glm::vec2 p) : isMoving(false), m_animationSpeed(0.8f)
{
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/weaponSprites/weapon.txt",
		"../Assets/sprites/weaponSprites/dagger_red.png",
		"dagger");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("dagger"));

	setWidth(32);
	setHeight(32);

	getTransform()->position.x = p.x;
	getTransform()->position.y = p.y;

	getRigidBody()->bounds = glm::vec2(getWidth(), getHeight());
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting motion properties
	setCurrentHeading(0.0f); 
	setCurrentDirection(glm::vec2(1.0f, 0.0f));
	m_maxSpeed = 30.0f; // a maximum number of pixels moved per frame
	m_turnRate = 50.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 20.0f; // a maximum number of pixels to add to the velocity each frame
	
	setLOSDistance(200.0f); // Length of the middle ray.
	setLOSColour(glm::vec4(1, 0, 0, 1));

	// Animation
	setAnimationSheet();

	setType(AGENT);
}

Weapon::~Weapon()
= default;

void Weapon::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	/*TextureManager::Instance().draw("space_ship", x, y, getCurrentHeading(), 255, isCentered());*/

	// draw LOS
	// Util::DrawLine(getTransform()->position, getTransform()->position + getCurrentDirection() * getLOSDistance(), getLOSColour());
	switch (isMoving)
	{
	case true:
		TextureManager::Instance().playAnimationDST("dagger", getAnimation("dagger"),
			x, y, (int)getDst().w, (int)getDst().h, m_animationSpeed, 0, 255, true);
		break;
	default:
		break;
	}

	/*	case true:
			TextureManager::Instance().playAnimation("dagger", getAnimation("dagger"),
				x, y, m_animationSpeed, 0, 255, true);
			break;*/
}

void Weapon::setAnimationSheet()
{
	Animation dagger = Animation();
	dagger.name = "dagger";
	dagger.frames.push_back(getSpriteSheet()->getFrame("DAGGER-0"));
	dagger.frames.push_back(getSpriteSheet()->getFrame("DAGGER-1"));
	dagger.frames.push_back(getSpriteSheet()->getFrame("DAGGER-2"));
	dagger.frames.push_back(getSpriteSheet()->getFrame("DAGGER-3"));
	setAnimation(dagger);
}

void Weapon::update()
{
	m_move();
}

void Weapon::clean()
{
}

float Weapon::getMaxSpeed() const
{
	return m_maxSpeed;
}

float Weapon::getTurnRate() const
{
	return m_turnRate;
}

float Weapon::getAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 Weapon::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

void Weapon::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Weapon::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void Weapon::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void Weapon::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position);
}

void Weapon::Seek()
{
	setDesiredVelocity(getTargetPosition());

	const glm::vec2 steering_direction = getDesiredVelocity() - getCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();
}

void Weapon::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	float target_rotation = Util::signedAngle(getCurrentDirection(), target_direction) - 90;

	const float turn_sensitivity = 3.0f;

	if (getCollisionWhiskers()[0])
	{
		setCurrentHeading(getCurrentHeading() + getTurnRate());
	}
	else if (getCollisionWhiskers()[2])
	{
		setCurrentHeading(getCurrentHeading() - getTurnRate());
	}
	else if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			setCurrentHeading(getCurrentHeading() + getTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			setCurrentHeading(getCurrentHeading() - getTurnRate());
		}
	}

	/*updateWhiskers(getWhiskerAngle());*/
}

void Weapon::drawLOS()
{

}


void Weapon::m_move()
{
	Seek();
	
	//                                   final Position     position term    velocity term     acceleration term
	// kinematic equation for motion --> Pf            =      Pi     +     Vi*(time)    +   (0.5)*Ai*(time * time)

	const float dt = TheGame::Instance().getDeltaTime();

	// compute the position term
	const glm::vec2 initial_position = getTransform()->position;

	// compute the velocity term
	const glm::vec2 velocity_term = getRigidBody()->velocity * dt;

	// compute the acceleration term
	const glm::vec2 acceleration_term = getRigidBody()->acceleration * 0.5f;// *dt;
	
	
	// compute the new position
	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	getTransform()->position = final_position;

	// add our acceleration to velocity
	getRigidBody()->velocity += getRigidBody()->acceleration;

	// clamp our velocity at max speed
	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, getMaxSpeed());
}
