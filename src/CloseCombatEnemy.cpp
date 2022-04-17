#include "CloseCombatEnemy.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "AttackAction.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"
#include "PlayScene.h"

CloseCombatEnemy::CloseCombatEnemy(Scene* scene) :m_pScene(scene)
{
	setAnimationState(ENEMY_WALK_L);
	setAnimationSpeed(0.8f);
	setIsRight(false);
	//TextureManager::Instance().load("../Assets/textures/Circle.png","circle");

	//const auto size = TextureManager::Instance().getTextureSize("circle");
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/enemySprites/enemy.txt",
		"../Assets/sprites/enemySprites/SteamMan_idle.png",
		"enemyIdle");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("enemyIdle"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/enemySprites/enemy.txt",
		"../Assets/sprites/enemySprites/SteamMan_walk.png",
		"enemyWalk");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("enemyWalk"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/enemySprites/enemy.txt",
		"../Assets/sprites/enemySprites/SteamMan_hurt.png",
		"enemyHurt");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("enemyHurt"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/enemySprites/enemy.txt",
		"../Assets/sprites/enemySprites/SteamMan_death.png",
		"enemyDeath");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("enemyDeath"));

	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/enemySprites/enemy.txt",
		"../Assets/sprites/enemySprites/SteamMan_death.png",
		"enemyGone");
	setSpriteSheet(TextureManager::Instance().getSpriteSheet("enemyGone"));

	setWidth(48);
	setHeight(48);

	getRigidBody()->bounds = glm::vec2(getWidth(), getHeight());
	getTransform()->position = glm::vec2(500.0f, 100.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting motion properties
	setCurrentHeading(0.0f);// current facing angle
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_maxSpeed = 10.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 2.0f; // a maximum number of pixels to add to the velocity each frame

	setLOSDistance(80.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));

	// Fill in action state and patrol code
	setActionState(NO_ACTION);

	// Create decision tree
	setTree(new DecisionTree(this)); // Overloaded constructor.
	m_buildTree();
	getTree()->Display(); // Optional.

	// Animation
	setAnimationSheet();

	// Patrol
	m_patrol.push_back(glm::vec2(400, 430));
	m_patrol.push_back(glm::vec2(600, 430));
	m_waypoint = 0;

	setTargetPosition(m_patrol[m_waypoint]);

	setType(AGENT);
}

CloseCombatEnemy::~CloseCombatEnemy()
= default;

void CloseCombatEnemy::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	Util::DrawCapsule(glm::vec2(m_patrol[0].x, m_patrol[0].y), 10, 10, glm::vec4(0, 0, 0, 1));
	Util::DrawCapsule(glm::vec2(m_patrol[1].x, m_patrol[1].y), 10, 10, glm::vec4(0, 0, 0, 1));

	// draw the Enemy
	//TextureManager::Instance().draw("circle", x, y, 0, 255, isCentered());
	switch (getAnimationState())
	{
	case ENEMY_IDLE_R:
		TextureManager::Instance().playAnimation("enemyIdle", getAnimation("idle"),
			x, y, getAnimationSpeed(), 0, 255, true);
		break;
	case ENEMY_IDLE_L:
		TextureManager::Instance().playAnimation("enemyIdle", getAnimation("idle"),
			x, y, getAnimationSpeed(), 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case ENEMY_WALK_R:
		getTransform()->position.x += 1;
		TextureManager::Instance().playAnimation("enemyWalk", getAnimation("walk"),
			x, y, getAnimationSpeed(), 0, 255, true);
		break;
	case ENEMY_WALK_L:
		getTransform()->position.x -= 1;
		TextureManager::Instance().playAnimation("enemyWalk", getAnimation("walk"),
			x, y, getAnimationSpeed(), 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case ENEMY_HURT_R:
		TextureManager::Instance().playAnimation("enemyHurt", getAnimation("hurt"),
			x, y, getAnimationSpeed(), 0, 255, true);
		break;
	case ENEMY_HURT_L:
		TextureManager::Instance().playAnimation("enemyHurt", getAnimation("hurt"),
			x, y, getAnimationSpeed(), 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case ENEMY_DEATH_R:
		TextureManager::Instance().playAnimation("enemyDeath", getAnimation("death"),
			x, y, getAnimationSpeed(), 0, 255, true);
		break;
	case ENEMY_DEATH_L:
		TextureManager::Instance().playAnimation("enemyDeath", getAnimation("death"),
			x, y, getAnimationSpeed(), 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case ENEMY_GONE:
		TextureManager::Instance().playAnimation("enemyGone", getAnimation("gone"),
			x, y, getAnimationSpeed(), 0, 255, true);
		break;
	default:
		break;
	}

	// draw LOS
	//if (EventManager::Instance().isIMGUIActive())
	//{
	//	Util::DrawLine(getTransform()->position, getMiddleLOSEndPoint(), getLOSColour());
	//}
}

void CloseCombatEnemy::update()
{
	// Determine which action to perform
	getTree()->MakeDecision();

	// Control Hit animation 
	if (getAnimation("hurt").current_frame == 2)
	{
		if (getIsRight() == false)
		{
			setAnimationState(ENEMY_IDLE_L);
		}
		else if (getIsRight() == true)
		{
			setAnimationState(ENEMY_IDLE_R);
		}
		getAnimation("hurt").current_frame = 0;
	}
}

void CloseCombatEnemy::clean()
{
}

float CloseCombatEnemy::getMaxSpeed() const
{
	return m_maxSpeed;
}

float CloseCombatEnemy::getTurnRate() const
{
	return m_turnRate;
}

float CloseCombatEnemy::getAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 CloseCombatEnemy::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

void CloseCombatEnemy::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void CloseCombatEnemy::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void CloseCombatEnemy::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void CloseCombatEnemy::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position);
}

void CloseCombatEnemy::Seek()
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
		else if (m_waypoint == 1)
		{
			setAnimationState(ENEMY_WALK_R);
		}
	}

	setDesiredVelocity(getTargetPosition());

	const glm::vec2 steering_direction = getDesiredVelocity() - getCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();
}

void CloseCombatEnemy::LookWhereYoureGoing(const glm::vec2 target_direction)
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

void CloseCombatEnemy::Patrol()
{
	ActionState action = PATROL;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	m_move();
	
}

void CloseCombatEnemy::MoveToPlayer()
{
	ActionState action = MOVE_TO_PLAYER;
	if (getActionState() != action)
	{
		// Initialize. Like set move target to player.
		setActionState(action);
	}
	// action
	setTargetPosition(static_cast<PlayScene*>(m_pScene)->GetPlayer()->getTransform()->position);
	m_move();
}

void CloseCombatEnemy::MoveToLOS()
{
	ActionState action = MOVE_TO_LOS;
	if (getActionState() != action)
	{
		// Initialize. Like set move target to player.
		setActionState(action);
	}
	// action
	setTargetPosition(static_cast<PlayScene*>(m_pScene)->GetPlayerClosest()->getTransform()->position);
	m_move();
}

void CloseCombatEnemy::Attack()
{
	ActionState action = ATTACK;
	if (getActionState() != action)
	{
		// Initialize. Like set move target to player.
		setActionState(action);
	}
	/*std::cout << "Attack" << std::endl;*/
	// action
}

void CloseCombatEnemy::m_move()
{
	Seek();

	//                                   final Position     position term    velocity term     acceleration term
	// kinematic equation for motion --> Pf            =      Pi     +     Vi*(time)    +   (0.5)*Ai*(time * time)

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
	//getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, getMaxSpeed());
}

void CloseCombatEnemy::m_buildTree()
{
	// Create and add root node.
	getTree()->setPlayerDetectedNode(new PlayerDetectedCondition());
	getTree()->getTree().push_back(getTree()->getPlayerDetectedNode());

	TreeNode* patrolNode = getTree()->AddNode(getTree()->getPlayerDetectedNode(), new PatrolAction(), LEFT_TREE_NODE);
	patrolNode->setAgent(this);
	getTree()->getTree().push_back(patrolNode);

	LOSCondition* losNode = new LOSCondition();
	getTree()->AddNode(getTree()->getPlayerDetectedNode(), losNode, RIGHT_TREE_NODE);
	losNode->setAgent(this);
	getTree()->getTree().push_back(losNode);

	TreeNode* moveToLOSNode = getTree()->AddNode(losNode, new MoveToLOSAction(), LEFT_TREE_NODE);
	moveToLOSNode->setAgent(this);
	getTree()->getTree().push_back(moveToLOSNode);

	getTree()->setCloseCombatNode(new CloseCombatCondition());
	getTree()->AddNode(losNode, getTree()->getCloseCombatNode(), RIGHT_TREE_NODE);
	getTree()->getTree().push_back(getTree()->getCloseCombatNode());

	TreeNode* moveToPlayerNode = getTree()->AddNode(getTree()->getCloseCombatNode(), new MoveToPlayerAction(), LEFT_TREE_NODE);
	moveToPlayerNode->setAgent(this);
	getTree()->getTree().push_back(moveToPlayerNode);

	TreeNode* attackNode = getTree()->AddNode(getTree()->getCloseCombatNode(), new AttackAction(), RIGHT_TREE_NODE);
	attackNode->setAgent(this);
	getTree()->getTree().push_back(attackNode);
}
