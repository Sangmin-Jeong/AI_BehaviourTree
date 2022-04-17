#include "RangedCombatEnemy.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "AttackAction.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"
#include "FleeAction.h"
#include "MoveToCoverAction.h"
#include "MoveToRangeAction.h"
#include "WaitBehindCoverAction.h"
#include "PlayScene.h" // in cpp is fine. in h can cause a infinity loop

RangedCombatEnemy::RangedCombatEnemy(Scene* scene)
	:m_pScene(scene), m_fireCtr(0), m_fireCtrMax(60)
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

	//TextureManager::Instance().loadSpriteSheet(
	//	"../Assets/sprites/enemySprites/enemy.txt",
	//	"../Assets/sprites/enemySprites/SteamMan_attack1.png",
	//	"enemyAttack");
	//setSpriteSheet(TextureManager::Instance().getSpriteSheet("enemyAttack"));

	setWidth(48);
	setHeight(48);
	
	getRigidBody()->bounds = glm::vec2(getWidth(), getHeight());
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting motion properties
	setCurrentHeading(0.0f);// current facing angle
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_maxSpeed = 5.0f; // a maximum number of pixels moved per frame
	m_turnRate = 50.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 2.0f; // a maximum number of pixels to add to the velocity each frame
	
	setLOSDistance(160.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));

	// Fill in action state and patrol code
	setActionState(NO_ACTION);

	// Animation
	setAnimationSheet();

	// Health
	setHealth(100);

	// Patrol
	m_patrol.push_back(glm::vec2(400, 230));
	m_patrol.push_back(glm::vec2(600, 230));
	m_waypoint = 0;

	setTargetPosition(m_patrol[m_waypoint]);

	setType(AGENT);

	// Create decision tree
	setTree(new DecisionTree(this)); // Overloaded constructor.
	m_buildTree();
	getTree()->Display(); // Optional.
}

RangedCombatEnemy::~RangedCombatEnemy()
= default;

void RangedCombatEnemy::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	Util::DrawCapsule(glm::vec2(m_patrol[0].x, m_patrol[0].y), 10, 10,glm::vec4(0, 0, 0, 1));
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
		TextureManager::Instance().playAnimation("enemyWalk", getAnimation("walk"),
			x, y, getAnimationSpeed(), 0, 255, true);
		break;
	case ENEMY_WALK_L:
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

	//// draw LOS
	//if (EventManager::Instance().isIMGUIActive())
	//{
	//	Util::DrawLine(getTransform()->position, getMiddleLOSEndPoint(), getLOSColour());
	//}
}

void RangedCombatEnemy::setAnimationSheet()
{
	Animation idle = Animation();
	idle.name = "idle";
	idle.frames.push_back(getSpriteSheet()->getFrame("ENEMY-IDLE-0"));
	idle.frames.push_back(getSpriteSheet()->getFrame("ENEMY-IDLE-1"));
	idle.frames.push_back(getSpriteSheet()->getFrame("ENEMY-IDLE-2"));
	idle.frames.push_back(getSpriteSheet()->getFrame("ENEMY-IDLE-3"));
	setAnimation(idle);

	Animation walk = Animation();
	walk.name = "walk";
	walk.frames.push_back(getSpriteSheet()->getFrame("ENEMY-WALK-0"));
	walk.frames.push_back(getSpriteSheet()->getFrame("ENEMY-WALK-1"));
	walk.frames.push_back(getSpriteSheet()->getFrame("ENEMY-WALK-2"));
	walk.frames.push_back(getSpriteSheet()->getFrame("ENEMY-WALK-3"));
	walk.frames.push_back(getSpriteSheet()->getFrame("ENEMY-WALK-4"));
	walk.frames.push_back(getSpriteSheet()->getFrame("ENEMY-WALK-5"));
	setAnimation(walk);

	Animation hurt = Animation();
	hurt.name = "hurt";
	hurt.frames.push_back(getSpriteSheet()->getFrame("ENEMY-HURT-0"));
	hurt.frames.push_back(getSpriteSheet()->getFrame("ENEMY-HURT-1"));
	hurt.frames.push_back(getSpriteSheet()->getFrame("ENEMY-HURT-2"));
	setAnimation(hurt);

	Animation death = Animation();
	death.name = "death";
	death.frames.push_back(getSpriteSheet()->getFrame("ENEMY-DEATH-0"));
	death.frames.push_back(getSpriteSheet()->getFrame("ENEMY-DEATH-1"));
	death.frames.push_back(getSpriteSheet()->getFrame("ENEMY-DEATH-2"));
	death.frames.push_back(getSpriteSheet()->getFrame("ENEMY-DEATH-3"));
	death.frames.push_back(getSpriteSheet()->getFrame("ENEMY-DEATH-4"));
	death.frames.push_back(getSpriteSheet()->getFrame("ENEMY-DEATH-5"));
	setAnimation(death);

	Animation gone = Animation();
	gone.name = "gone";
	gone.frames.push_back(getSpriteSheet()->getFrame("ENEMY-GONE-0"));
	gone.frames.push_back(getSpriteSheet()->getFrame("ENEMY-GONE-1"));
	gone.frames.push_back(getSpriteSheet()->getFrame("ENEMY-GONE-2"));
	gone.frames.push_back(getSpriteSheet()->getFrame("ENEMY-GONE-3"));
	gone.frames.push_back(getSpriteSheet()->getFrame("ENEMY-GONE-4"));
	gone.frames.push_back(getSpriteSheet()->getFrame("ENEMY-GONE-5"));
	setAnimation(gone);
}

void RangedCombatEnemy::update()
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

void RangedCombatEnemy::clean()
{
}

float RangedCombatEnemy::getMaxSpeed() const
{
	return m_maxSpeed;
}

float RangedCombatEnemy::getTurnRate() const
{
	return m_turnRate;
}

float RangedCombatEnemy::getAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 RangedCombatEnemy::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

void RangedCombatEnemy::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void RangedCombatEnemy::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void RangedCombatEnemy::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void RangedCombatEnemy::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position);
}

void RangedCombatEnemy::Seek()
{
	// Find next waypoint:
	if (Util::distance(m_patrol[m_waypoint], getTransform()->position) <= 10)
	{
		if (++m_waypoint == m_patrol.size()) m_waypoint = 0;
		setTargetPosition(m_patrol[m_waypoint]);
	}

	setDesiredVelocity(getTargetPosition());

	const glm::vec2 steering_direction = getDesiredVelocity() - getCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();
}

void RangedCombatEnemy::LookWhereYoureGoing(const glm::vec2 target_direction)
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

void RangedCombatEnemy::Patrol()
{
	ActionState action = PATROL;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	if (m_waypoint == 0)
	{
		setAnimationState(ENEMY_WALK_L);
	}
	else
	{
		setAnimationState(ENEMY_WALK_R);
	}
	m_move();
}

void RangedCombatEnemy::MoveToPlayer()
{
	ActionState action = MOVE_TO_PLAYER;
	if (getActionState() != action)
	{
		// Initialize. Like set move target to player.
		setActionState(action);
	}
	// m_move();
}

void RangedCombatEnemy::Flee()
{
	ActionState action = FLEE;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	// Action
}

void RangedCombatEnemy::MovetoCover()
{
	ActionState action = MOVE_TO_COVER;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	// Action
}

void RangedCombatEnemy::Attack()
{
	ActionState action = ATTACK;
	if (getActionState() != action)
	{
		// Initialize
		m_fireCtr = 0;
		setActionState(action);
	}
	// Action
	if (m_fireCtr++ % m_fireCtrMax == 0)
	{
		// Fire torpedo
		static_cast<PlayScene*>(m_pScene)->SpawnEnemyTorpedo();
	}
}

void RangedCombatEnemy::WaitBehindCover()
{
	ActionState action = WAIT_BEHIND_COVER;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	// Action
}

void RangedCombatEnemy::MoveToRange()
{
	ActionState action = MOVE_TO_RANGE;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	// Action
}

void RangedCombatEnemy::MoveToLOS()
{
	ActionState action = MOVE_TO_LOS;
	if (getActionState() != action)
	{
		// Initialize
		setActionState(action);
	}
	// Action
}

void RangedCombatEnemy::m_move()
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

	getTransform()->position.x = final_position.x;

	// add our acceleration to velocity
	getRigidBody()->velocity += getRigidBody()->acceleration;

	// clamp our velocity at max speed
	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, getMaxSpeed());
}

void RangedCombatEnemy::m_buildTree()
{
	// Create and add root node.
	getTree()->setEnemyHealthNode(new EnemyHealthCondition());
	getTree()->getTree().push_back(getTree()->getEnemyHealthNode());

	// Nodes off new health root
	TreeNode* fleeNode = getTree()->AddNode(getTree()->getEnemyHealthNode(), new FleeAction(), LEFT_TREE_NODE);
	fleeNode->setAgent(this);
	getTree()->getTree().push_back(fleeNode);

	getTree()->setEnemyHitNode(new EnemyHitCondition());
	getTree()->AddNode(getTree()->getEnemyHealthNode(), getTree()->getEnemyHitNode(), RIGHT_TREE_NODE);
	getTree()->getTree().push_back(getTree()->getEnemyHitNode());
	//

	// Node children of enemy hit node
	getTree()->setPlayerDetectedNode(new PlayerDetectedCondition());
	getTree()->AddNode(getTree()->getEnemyHitNode(), getTree()->getPlayerDetectedNode(), LEFT_TREE_NODE);
	getTree()->getTree().push_back(getTree()->getPlayerDetectedNode());

	LOSCondition* losNodeRight = new LOSCondition();
	getTree()->AddNode(getTree()->getEnemyHitNode(), losNodeRight, RIGHT_TREE_NODE);
	losNodeRight->setAgent(this);
	getTree()->getTree().push_back(losNodeRight);
	//

	TreeNode* patrolNode = getTree()->AddNode(getTree()->getPlayerDetectedNode(), new PatrolAction(), LEFT_TREE_NODE);
	patrolNode->setAgent(this);
	getTree()->getTree().push_back(patrolNode);

	// We have a LOS condition already so make a new node of it.
	LOSCondition* losNodeLeft = new LOSCondition();
	getTree()->AddNode(getTree()->getPlayerDetectedNode(), losNodeLeft, RIGHT_TREE_NODE);
	losNodeLeft->setAgent(this);
	getTree()->getTree().push_back(losNodeLeft);

	// Child nodes of left LOS node
	TreeNode* moveToLOSNode = getTree()->AddNode(losNodeLeft, new MoveToLOSAction(), LEFT_TREE_NODE);
	moveToLOSNode->setAgent(this);
	getTree()->getTree().push_back(moveToLOSNode);

	getTree()->setRangedCombatNode(new RangedCombatCondition());
	getTree()->AddNode(losNodeLeft, getTree()->getRangedCombatNode(), RIGHT_TREE_NODE);
	getTree()->getTree().push_back(getTree()->getRangedCombatNode());

	TreeNode* moveToRangeNode = getTree()->AddNode(getTree()->getRangedCombatNode(), new MoveToRangeAction(), LEFT_TREE_NODE);
	moveToRangeNode->setAgent(this);
	getTree()->getTree().push_back(moveToRangeNode);

	TreeNode* attackNode = getTree()->AddNode(getTree()->getRangedCombatNode(), new AttackAction(), RIGHT_TREE_NODE);
	attackNode->setAgent(this);
	getTree()->getTree().push_back(attackNode);
	//

	// Child nodes of right LOS node
	TreeNode* waitBehindCoverNode = getTree()->AddNode(losNodeRight, new WaitBehindCoverAction(), LEFT_TREE_NODE);
	waitBehindCoverNode->setAgent(this);
	getTree()->getTree().push_back(waitBehindCoverNode);

	TreeNode* moveToCoverNode = getTree()->AddNode(losNodeRight, new MoveToCoverAction(), RIGHT_TREE_NODE);
	moveToCoverNode->setAgent(this);
	getTree()->getTree().push_back(moveToCoverNode);
	//

	//// Create and add root node.
	//m_tree->setPlayerDetectedNode(new PlayerDetectedCondition());
	//m_tree->getTree().push_back(m_tree->getPlayerDetectedNode());

	//TreeNode* patrolNode = m_tree->AddNode(m_tree->getPlayerDetectedNode(), new PatrolAction(), LEFT_TREE_NODE);
	//patrolNode->setAgent(this);
	//m_tree->getTree().push_back(patrolNode);

	//LOSCondition* losNode = new LOSCondition();
	//m_tree->AddNode(m_tree->getPlayerDetectedNode(), losNode, RIGHT_TREE_NODE);
	//losNode->setAgent(this);
	//m_tree->getTree().push_back(losNode);
	//
	//TreeNode* moveToLOSNode = m_tree->AddNode(losNode, new MoveToLOSAction(), LEFT_TREE_NODE);
	//moveToLOSNode->setAgent(this);
	//m_tree->getTree().push_back(moveToLOSNode);

	//m_tree->setCloseCombatNode(new CloseCombatCondition());
	//m_tree->AddNode(losNode, m_tree->getCloseCombatNode(), RIGHT_TREE_NODE);
	//m_tree->getTree().push_back(m_tree->getCloseCombatNode());

	//TreeNode* moveToPlayerNode = m_tree->AddNode(m_tree->getCloseCombatNode(), new MoveToPlayerAction(), LEFT_TREE_NODE);
	//moveToPlayerNode->setAgent(this);
	//m_tree->getTree().push_back(moveToPlayerNode);

	//TreeNode* attackNode = m_tree->AddNode(m_tree->getCloseCombatNode(), new AttackAction(), RIGHT_TREE_NODE);
	//attackNode->setAgent(this);
	//m_tree->getTree().push_back(attackNode);
}
