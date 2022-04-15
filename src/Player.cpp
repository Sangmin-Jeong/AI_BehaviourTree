#include "Player.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

Player::Player() : m_animationState(PLAYER_IDLE_R), m_animationSpeed(1.0f)
{
	//TextureManager::Instance().load("../Assets/textures/ncl_small.png", "space_ship");

	//const auto size = TextureManager::Instance().getTextureSize("space_ship");
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/playerSprites/player.txt",
		"../Assets/sprites/playerSprites/GraveRobber_idle.png",
		"playerIdle");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerIdle"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/playerSprites/player.txt",
		"../Assets/sprites/playerSprites/GraveRobber_run.png",
		"playerRun");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerRun"));

	//TextureManager::Instance().loadSpriteSheet(
	//	"../Assets/sprites/playerSprites/player.txt",
	//	"../Assets/sprites/playerSprites/GraveRobber_jump.png",
	//	"playerJump");
	//setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerJump"));
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/playerSprites/player.txt",
		"../Assets/sprites/playerSprites/GraveRobber_climb.png",
		"playerClimb");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerClimb"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/playerSprites/player.txt",
		"../Assets/sprites/playerSprites/GraveRobber_climb.png",
		"playerClimb");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerClimb"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/playerSprites/player.txt",
		"../Assets/sprites/playerSprites/GraveRobber_attack1.png",
		"playerMelee");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerMelee"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/playerSprites/player.txt",
		"../Assets/sprites/playerSprites/GraveRobber_attack2.png",
		"playerRange");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerRange"));

	setWidth(48);
	setHeight(48);

	getRigidBody()->bounds = glm::vec2(getWidth(), getHeight());
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;
	getTransform()->position = glm::vec2(110.0f, 600.0f);

	setCanDown(false);
	setCanUp(false);

	// starting motion properties
	setCurrentHeading(0.0f);
	setCurrentDirection(glm::vec2(1.0f, 0.0f));
	m_maxSpeed = 20.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 4.0f; // a maximum number of pixels to add to the velocity each frame

	setLOSDistance(200.0f); // Length of the middle ray.
	setLOSColour(glm::vec4(1, 0, 0, 1));

	// Animation
	setAnimationSheet();

	setType(AGENT);
}

Player::~Player()
= default;

void Player::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	/*TextureManager::Instance().draw("space_ship", x, y, getCurrentHeading(), 255, isCentered());*/

	// draw LOS
	// Util::DrawLine(getTransform()->position, getTransform()->position + getCurrentDirection() * getLOSDistance(), getLOSColour());
	switch (m_animationState)
	{
	case PLAYER_IDLE_L:
		TextureManager::Instance().playAnimation("playerIdle", getAnimation("idle"),
			x, y, m_animationSpeed, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PLAYER_IDLE_R:
		TextureManager::Instance().playAnimation("playerIdle", getAnimation("idle"),
			x, y, m_animationSpeed, 0, 255, true);
		break;
	case PLAYER_RUN_L:
		TextureManager::Instance().playAnimation("playerRun", getAnimation("run"),
			x, y, m_animationSpeed, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PLAYER_RUN_R:
		TextureManager::Instance().playAnimation("playerRun", getAnimation("run"),
			x, y, m_animationSpeed, 0, 255, true);
		break;
		//case PLAYER_JUMP_R:
		//	TextureManager::Instance().playAnimation("playerJump", getAnimation("jump"),
		//		x, y, m_animationSpeed, 0, 255, true);
		//	break;
		//case PLAYER_JUMP_L:
		//	TextureManager::Instance().playAnimation("playerJump", getAnimation("jump"),
		//		x, y, m_animationSpeed, 0, 255, true, SDL_FLIP_HORIZONTAL);
		//	break;
	case PLAYER_CLIMB_L:
		TextureManager::Instance().playAnimation("playerClimb", getAnimation("climb"),
			x, y, m_animationSpeed, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PLAYER_CLIMB_R:
		TextureManager::Instance().playAnimation("playerClimb", getAnimation("climb"),
			x, y, m_animationSpeed, 0, 255, true);
		break;
	case PLAYER_MELEE_L:
		TextureManager::Instance().playAnimation("playerMelee", getAnimation("melee"),
			x, y, m_animationSpeed, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PLAYER_MELEE_R:
		TextureManager::Instance().playAnimation("playerMelee", getAnimation("melee"),
			x, y, m_animationSpeed, 0, 255, true);
		break;
	case PLAYER_RANGE_L:
		TextureManager::Instance().playAnimation("playerRange", getAnimation("range"),
			x, y, m_animationSpeed, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PLAYER_RANGE_R:
		TextureManager::Instance().playAnimation("playerRange", getAnimation("range"),
			x, y, m_animationSpeed, 0, 255, true);
		break;
	default:
		break;
	}

}

void Player::setAnimationSheet()
{
	Animation idle = Animation();
	idle.name = "idle";
	idle.frames.push_back(getSpriteSheet()->getFrame("PLAYER-IDLE-0"));
	idle.frames.push_back(getSpriteSheet()->getFrame("PLAYER-IDLE-1"));
	idle.frames.push_back(getSpriteSheet()->getFrame("PLAYER-IDLE-2"));
	idle.frames.push_back(getSpriteSheet()->getFrame("PLAYER-IDLE-3"));
	setAnimation(idle);

	Animation run = Animation();
	run.name = "run";
	run.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RUN-0"));
	run.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RUN-1"));
	run.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RUN-2"));
	run.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RUN-3"));
	run.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RUN-4"));
	run.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RUN-5"));
	setAnimation(run);

	//Animation jump = Animation();
	//jump.name = "jump";
	//jump.frames.push_back(getSpriteSheet()->getFrame("PLAYER-JUMP-0"));
	//jump.frames.push_back(getSpriteSheet()->getFrame("PLAYER-JUMP-1"));
	//jump.frames.push_back(getSpriteSheet()->getFrame("PLAYER-JUMP-2"));
	//jump.frames.push_back(getSpriteSheet()->getFrame("PLAYER-JUMP-3"));
	//jump.frames.push_back(getSpriteSheet()->getFrame("PLAYER-JUMP-4"));
	//jump.frames.push_back(getSpriteSheet()->getFrame("PLAYER-JUMP-5"));
	//setAnimation(jump);

	Animation climb = Animation();
	climb.name = "climb";
	climb.frames.push_back(getSpriteSheet()->getFrame("PLAYER-CLIMB-0"));
	climb.frames.push_back(getSpriteSheet()->getFrame("PLAYER-CLIMB-1"));
	climb.frames.push_back(getSpriteSheet()->getFrame("PLAYER-CLIMB-2"));
	climb.frames.push_back(getSpriteSheet()->getFrame("PLAYER-CLIMB-3"));
	climb.frames.push_back(getSpriteSheet()->getFrame("PLAYER-CLIMB-4"));
	climb.frames.push_back(getSpriteSheet()->getFrame("PLAYER-CLIMB-5"));
	setAnimation(climb);

	Animation melee = Animation();
	melee.name = "melee";
	melee.frames.push_back(getSpriteSheet()->getFrame("PLAYER-MELEE-0"));
	melee.frames.push_back(getSpriteSheet()->getFrame("PLAYER-MELEE-1"));
	melee.frames.push_back(getSpriteSheet()->getFrame("PLAYER-MELEE-2"));
	melee.frames.push_back(getSpriteSheet()->getFrame("PLAYER-MELEE-3"));
	melee.frames.push_back(getSpriteSheet()->getFrame("PLAYER-MELEE-4"));
	melee.frames.push_back(getSpriteSheet()->getFrame("PLAYER-MELEE-5"));
	setAnimation(melee);

	Animation range = Animation();
	range.name = "range";
	range.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RANGE-0"));
	range.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RANGE-1"));
	range.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RANGE-2"));
	range.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RANGE-3"));
	range.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RANGE-4"));
	range.frames.push_back(getSpriteSheet()->getFrame("PLAYER-RANGE-5"));
	setAnimation(range);
}

void Player::drawLOS()
{
	// For detection radius
	int radius = (int)getLOSDistance();
	Util::DrawCircle(getTransform()->position, radius, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	Util::DrawLine(getTransform()->position, getTransform()->position + getCurrentDirection() * getLOSDistance(), getLOSColour());
}

void Player::update()
{
	setMiddleLOSEndPoint(getTransform()->position + getCurrentDirection() * getLOSDistance());
}

void Player::clean()
{
}

float Player::getMaxSpeed() const
{
	return m_maxSpeed;
}

float Player::getTurnRate() const
{
	return m_turnRate;
}

float Player::getAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 Player::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

void Player::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Player::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void Player::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void Player::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position);
}

void Player::Seek()
{


	//setDesiredVelocity(getTargetPosition());

	//const glm::vec2 steering_direction = getDesiredVelocity() - getCurrentDirection();

	//LookWhereYoureGoing(steering_direction);

	//getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();
}

void Player::LookWhereYoureGoing(const glm::vec2 target_direction)
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

	updateWhiskers(getWhiskerAngle());
}

void Player::m_move()
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
