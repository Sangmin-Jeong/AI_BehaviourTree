#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "SoundManager.h"
#include <string>
#include <fstream>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"


PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	TextureManager::Instance().draw("bg", 0, 0);
	for (auto element : m_pObstacles)
	{
		Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	}

	for (auto element : m_pInvisObstacles)
	{
		Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	}

	for (auto element : m_pLadders)
	{
		Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	}

	Util::DrawRect(glm::vec2(m_pPlayer->getTransform()->position.x - (m_pPlayer->getWidth() / 2), m_pPlayer->getTransform()->position.y - (m_pPlayer->getHeight() / 2)), m_pPlayer->getWidth(), m_pPlayer->getHeight());
	Util::DrawRect(glm::vec2(m_pEnemy->getTransform()->position.x - (m_pEnemy->getWidth() / 2), m_pEnemy->getTransform()->position.y - (m_pEnemy->getHeight() / 2)), m_pEnemy->getWidth(), m_pEnemy->getHeight());

	drawDisplayList();

	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);

	if (m_isGridEnabled == true)
	{
		m_pPlayer->drawLOS();
		m_pEnemy->drawEnemyLOS();
	}

	if(m_path_toggle == true)
	{
		// for the line that is connected between Player and Closest path_node
		Util::DrawLine(m_pPlayer->getTransform()->position, m_pPlayerClosest->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		// for the line that is connected between m_pPlayerClosest path_node and m_pEnemyClosest
		Util::DrawLine(m_pPlayerClosest->getTransform()->position, m_pEnemyClosest->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		// for the line that is connected between m_pEnemyClosest path_node and Enemy
		Util::DrawLine(m_pEnemyClosest->getTransform()->position, m_pEnemy->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	}
}

void PlayScene::update()
{
	//std::cout << CollisionManager::circleAABBsquaredDistance(m_pPlayer->getTransform()->position, m_pPlayer->getLOSDistance(), m_pEnemy->getTransform()->position, m_pEnemy->getWidth(), m_pEnemy->getHeight()) << std::endl;

	updateDisplayList();
	m_checkAgentLOS(m_pPlayer, m_pEnemy);
	m_checkAgentLOS(m_pEnemy ,m_pPlayer);

	// For the path_nodes LOS
	switch (m_LOSMode)
	{
	case 0:
		m_checkAllNodesWithTarget(m_pEnemy);
		break;
	case 1:
		m_checkAllNodesWithTarget(m_pPlayer);
		break;
	case 2:
		m_checkAllNodesWithBoth();
		break;
	}

	
	PathNode* temp1 = new PathNode;
	PathNode* temp2 = new PathNode;
	for (auto path_node : m_pGrid)
	{
		if (path_node->hasLOS()) // check if path_node has LOS
		{
			auto T2ODistance = Util::getClosestEdge(m_pEnemy->getTransform()->position, path_node);
			auto A2ODistance = Util::getClosestEdge(m_pPlayer->getTransform()->position, path_node);

			if (A2ODistance < temp1->getLOSDistance()) // find what path_node is the m_pPlayerClosest from Player
			{
				temp1->setLOSDistance(A2ODistance);
				m_pPlayerClosest->getTransform()->position = temp1->getTransform()->position = path_node->getTransform()->position;
			}
			else if (T2ODistance < temp2->getLOSDistance()) // find what path_node is the m_pEnemyClosest from Enemy
			{
				temp2->setLOSDistance(T2ODistance);
				m_pEnemyClosest->getTransform()->position = temp2->getTransform()->position = path_node->getTransform()->position;
			}
		}

	}
	delete temp1;
	delete temp2;
	// If path is clear or not
	if (m_checkPathNodeLOS(m_pPlayerClosest, m_pPlayer) && m_checkPathNodeLOS(m_pPlayerClosest, m_pEnemyClosest) && m_checkPathNodeLOS(m_pPlayerClosest, m_pEnemyClosest))
	{
		m_LOS_Clear = true;
	}
	else
	{
		m_LOS_Clear = false;
	}

	// Player VS Obstacles
	if (m_pObstacles.size() > 0 && !(m_pPlayer->getCanDown() == true || m_pPlayer->getCanUp() == true))
	{
		for (auto obstacle : m_pObstacles)
		{
			if (CollisionManager::AABBCheck2(m_pPlayer, obstacle))
			{
				// Player VS Leftside
				if (m_pPlayer->getTransform()->position.x <= obstacle->getTransform()->position.x - obstacle->getWidth() / 2)
				{
					m_pPlayer->getTransform()->position.x -= m_moveAmount;
				}
				// Player VS Rightside
				else if (m_pPlayer->getTransform()->position.x >= obstacle->getTransform()->position.x + obstacle->getWidth() / 2)
				{
					m_pPlayer->getTransform()->position.x += m_moveAmount;
				}
				// Player VS Topside
				else if (m_pPlayer->getTransform()->position.y <= obstacle->getTransform()->position.y - obstacle->getHeight() / 2)
				{
					m_pPlayer->getTransform()->position.y -= m_moveAmount;
				}
				// Player VS Downside
				else if (m_pPlayer->getTransform()->position.y >= obstacle->getTransform()->position.y + obstacle->getHeight() / 2)
				{
					m_pPlayer->getTransform()->position.y += m_moveAmount;
				}
			}
		}
	}

	// Player VS Invisble Obstacles
	if (m_pInvisObstacles.size() > 0 && !(m_pPlayer->getCanDown() == true || m_pPlayer->getCanUp() == true))
	{
		for (auto invisobstacle : m_pInvisObstacles)
		{
			if (CollisionManager::AABBCheck2(m_pPlayer, invisobstacle))
			{
				// Player VS Leftside
				if (m_pPlayer->getTransform()->position.x <= invisobstacle->getTransform()->position.x - invisobstacle->getWidth() / 2)
				{
					m_pPlayer->getTransform()->position.x -= m_moveAmount;
				}
				// Player VS Rightside
				else if (m_pPlayer->getTransform()->position.x >= invisobstacle->getTransform()->position.x + invisobstacle->getWidth() / 2)
				{
					m_pPlayer->getTransform()->position.x += m_moveAmount;
				}
			}
		}
	}

	// Player VS Ladders
	if (m_pLadders.size() > 0)
	{
		int trueCountUp = 0;
		int trueCountDown = 0;
		for (auto ladder : m_pLadders)
		{
			
			if (CollisionManager::AABBCheck2(ladder, m_pPlayer))
			{
				m_pPlayer->setCanUp(true);
				m_pPlayer->setCanDown(true);
				// Player VS Leftside
				if (m_pPlayer->getTransform()->position.x <= ladder->getTransform()->position.x - ladder->getWidth() / 2)
				{
					m_pPlayer->setCanUp(true);
				}
				// Player VS Rightside
				else if (m_pPlayer->getTransform()->position.x >= ladder->getTransform()->position.x + ladder->getWidth() / 2)
				{
					m_pPlayer->setCanUp(true);
				}
				// Player VS Topside
				else if (m_pPlayer->getTransform()->position.y <= ladder->getTransform()->position.y - ladder->getHeight() / 2)
				{
					m_pPlayer->setCanDown(true);
					m_pPlayer->setCanUp(false);
				}
				// Player VS Downside
				else if (m_pPlayer->getTransform()->position.y >= ladder->getTransform()->position.y + ladder->getHeight() / 2)
				{
					m_pPlayer->setCanUp(true);
					m_pPlayer->setCanDown(false);
				}
			}

			if (ladder->getRigidBody()->isColliding == true)
			{
				trueCountUp++;
			}
		}
		if (trueCountUp <= 0)
		{
			m_pPlayer->setCanUp(false);
			m_pPlayer->setCanDown(false);
			trueCountUp = 0;
		}
	}

	// Enemy VS Obstacles
	if (m_pObstacles.size() > 0)
	{
		for (auto obstacle : m_pObstacles)
		{
			if (CollisionManager::AABBCheck2(m_pEnemy, obstacle))
			{
				// Enemy VS Leftside
				if (m_pEnemy->getTransform()->position.x <= obstacle->getTransform()->position.x - obstacle->getWidth() / 2)
				{
					m_pEnemy->getTransform()->position.x -= m_moveAmount;
				}
				// Enemy VS Rightside
				else if (m_pEnemy->getTransform()->position.x >= obstacle->getTransform()->position.x + obstacle->getWidth() / 2)
				{
					m_pEnemy->getTransform()->position.x += m_moveAmount;
				}
				// Enemy VS Topside
				else if (m_pEnemy->getTransform()->position.y <= obstacle->getTransform()->position.y - obstacle->getHeight() / 2)
				{
					m_pEnemy->getTransform()->position.y -= m_moveAmount;
				}
				// Enemy VS Downside
				else if (m_pEnemy->getTransform()->position.y >= obstacle->getTransform()->position.y + obstacle->getHeight() / 2)
				{
					m_pEnemy->getTransform()->position.y += m_moveAmount;
				}
			}
		}
	}


	// Dagger(Range attack) VS Enemy
	if (m_pDaggers.size() > 0)
	{
		// Guide Dagger to Enemy

		// Player's Position -> PlayerClosest's position
		if (CollisionManager::squaredDistance(m_pDaggers.back()->getTransform()->position, m_pPlayerClosest->getTransform()->position) < 500)
		{
			m_pDaggers.back()->setTargetPosition(m_pEnemyClosest->getTransform()->position);
		}
		// PlayerClosest's position -> EnemyClosest's position
		else if (CollisionManager::squaredDistance(m_pDaggers.back()->getTransform()->position, m_pEnemyClosest->getTransform()->position) < 1000)
		{
			m_pDaggers.back()->setTargetPosition(m_pEnemy->getTransform()->position);
		}

		// Collision check
		if (CollisionManager::AABBCheck(m_pDaggers.back(), m_pEnemy))
		{
			m_pEnemy->setAnimationState(ENEMY_HURT_L);
			m_pEnemyHealth->setEnemyHealth(m_pEnemyHealth->getEnemyHealth() - 1);
			SoundManager::Instance().playSound("hit", 0, -1);

			removeChild(m_pDaggers.back());
			m_pDaggers.back() = nullptr;
			m_pDaggers.pop_back();
			m_pDaggers.shrink_to_fit();
		}
		//If Dagger is away from Player so far, remove Dagger
		else if (CollisionManager::squaredDistance(m_pDaggers.back()->getTransform()->position, m_pPlayer->getTransform()->position) > 50000)
		{
			removeChild(m_pDaggers.back());
			m_pDaggers.back() = nullptr;
			m_pDaggers.pop_back();
			m_pDaggers.shrink_to_fit();
		}

	}

	if (m_pEnemy->getAnimation("hurt").current_frame == 2)
	{
		if (isRight == false)
		{
			m_pEnemy->setAnimationState(ENEMY_WALK_L);
		}
		else if (isRight == true)
		{
			m_pEnemy->setAnimationState(ENEMY_WALK_R);
		}
		m_pEnemy->getAnimation("hurt").current_frame = 0;
	}

	if (CollisionManager::AABBCheck(m_pPlayer, m_pEnemy))
	{
		countTimer -= TheGame::Instance().getDeltaTime() * 1000.0f;
		if (countTimer <= 0)
		{
			m_pPlayerHealth->setPlayerHealth(m_pPlayerHealth->getPlayerHealth() - 1);
			SoundManager::Instance().playSound("hit", 0, -1);
			countTimer = 1500;
		}
		//std::cout << countTimer/1000 << "\n";
	}
	else
	{
		countTimer = 1500;
	}

	if (m_pPlayerHealth->getPlayerHealth() <= 0)
	{
		m_score--;

		// reset enemy health
		m_pPlayerHealth->setPlayerHealth(4);

		// reset enemy position
		m_pPlayer->getTransform()->position = glm::vec2(110.0f, 600.0f);

		// reset enemy animation
		m_pPlayer->setAnimationState(PLAYER_IDLE_L);
	}
	//std::cout << m_score << std::endl;

	if (m_pEnemyHealth->getEnemyHealth() <= 0)
	{
		m_pEnemy->setAnimationState(ENEMY_DEATH_L);
	}

	// After Enemy death animation is done, set enemy animation to "blank" (blank frame to simulate disappearance)
	if (m_pEnemy->getAnimation("death").current_frame == 5)// end of frame
	{
		// raise score
		m_score++;

		// enemy death 
		SoundManager::Instance().playSound("Death");

		//reset enemy health
		m_pEnemyHealth->setEnemyHealth(4);

		//reset enemy position
		m_pEnemy->getTransform()->position = glm::vec2(600.0f, 430.0f);

		/// reset enemy animation
		m_pEnemy->setAnimationState(ENEMY_WALK_L);

		m_pEnemy->getAnimation("death").current_frame = 0;

	}

	// Score
	if (m_score-- == m_score++)
	{
		const SDL_Color blue = { 0, 0, 255, 255 };

		removeChild(m_pScoreLabel);

		m_pScoreLabel = new Label("Score:" + std::to_string(m_score), "Consolas", 20, blue, glm::vec2(55.0f, 70.0f));
		//m_pScoreLabel = new Label("Score:", "Consolas", 20, blue, glm::vec2(45.0f, 70.0f));
		m_pScoreLabel->setParent(this);
		addChild(m_pScoreLabel);
	}

	m_pEnemy->update();
	
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	const int movingTime = Game::Instance().getFrames();
	const int movingInterval = 8;

	////////////////////// CONTROLLER ////////////////////// 
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_W))
		if (CheckKeyList('W'))DeleteKeyList('W');
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W))
		if (!CheckKeyList('W'))keyList.push_back('W');
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_S))
		if (CheckKeyList('S'))DeleteKeyList('S');
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_S))
		if (!CheckKeyList('S'))keyList.push_back('S');
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_A))
		if (CheckKeyList('A'))DeleteKeyList('A');
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
		if (!CheckKeyList('A'))keyList.push_back('A');
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_D))
		if (CheckKeyList('D'))DeleteKeyList('D');
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
		if (!CheckKeyList('D'))keyList.push_back('D');
	if (EventManager::Instance().mousePressed(1))
		if (!CheckKeyList('L'))keyList.push_back('L');
	if (EventManager::Instance().mousePressed(3))
		if (!CheckKeyList('R'))keyList.push_back('R');

	key = keyList.size() > 0 ? ((char)keyList[keyList.size() - 1]) : 0;

	if (key == 'W')
	{
		if (m_pPlayer->getCanUp() == true)
		{
			m_pPlayer->getTransform()->position.y -= m_moveAmount;
			m_pPlayer->setCurrentHeading(270);
			if (isRight == false)
			{
				m_pPlayer->setAnimationState(PLAYER_CLIMB_L);
			}
			else
			{
				m_pPlayer->setAnimationState(PLAYER_CLIMB_R);
			}
		}
	}
	else if (key == 'A')
	{
		if (!(m_pPlayer->getCanUp() && m_pPlayer->getCanDown()))
		{
			m_pPlayer->getTransform()->position.x -= m_moveAmount;
			m_pPlayer->setCurrentHeading(180);
			isRight = false;
			m_pPlayer->setAnimationState(PLAYER_RUN_L);
		}
		if (m_pPlayer->getTransform()->position.y == 565)
		{
			m_pPlayer->getTransform()->position.x = 730;
			m_pPlayer->getTransform()->position.y = 575;
		}
		if (m_pPlayer->getTransform()->position.y == 425)
		{
			m_pPlayer->getTransform()->position.y = 435;
		}

		if (movingTime % movingInterval == 0)
		{
			isMoving = true;
		}

		if (isMoving == true)
		{
			SoundManager::Instance().playSound("walk", 0, -1);
			isMoving = false;
		}

		m_pPlayer->setCurrentHeading(180);
		isRight = false;
		m_pPlayer->setAnimationState(PLAYER_RUN_L);
	}
	else if (key == 'S')
	{
		if (m_pPlayer->getCanDown() == true && m_pPlayer->getTransform()->position.y < HEIGHT - 40)
		{
			m_pPlayer->getTransform()->position.y += m_moveAmount;
			m_pPlayer->setCurrentHeading(90);
			if (isRight == false)
			{
				m_pPlayer->setAnimationState(PLAYER_CLIMB_L);
			}
			else
			{
				m_pPlayer->setAnimationState(PLAYER_CLIMB_R);
			}
		}
	}
	else if (key == 'D')
	{
		m_pPlayer->getTransform()->position.x += m_moveAmount;

		if (movingTime % movingInterval == 0)
		{
			isMoving = true;
		}

		if (isMoving == true)
		{
			SoundManager::Instance().playSound("walk", 0, -1);
			isMoving = false;
		}

		m_pPlayer->setCurrentHeading(0);
		isRight = true;
		m_pPlayer->setAnimationState(PLAYER_RUN_R);
	}

	else if (key == 'R')
	{
		// isRange Range Possible
		if(CollisionManager::circleAABBsquaredDistance(m_pPlayer->getTransform()->position, m_pPlayer->getLOSDistance(), m_pEnemy->getTransform()->position, m_pEnemy->getWidth(), m_pEnemy->getHeight()) <= 28 && m_LOS_Clear)
		{
			if (isRight == false)
			{
				m_pPlayer->setAnimationState(PLAYER_RANGE_L);

				if (knifeThrowingSound == false)
				{
					SoundManager::Instance().playSound("knifethrowing", 0, -1);
					knifeThrowingSound = true;
				}
			}
			else if (isRight == true)
			{
				m_pPlayer->setAnimationState(PLAYER_RANGE_R);
				if (knifeThrowingSound == false)
				{
					SoundManager::Instance().playSound("knifethrowing", 0, -1);
					knifeThrowingSound = true;
				}
			}

			if (m_pDaggers.size() < 1)
			{
				m_pDaggers.push_back(new Weapon(m_pPlayer->getTransform()->position));
				m_pDaggers.shrink_to_fit();
				m_pDaggers.back()->setIsMoving(true);
				m_pDaggers.back()->setTargetPosition(m_pPlayerClosest->getTransform()->position);
				addChild(m_pDaggers.back());
			}
		}

		if (m_pPlayer->getAnimation("range").current_frame == 5)
		{
			if (isRight == false)
			{
				m_pPlayer->setAnimationState(PLAYER_RANGE_L);

				if (knifeThrowingSound == true)
				{
					knifeThrowingSound = false;
				}
			}
			else if (isRight == true)
			{
				m_pPlayer->setAnimationState(PLAYER_RANGE_R);

				if (knifeThrowingSound == true)
				{
					knifeThrowingSound = false;
				}
			}
			m_pPlayer->getAnimation("range").current_frame = 0;
			keyList.pop_back();
		}
	}
	else if (key == 'L')
	{
		if (isRight == false)
		{
			m_pPlayer->setAnimationState(PLAYER_MELEE_L);
			if (knifeSound == false)
			{
				SoundManager::Instance().playSound("knife", 0, -1);
				knifeSound = true;
			}
		}
		else if (isRight == true)
		{
			m_pPlayer->setAnimationState(PLAYER_MELEE_R);

			if (knifeSound == false)
			{
				SoundManager::Instance().playSound("knife", 0, -1);
				knifeSound = true;
			}
		}
		
		// isMelee Range Possible
		float distance = Util::distance(m_pPlayer->getTransform()->position, m_pEnemy->getTransform()->position);
		//std::cout << "distance: " << distance << " : " << m_pPlayer->hasLOS() << "isRight" << isRight << "\n";
		if (m_pPlayer->hasLOS() && ((isRight && distance <= MELEE_DISTANCE_R) || (!isRight && distance <= MELEE_DISTANCE_L)))
		{
			m_pEnemy->setAnimationState(ENEMY_HURT_L);
			time = 500;
			isMeleeDamageTaken = true;
		}

		if (m_pPlayer->getAnimation("melee").current_frame == 5)
		{
			if (isRight == false)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_L);

				if (knifeSound == true)
				{
					knifeSound = false;
				}
			}
			else if (isRight == true)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_R);

				if (knifeSound == true)
				{
					knifeSound = false;
				}
			}
			m_pPlayer->getAnimation("melee").current_frame = 0;
			keyList.pop_back();
		}
	}
	else
	{
		if (isRight == false)
		{
			m_pPlayer->setAnimationState(PLAYER_IDLE_L);
		}
		else if (isRight == true)
		{
			m_pPlayer->setAnimationState(PLAYER_IDLE_R);
		}
	}

	time -= TheGame::Instance().getDeltaTime() * 1000.0f;
	// After enemy damage taken by player
	if (isMeleeDamageTaken)
	{
		if (time < 0)
		{
			m_pEnemyHealth->setEnemyHealth(m_pEnemyHealth->getEnemyHealth() - 1);
			m_pEnemy->setAnimationState(ENEMY_WALK_L);
			isMeleeDamageTaken = false;
			SoundManager::Instance().playSound("hit", 0, -1);
		}
	}

	// Boundary for player
	if (m_pPlayer->getTransform()->position.x < 0)
	{
		m_pPlayer->getTransform()->position.x = 0;
	}
	else if (m_pPlayer->getTransform()->position.x > WIDTH)
	{
		m_pPlayer->getTransform()->position.x = WIDTH;
	}
	else if (m_pPlayer->getTransform()->position.y < 25)
	{
		m_pPlayer->getTransform()->position.y = 25;
	}
	else if (m_pPlayer->getTransform()->position.y > HEIGHT -25)
	{
		m_pPlayer->getTransform()->position.y = HEIGHT -25;
	}

	// Base Enemy¡¯s movement will be toggled between idle and patrol
	if (EventManager::Instance().keyPressed(SDL_SCANCODE_P))
	{
		if (!isRight)
		{
			if (isPatrolling == true)
			{
				m_pEnemy->setAnimationState(ENEMY_IDLE_L);
				isPatrolling = false;
			}
			else
			{
				m_pEnemy->setAnimationState(ENEMY_WALK_L);
				isPatrolling = true;
			}
		}

		if (isRight)
		{
			if (isPatrolling == true)
			{
				m_pEnemy->setAnimationState(ENEMY_IDLE_R);
				isPatrolling = false;
			}
			else
			{
				m_pEnemy->setAnimationState(ENEMY_WALK_R);
				isPatrolling = true;
			}
		}


	}

	// Debug view
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_H))
	{
		if (m_isGridEnabled == true)
		{
			m_isGridEnabled = false;
			m_toggleGrid(m_isGridEnabled);
			SDL_Delay(200);
		}
		else
		{
			m_isGridEnabled = true;
			m_toggleGrid(m_isGridEnabled);
			SDL_Delay(200);

		}
	}

	// To check Player and Enemy health
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_L))
	{
		m_pPlayerHealth->setPlayerHealth(m_pPlayerHealth->getPlayerHealth() - 1);
		SDL_Delay(100);
	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_K))
	{
		m_pEnemyHealth->setEnemyHealth(m_pEnemyHealth->getEnemyHealth() - 1);
		SDL_Delay(100);
	}
	/*----------------------------------------------------------------------------------------*/

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance().changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance().changeSceneState(END_SCENE);
	}
}

void PlayScene::start()
{
	// dead
	enemyIsDead = false;
	playerIsDead = false;

	isMoving = false;
	isPatrolling = true;

	// bools for sound effects
	knifeSound = false;
	knifeThrowingSound = false;

	// Movement
	m_moveAmount = 10;

	// Set GUI Title
	m_guiTitle = "Play Scene";

	// Obstacle
	std::ifstream inFile("../Assets/data/obstacles.txt");
	while(!inFile.eof())
	{
		Obstacle* obstacle = new Obstacle();
		float x, y, w, h;
		inFile >> x >> y >> w >> h;
		obstacle->getTransform()->position = glm::vec2(x, y);
		obstacle->setWidth(w);
		obstacle->setHeight(h);
		addChild(obstacle);
		m_pObstacles.push_back(obstacle);
	}
	inFile.close();

	// Invisible Obstacle
	inFile.open("../Assets/data/InvisObstacle.txt");
	while (!inFile.eof())
	{
		InvisObstacle* invisobstacle = new InvisObstacle();
		float x, y, w, h;
		inFile >> x >> y >> w >> h;
		invisobstacle->getTransform()->position = glm::vec2(x, y);
		invisobstacle->setWidth(w);
		invisobstacle->setHeight(h);
		addChild(invisobstacle);
		m_pInvisObstacles.push_back(invisobstacle);
	}
	inFile.close();

	// ladder
	inFile.open("../Assets/data/ladders.txt");
	while (!inFile.eof())
	{
		Ladder* ladder = new Ladder();
		float x, y, w, h;
		inFile >> x >> y >> w >> h;
		ladder->getTransform()->position = glm::vec2(x, y);
		ladder->setWidth(w);
		ladder->setHeight(h);
		addChild(ladder);
		m_pLadders.push_back(ladder);
	}
	inFile.close();

	// Setup a few fields
	m_LOSMode = 0;
	m_obstacleBuffer = 0;
	m_pathNodeLOSDistance = 1000;
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);

	// For LOS
	m_pPlayerClosest = new PathNode();
	m_pEnemyClosest = new PathNode();


	// Setup the grid
	m_isGridEnabled = false;
	m_path_toggle = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	m_pEnemy = new Enemy();
	m_pEnemy->getTransform()->position = glm::vec2(600.0f, 430.0f);
	addChild(m_pEnemy , 2);

	m_pPlayer = new Player();
	m_pPlayer->getTransform()->position = glm::vec2(110.0f, 600.0f);
	m_pPlayer->setTargetPosition(m_pEnemy->getTransform()->position);
	addChild(m_pPlayer, 3);

	//Life
	m_pPlayerHealth = new PlayerHealth();
	addChild(m_pPlayerHealth, 3, 0);

	m_pEnemyHealth = new EnemyHealth();
	addChild(m_pEnemyHealth, 3, 0);

	// Score
	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pScoreLabel = new Label(std::to_string(m_score) + " Pts", "Consolas", 20, blue, glm::vec2(45.0f, 70.0f));
	//m_pScoreLabel = new Label("Score:", "Consolas", 20, blue, glm::vec2(45.0f, 70.0f));
	m_pScoreLabel->setParent(this);
	addChild(m_pScoreLabel);

	// load sound
	SoundManager::Instance().load("../Assets/audio/Knife.flac", "knife", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/KnifeThrow.mp3", "knifethrowing", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/Hit.wav", "hit", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/Walking.mp3", "walk", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/EnemyDeath.mp3", "Death", SOUND_SFX);

	// load music 
	SoundManager::Instance().load("../Assets/audio/cucucucucunningcity.mp3", "song", SOUND_MUSIC);

	// play music
	SoundManager::Instance().playMusic("song", -1, 0);
	SoundManager::Instance().setMusicVolume(5);
	SoundManager::Instance().setSoundVolume(7);


	//BG
	TextureManager::Instance().load("../Assets/textures/bg.png", "bg");

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Assignment 3 Debug Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	if (ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		m_toggleGrid(m_isGridEnabled);
	}

	ImGui::Separator();

	if (ImGui::Button("Node LOS to Enemy", {300,20}))
	{
		m_LOSMode = 0;
	}

	if (m_LOSMode == 0)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to Player", { 300,20 }))
	{
		m_LOSMode = 1;
	}

	if (m_LOSMode == 1)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to both Enemy and Player", { 300,20 }))
	{
		m_LOSMode = 2;
	}

	if (m_LOSMode == 2)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Draw path", { 300,20 }))
	{
		if (m_path_toggle == true)
		{
			m_path_toggle = false;
		}
		else
		{
			m_path_toggle = true;
		}
	}

	if (m_path_toggle == true)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::SliderInt("Path Node LOS Distance", &m_pathNodeLOSDistance, 0, 1000))
	{
		m_setPathNodeLOSDistance(m_pathNodeLOSDistance);
	}

	ImGui::Separator();

	static int shipPosition[] = { m_pPlayer->getTransform()->position.x, m_pPlayer->getTransform()->position.y };
	if (ImGui::SliderInt2("Player Position", shipPosition, 0, 800))
	{
		m_pPlayer->getTransform()->position.x = shipPosition[0];
		m_pPlayer->getTransform()->position.y = shipPosition[1];
	}

	// allow ship rotation
	static int angle;
	if (ImGui::SliderInt("Player Direction", &angle, -360, 360))
	{
		m_pPlayer->setCurrentHeading(angle);
	}

	ImGui::Separator();

	static int EnemyPosition[] = { m_pEnemy->getTransform()->position.x, m_pEnemy->getTransform()->position.y };
	if (ImGui::SliderInt2("Enemy Position", EnemyPosition, 0, 800))
	{
		m_pEnemy->getTransform()->position.x = EnemyPosition[0];
		m_pEnemy->getTransform()->position.y = EnemyPosition[1];
	}

	ImGui::Separator();

	for (unsigned i = 0; i < m_pObstacles.size(); i++)
	{
		int obsPosition[] = { m_pObstacles[i]->getTransform()->position.x, m_pObstacles[i]->getTransform()->position.y };
		std::string label = "Obstacle" + std::to_string(i + 1) + " Position";
		if (ImGui::SliderInt2(label.c_str(), obsPosition, 0, 800))
		{
			m_pObstacles[i]->getTransform()->position.x = obsPosition[0];
			m_pObstacles[i]->getTransform()->position.y = obsPosition[1];
			m_buildGrid();
		}
	}

	ImGui::Separator();

	if (ImGui::SliderInt("Obstacle Buffer", &m_obstacleBuffer, 0, 100))
	{
		m_buildGrid();
	}

	ImGui::End();
}

void PlayScene::m_buildGrid()
{
	auto tileSize = Config::TILE_SIZE;
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	m_clearNodes(); // Because we rebuild often

	// add PathNodes to the Grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* path_node = new PathNode();
			path_node->getTransform()->position = glm::vec2((col * tileSize) + offset.x, (row * tileSize) + offset.y);
			bool keepNode = true;
			for (auto obstacle : m_pObstacles)
			{
				if (CollisionManager::AABBCheckWithBuffer(path_node, obstacle, m_obstacleBuffer))
				{
					keepNode = false; // We have collision between node and an obstacle
				}
			}
			if (keepNode)
			{
				addChild(path_node);
				m_pGrid.push_back(path_node);
			}
			else delete path_node;
		}
	}
	// if Grid is supposed to be hidden, make it so!

	m_toggleGrid(m_isGridEnabled);
}

void PlayScene::m_toggleGrid(bool state)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setVisible(state);
	}
}
bool PlayScene::m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object)
{
	// check angle to target so we can still use LOS distance for path_nodes
	auto targetDirection = target_object->getTransform()->position - path_node->getTransform()->position;
	auto normalizedDirection = Util::normalize(targetDirection);
	path_node->setCurrentDirection(normalizedDirection);
	return m_checkAgentLOS(path_node, target_object);
}

void PlayScene::m_checkAllNodesWithTarget(DisplayObject* target_object)
{
	for (auto path_node : m_pGrid)
	{
		m_checkPathNodeLOS(path_node, target_object);
	}
}

void PlayScene::m_checkAllNodesWithBoth()
{
	for (auto path_node : m_pGrid)
	{
		bool LOSWithSpaceShip = m_checkPathNodeLOS(path_node, m_pPlayer);
		bool LOSWithTarget = m_checkPathNodeLOS(path_node, m_pEnemy);
		path_node->setHasLOS((LOSWithSpaceShip && LOSWithTarget ? true : false));
	}
}

bool PlayScene::m_checkAgentLOS(Agent* agent, DisplayObject* target_object)
{
	bool hasLOS = false;
	agent->setHasLOS(hasLOS);
	// if agent to target distance is less than or equal to LOS Distance
	auto AgentToTargetDistance = Util::getClosestEdge(agent->getTransform()->position, target_object);
	if (AgentToTargetDistance <= agent->getLOSDistance())
	{
		std::vector<DisplayObject*> contactList;
		for (auto object : getDisplayList())
		{
			auto AgentToObjectDistance = Util::getClosestEdge(agent->getTransform()->position, object);
			if (AgentToObjectDistance > AgentToTargetDistance)
			{
				continue;
			}
			if ((object->getType() != AGENT) && (object->getType() != PATH_NODE) && (object->getType() != ENEMY))
			{
				contactList.push_back(object); // If an obstacle, put in here.
			}
		}
		const glm::vec2 agentEndPoint = agent->getTransform()->position + agent->getCurrentDirection() * agent->getLOSDistance();
		hasLOS = CollisionManager::LOSCheck(agent, agentEndPoint, contactList, target_object);

		agent->setHasLOS(hasLOS);
	}
	return hasLOS;
}

void PlayScene::m_clearNodes()
{
	m_pGrid.clear();
	for (auto object : getDisplayList())
	{
		if (object->getType() == PATH_NODE)
			removeChild(object);
	}
}

void PlayScene::m_setPathNodeLOSDistance(int dist)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setLOSDistance((float)dist);
	}
}

bool PlayScene::CheckKeyList(char c) {
	if (std::find(keyList.begin(), keyList.end(), c) != keyList.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool PlayScene::DeleteKeyList(char c) {
	bool _rtn = false;
	for (int i = 0; i < keyList.size(); ) {
		if (keyList[i] == c) {
			keyList.erase(keyList.begin() + i);
			_rtn = true;
			break;
		}
		else {
			i++;
		}
	}
	return _rtn;
}