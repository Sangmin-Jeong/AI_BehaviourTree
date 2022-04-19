#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
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
	drawDisplayList();


	// player Health
	Util::DrawFilledRect(glm::vec2{ m_pPlayer->getTransform()->position.x - 49, m_pPlayer->getTransform()->position.y - 38 }, m_pPlayer->getHealth(), 10, 
		(m_pPlayer->getHealth() <= 50 ? (m_pPlayer->getHealth() <= 25 ? red : yellow) : green));
	
	// closeCombat Health
	if (m_pEnemies[m_keys[0]] != nullptr)
	{
		Util::DrawFilledRect(glm::vec2{ m_pEnemies[m_keys[0]]->getTransform()->position.x - 49, m_pEnemies[m_keys[0]]->getTransform()->position.y - 38 }, m_pEnemies[m_keys[0]]->getHealth(), 10,
			(m_pEnemies[m_keys[0]]->getHealth() <= 50 ? (m_pEnemies[m_keys[0]]->getHealth() <= 25 ? red : yellow) : green));
	}
	
	// RangedCombat Health
	if (m_pEnemies[m_keys[1]] != nullptr)
	{
		Util::DrawFilledRect(glm::vec2{ m_pEnemies[m_keys[1]]->getTransform()->position.x - 49, m_pEnemies[m_keys[1]]->getTransform()->position.y - 38 }, m_pEnemies[m_keys[1]]->getHealth(), 10,
			(m_pEnemies[m_keys[1]]->getHealth() <= 50 ? (m_pEnemies[m_keys[1]]->getHealth() <= 25 ? red : yellow) : green));
	}
	
	// Shield Health
	Util::DrawFilledRect(glm::vec2{ m_pShields[0]->getTransform()->position.x - 38, m_pShields[0]->getTransform()->position.y - 25}, m_pShields[0]->getHealth(), 7, blue);


	for (auto element : m_pObstacles)
	{
		Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight(), black);
	}

	//for (auto element : m_pInvisObstacles)
	//{
	//	Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	//}

	//for (auto element : m_pLadders)
	//{
	//	Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	//}

	for (auto element : m_pEnemyDaggers)
	{
		Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight(), black);

	}

	Util::DrawRect(glm::vec2(m_pPlayer->getTransform()->position.x - (m_pPlayer->getWidth() / 2), m_pPlayer->getTransform()->position.y - (m_pPlayer->getHeight() / 2)), m_pPlayer->getWidth(), m_pPlayer->getHeight(), black);

	if (m_pEnemies[m_keys[0]] != nullptr)
	{
		Util::DrawRect(glm::vec2(m_pEnemies[m_keys[0]]->getTransform()->position.x - (m_pEnemies[m_keys[0]]->getWidth() / 2), m_pEnemies[m_keys[0]]->getTransform()->position.y - (m_pEnemies[m_keys[0]]->getHeight() / 2)), m_pEnemies[m_keys[0]]->getWidth(), m_pEnemies[m_keys[0]]->getHeight(), black);
	}

	if (m_pEnemies[m_keys[1]] != nullptr)
	{
		Util::DrawRect(glm::vec2(m_pEnemies[m_keys[1]]->getTransform()->position.x - (m_pEnemies[m_keys[1]]->getWidth() / 2), m_pEnemies[m_keys[1]]->getTransform()->position.y - (m_pEnemies[m_keys[1]]->getHeight() / 2)), m_pEnemies[m_keys[1]]->getWidth(), m_pEnemies[m_keys[1]]->getHeight(), black);
	}
	/*SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);*/

	if (m_isGridEnabled == true)
	{
		m_pPlayer->drawLOS();
		m_pEnemies[m_keys[0]]->drawEnemyLOS();
		m_pEnemies[m_keys[1]]->drawEnemyLOS();
	}

	if (m_path_toggle == true)
	{
		// for the line that is connected between Player and Closest path_node
		Util::DrawLine(m_pPlayer->getTransform()->position, m_pPlayerClosest->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		// for the line that is connected between m_pPlayerClosest path_node and m_pCCEClosest
		Util::DrawLine(m_pPlayerClosest->getTransform()->position, m_pCCEClosest->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		// for the line that is connected between m_pCCEClosest path_node and Enemy
		Util::DrawLine(m_pCCEClosest->getTransform()->position, m_pEnemies[m_keys[0]]->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		// for the line that is connected between Player and Closest path_node
		Util::DrawLine(m_pPlayer->getTransform()->position, m_pPlayerClosest->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		// for the line that is connected between m_pPlayerClosest path_node and m_pCCEClosest
		Util::DrawLine(m_pPlayerClosest->getTransform()->position, m_pRCEClosest->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		// for the line that is connected between m_pCCEClosest path_node and Enemy
		Util::DrawLine(m_pRCEClosest->getTransform()->position, m_pEnemies[m_keys[1]]->getTransform()->position, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	}

	//m_pHealthBar->Render();
	//m_e1HealthBar->Render();
	//m_e2HealthBar->Render();
}

void PlayScene::update()
{
	updateDisplayList();

	std::cout << CollisionManager::circleAABBsquaredDistance(m_pPlayer->getTransform()->position, m_pPlayer->getLOSDistance(), m_pEnemies[m_keys[1]]->getTransform()->position, m_pEnemies[m_keys[1]]->getWidth(), m_pEnemies[m_keys[1]]->getHeight()) << std::endl;

	// Now for the path_nodes LOS
	switch (m_LOSMode)
	{
	case 0:
		m_checkAllNodesWithTarget(m_pEnemies[m_keys[0]]);
		m_checkAllNodesWithTarget(m_pEnemies[m_keys[1]]); //TODO: need to separate
		break;
	case 1:
		m_checkAllNodesWithTarget(m_pPlayer);
		break;
	case 2:
		m_checkAllNodesWithBoth();
		break;
	}

	/////////////////// Respawn///////////////////
	for (int i = 0; i < m_pEnemies.size(); i++)
	{
		if (m_pEnemies[m_keys[i]] != nullptr)
		{
			float enemyPosX = m_pEnemies[m_keys[i]]->getTransform()->position.x;
			if (enemyPosX > 835 || enemyPosX < -35)
			{
				Enemy* enemy = m_pEnemies[m_keys[i]];
				removeChild(enemy);
				m_pEnemies.erase(m_keys[i]);
				//delete enemy;
				//enemy = nullptr;
				if (i == 0)
				{
					m_pEnemies.emplace(m_keys[i], new CloseCombatEnemy(this));
					m_pEnemies[m_keys[i]]->getTransform()->position = glm::vec2(rand() % 750 + 50, 430.f);
					addChild(m_pEnemies[m_keys[i]], 3);
				}
				else if (i == 1)
				{
					m_pEnemies.emplace(m_keys[i], new RangedCombatEnemy(this));
					m_pEnemies[m_keys[i]]->getTransform()->position = glm::vec2(rand() % 750 + 50, 230.f);
					addChild(m_pEnemies[m_keys[i]], 3);
				}
			}
		}
		
	}
	/////////////////// Respawn///////////////////

	
	//std::cout << m_pEnemies[m_keys[1]]->getIsHit() << " " << m_pEnemies[m_keys[1]]->hasLOS() << " " << m_pEnemies[m_keys[1]]->getIsCovered() << std::endl;

	// Set agent tree conditions
	// Radius condition (distance check)
	// Close combat condition
	//m_pPlayer->getTree()->getLOSNode()->setLOS(m_pPlayer->checkAgentLOSToTarget(m_pPlayer, m_pEnemies, m_pObstacles));
	/*m_checkAgentLOS(m_pPlayer, m_pEnemies);*/

	if (m_pEnemies[m_keys[0]] != nullptr)
	{
		float CCE2Pdistance = Util::distance(m_pEnemies[m_keys[0]]->getTransform()->position, m_pPlayer->getTransform()->position);

		bool CCEisDetected = CCE2Pdistance <= m_pEnemies[m_keys[0]]->getLOSDistance();

		bool inClose = CCE2Pdistance <= 10;

		// For CloseCombatEnemy
		m_pEnemies[m_keys[0]]->getTree()->getEnemyHealthNode()->setHealth(m_pEnemies[m_keys[0]]->getHealth() > 25); // #1
		m_pEnemies[m_keys[0]]->getTree()->getEnemyHitNode()->setIsHit(false); // #2
		if (m_pEnemies[m_keys[0]]->getTree()->getPlayerDetectedNode()->getDetected() == false)
			m_pEnemies[m_keys[0]]->getTree()->getPlayerDetectedNode()->setDetected(CCEisDetected); // #1
		m_pEnemies[m_keys[0]]->checkAgentLOSToTarget(m_pEnemies[m_keys[0]], m_pPlayer, m_pObstacles); // #2
		m_pEnemies[m_keys[0]]->getTree()->getCloseCombatNode()->setIsWithinCombatRange(inClose); // #3
	}

	if (m_pEnemies[m_keys[1]] != nullptr)
	{
		float RCE2Pdistance = Util::distance(m_pEnemies[m_keys[1]]->getTransform()->position, m_pPlayer->getTransform()->position);

		bool RCEisDetected = RCE2Pdistance <= m_pEnemies[m_keys[1]]->getLOSDistance();

		bool inRange = RCE2Pdistance <= m_pEnemies[m_keys[1]]->getLOSDistance() - 30;  //200 && RCE2Pdistance <= 350;
		bool RCCisHit = m_pEnemies[m_keys[1]]->getIsHit();

		//// For RangedCombatEnemy
		m_pEnemies[m_keys[1]]->getTree()->getEnemyHealthNode()->setHealth(m_pEnemies[m_keys[1]]->getHealth() > 25); // #1
		m_pEnemies[m_keys[1]]->getTree()->getEnemyHitNode()->setIsHit(RCCisHit); // #2
		m_pEnemies[m_keys[1]]->getTree()->getPlayerDetectedNode()->setDetected(RCEisDetected); // #3
		m_pEnemies[m_keys[1]]->checkAgentLOSToTarget(m_pEnemies[m_keys[1]], m_pPlayer, m_pObstacles); // #4/#5
		m_pEnemies[m_keys[1]]->getTree()->getRangedCombatNode()->setIsWithinCombatRange(inRange); // #6

	}

	PathNode* temp1 = new PathNode;
	PathNode* temp2 = new PathNode;
	PathNode* temp3 = new PathNode;
	if (m_pEnemies.size() > 0)
	{
		for (auto path_node : m_pGrid)
		{
			if (path_node->hasLOS()) // check if path_node has LOS
			{
				if (m_pEnemies[m_keys[0]] != nullptr)
				{
					auto CCE2ODistance = Util::getClosestEdge(m_pEnemies[m_keys[0]]->getTransform()->position, path_node);
					auto A2ODistance = Util::getClosestEdge(m_pPlayer->getTransform()->position, path_node);

					if (A2ODistance < temp1->getLOSDistance()) // find what path_node is the m_pPlayerClosest from Player
					{
						temp1->setLOSDistance(A2ODistance);
						m_pPlayerClosest->getTransform()->position = temp1->getTransform()->position = path_node->getTransform()->position;
					}
					else if (CCE2ODistance < temp2->getLOSDistance()) // find what path_node is the m_pCCEClosest from Enemy
					{
						temp2->setLOSDistance(CCE2ODistance);
						m_pCCEClosest->getTransform()->position = temp2->getTransform()->position = path_node->getTransform()->position;
					}
				}

				if (m_pEnemies[m_keys[1]] != nullptr)
				{
					auto RCE2ODistance = Util::getClosestEdge(m_pEnemies[m_keys[1]]->getTransform()->position, path_node);
					auto A2ODistance = Util::getClosestEdge(m_pPlayer->getTransform()->position, path_node);

					if (A2ODistance < temp1->getLOSDistance()) // find what path_node is the m_pPlayerClosest from Player
					{
						temp1->setLOSDistance(A2ODistance);
						m_pPlayerClosest->getTransform()->position = temp1->getTransform()->position = path_node->getTransform()->position;
					}
					else if (RCE2ODistance < temp3->getLOSDistance()) // find what path_node is the m_pCCEClosest from Enemy
					{
						temp3->setLOSDistance(RCE2ODistance);
						m_pRCEClosest->getTransform()->position = temp3->getTransform()->position = path_node->getTransform()->position;
					}
				}
			}

		}
	}

	delete temp1;
	delete temp2;
	delete temp3;
	temp1 = nullptr;
	temp2 = nullptr;
	temp3 = nullptr;

	// Dagger(Enemy's Range attack) VS Player
	if (m_pEnemyDaggers.size() > 0)
	{
		for (unsigned int i = 0; i < m_pEnemyDaggers.size(); i++)
		{
			// Guide Dagger to Enemy

			// Player's Position -> PlayerClosest's position
			if (CollisionManager::squaredDistance(m_pEnemyDaggers[i]->getTransform()->position, m_pRCEClosest->getTransform()->position) < 500)
			{
				m_pEnemyDaggers[i]->setTargetPosition(m_pPlayerClosest->getTransform()->position);
			}
			// PlayerClosest's position -> EnemyClosest's position
			else if (CollisionManager::squaredDistance(m_pEnemyDaggers[i]->getTransform()->position, m_pPlayerClosest->getTransform()->position) < 1000)
			{
				m_pEnemyDaggers[i]->setTargetPosition(m_pPlayer->getTransform()->position);
			}

			// Collision check
			if (CollisionManager::AABBCheck(m_pEnemyDaggers[i], m_pPlayer))
			{
				m_pPlayer->setAnimationState(PLAYER_HURT_L);
				m_pPlayer->setHealth(m_pPlayer->getHealth()-25);
				SoundManager::Instance().playSound("hit", 0, -1);

				removeChild(m_pEnemyDaggers[i]);
				//delete m_pEnemyDaggers[i];
				//m_pEnemyDaggers[i] = nullptr;
				//m_pEnemyDaggers.erase(m_pEnemyDaggers.begin() + i);
				//m_pEnemyDaggers.shrink_to_fit();
			}
			//If Dagger is away from Player so far, remove Dagger
			//else if (CollisionManager::squaredDistance(m_pEnemyDaggers[i]->getTransform()->position, m_pEnemies[m_keys[1]]->getTransform()->position) > 50000)
			//{
			//	removeChild(m_pEnemyDaggers[i]);
			///*	delete m_pEnemyDaggers[i];*/
			///*	m_pEnemyDaggers[i] = nullptr;
			//	m_pEnemyDaggers.shrink_to_fit();*/
			//}
		}
	}

	// Dagger(Player's Range attack) VS Enemies
	if (!m_pPlayerDaggers.empty() && !m_pEnemies.empty())
	{
		for (unsigned int i = 0; i < m_pPlayerDaggers.size(); i++)
		{
			// Guide Dagger to Enemy

			// Player's Position -> PlayerClosest's position
			if (CollisionManager::squaredDistance(m_pPlayerDaggers[i]->getTransform()->position, m_pPlayerClosest->getTransform()->position) < 500)
			{
				m_pPlayerDaggers[i]->setTargetPosition(m_pRCEClosest->getTransform()->position);
			}
			// PlayerClosest's position -> EnemyClosest's position
			else if (CollisionManager::squaredDistance(m_pPlayerDaggers[i]->getTransform()->position, m_pRCEClosest->getTransform()->position) < 1000)
			{
				m_pPlayerDaggers[i]->setTargetPosition(m_pEnemies[m_keys[1]]->getTransform()->position);
			}

			// Collision check
			if (CollisionManager::AABBCheck(m_pPlayerDaggers[i], m_pEnemies[m_keys[1]]))
			{
				m_pEnemies[m_keys[1]]->setAnimationState(ENEMY_HURT_L);
				m_pEnemies[m_keys[1]]->takeDamage(25);
				SoundManager::Instance().playSound("hit", 0, -1);

				removeChild(m_pPlayerDaggers[i]);
				//delete m_pEnemyDaggers[i];
				//m_pEnemyDaggers[i] = nullptr;
				//m_pEnemyDaggers.erase(m_pEnemyDaggers.begin() + i);
				//m_pEnemyDaggers.shrink_to_fit();
			}
			//If Dagger is away from Player so far, remove Dagger
			//else if (CollisionManager::squaredDistance(m_pEnemyDaggers[i]->getTransform()->position, m_pEnemies[m_keys[1]]->getTransform()->position) > 50000)
			//{
			//	removeChild(m_pEnemyDaggers[i]);
			///*	delete m_pEnemyDaggers[i];*/
			///*	m_pEnemyDaggers[i] = nullptr;
			//	m_pEnemyDaggers.shrink_to_fit();*/
			//}
		}
	}

	if (CollisionManager::AABBCheck(m_pShields[0], m_pPlayer))
	{
		m_pShields[0]->setHealth(m_pShields[0]->getHealth() - 34);
		SoundManager::Instance().playSound("hit", 0, -1);
		removeChild(m_pShields[0]);
	}


	for (unsigned int i = 0; i < m_pEnemies.size(); i++)
	{
		if (m_pEnemies[m_keys[i]] != nullptr && m_pEnemies[m_keys[i]]->getHealth() <= 0)
		{
			Enemy* enemy = m_pEnemies[m_keys[i]];
			removeChild(enemy);
			m_pEnemies[m_keys[i]] = nullptr;
			m_pEnemies.erase(m_keys[i]);
		}
	}

	for (unsigned int i = 0; i < m_pEnemies.size(); i++)
	{
		if (m_pEnemies[m_keys[0]] != nullptr)
		{
			m_pPlayer->checkAgentLOSToTarget(m_pPlayer, m_pEnemies[m_keys[0]], m_pObstacles);
			m_pEnemies[m_keys[0]]->checkAgentLOSToTarget(m_pEnemies[m_keys[0]], m_pPlayer, m_pObstacles);
		}
			
		if (m_pEnemies[m_keys[1]] != nullptr)
		{
			m_pPlayer->checkAgentLOSToTarget(m_pPlayer, m_pEnemies[m_keys[1]], m_pObstacles);
			m_pEnemies[m_keys[1]]->checkAgentLOSToTarget(m_pEnemies[m_keys[1]], m_pPlayer, m_pObstacles);
		}
	}
	if (m_pEnemies[m_keys[1]] == nullptr && m_pEnemies[m_keys[0]] == nullptr)
	{
		TheGame::Instance().changeSceneState(END_SCENE);
	}
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	////////////////////// WASD ////////////////////// 
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
		m_pPlayer->getTransform()->position.y -= m_moveAmount;
		m_pPlayer->setCurrentHeading(270);
		m_pPlayer->setAnimationState(PLAYER_CLIMB);
	}
	else if (key == 'A')
	{
		m_pPlayer->getTransform()->position.x -= m_moveAmount;
		m_pPlayer->setCurrentHeading(180);
		isRight = false;
		m_pPlayer->setAnimationState(PLAYER_RUN_L);
	}
	else if (key == 'S')
	{
		m_pPlayer->getTransform()->position.y += m_moveAmount;
		m_pPlayer->setCurrentHeading(90);
		m_pPlayer->setAnimationState(PLAYER_CLIMB);
	}
	else if (key == 'D')
	{
		m_pPlayer->getTransform()->position.x += m_moveAmount;
		m_pPlayer->setCurrentHeading(0);
		isRight = true;
		m_pPlayer->setAnimationState(PLAYER_RUN_R);
	}
	else if (key == 'R')
	{
		// isRange Range Possible
		if ((CollisionManager::circleAABBsquaredDistance(m_pPlayer->getTransform()->position, m_pPlayer->getLOSDistance(), m_pEnemies[m_keys[0]]->getTransform()->position, m_pEnemies[m_keys[0]]->getWidth(), m_pEnemies[m_keys[0]]->getHeight()) <= 17000 && m_pPlayer->hasLOS())
			|| (CollisionManager::circleAABBsquaredDistance(m_pPlayer->getTransform()->position, m_pPlayer->getLOSDistance(), m_pEnemies[m_keys[1]]->getTransform()->position, m_pEnemies[m_keys[1]]->getWidth(), m_pEnemies[m_keys[1]]->getHeight()) <= 17000 && m_pPlayer->hasLOS()))
			
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

			if (m_pPlayerDaggers.size() < 1)
			{
				m_pPlayerDaggers.push_back(new Weapon(m_pPlayer->getTransform()->position));
				m_pPlayerDaggers.shrink_to_fit();
				m_pPlayerDaggers.back()->setIsMoving(true);
				m_pPlayerDaggers.back()->setTargetPosition(m_pPlayerClosest->getTransform()->position);
				addChild(m_pPlayerDaggers.back());
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
				SoundManager::Instance().playSound("knife");
				knifeSound = true;
			}
		}
		else if (isRight == true)
		{
			m_pPlayer->setAnimationState(PLAYER_MELEE_R);

			if (knifeSound == false)
			{
				SoundManager::Instance().playSound("knife");
				knifeSound = true;
			}
		}
		/* || (!m_pPlayer->getIsRight() && distanceCCE <= MELEE_DISTANCE_L))*/
		// isMelee Range Possible
		if(m_pEnemies.size() != 0)
		{
			if (m_pEnemies[m_keys[0]] != nullptr && m_pEnemies[m_keys[0]]->getHealth() > 0)
			{
				float distanceCCE = Util::distance(m_pPlayer->getTransform()->position, m_pEnemies[m_keys[0]]->getTransform()->position);
				if (((m_pPlayer->getIsRight() && distanceCCE <= MELEE_DISTANCE_R) || (!m_pPlayer->getIsRight() && distanceCCE <= MELEE_DISTANCE_L)))
				{
					m_pEnemies[m_keys[0]]->setAnimationState(ENEMY_HURT_L);
					/*time = 500;*/
					if (!m_isHit)
					{
						m_isHit = true;
						m_pEnemies[m_keys[0]]->takeDamage(25);
						m_pEnemies[m_keys[0]]->setIsHit(true);
					}


				}
			}
			if (m_pEnemies[m_keys[1]] != nullptr && m_pEnemies[m_keys[1]]->getHealth() > 0)
			{
				float distanceRCE = Util::distance(m_pPlayer->getTransform()->position, m_pEnemies[m_keys[1]]->getTransform()->position);
				if (((m_pPlayer->getIsRight() && distanceRCE <= MELEE_DISTANCE_R) || (!m_pPlayer->getIsRight() && distanceRCE <= MELEE_DISTANCE_L)))
				{
					m_pEnemies[m_keys[1]]->setAnimationState(ENEMY_HURT_L);
					/*time = 500;*/
					if (!m_isHit)
					{
						m_isHit = true;
						m_pEnemies[m_keys[1]]->takeDamage(25);
						m_pEnemies[m_keys[1]]->setIsHit(true);
					}
				}
			}
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
				if (m_isHit == true)
				{
					m_isHit = false;
				}
			}
			else if (isRight == true)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_R);

				if (knifeSound == true)
				{
					knifeSound = false;
				}
				if (m_isHit == true)
				{
					m_isHit = false;
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
	else if (m_pPlayer->getTransform()->position.y > HEIGHT - 25)
	{
		m_pPlayer->getTransform()->position.y = HEIGHT - 25;
	}

	////////////////////// WASD ////////////////////// 

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

	// player health
	if (EventManager::Instance().keyPressed(SDL_SCANCODE_EQUALS))
	{
		m_pPlayer->takeDamage(-25);
	}

	if (EventManager::Instance().keyPressed(SDL_SCANCODE_MINUS))
	{
		m_pPlayer->takeDamage(25);
	}

	// ranged enemy health
	if (EventManager::Instance().keyPressed(SDL_SCANCODE_0))
	{
		m_pEnemies[m_keys[0]]->takeDamage(-25);
		if (m_pEnemies[m_keys[0]]->getIsRight() == true)
		{
			m_pEnemies[m_keys[0]]->setAnimationState(ENEMY_HURT_R);
		}
		else
		{
			m_pEnemies[m_keys[0]]->setAnimationState(ENEMY_HURT_L);
		}
	}

	if (EventManager::Instance().keyPressed(SDL_SCANCODE_9))
	{
		m_pEnemies[m_keys[0]]->takeDamage(25);
		m_pEnemies[m_keys[0]]->setIsHit(true);
	}

	// CC enemy health
	if (EventManager::Instance().keyPressed(SDL_SCANCODE_8))
	{
		m_pEnemies[m_keys[1]]->takeDamage(-25);
		if (m_pEnemies[m_keys[1]]->getIsRight() == true)
		{					  
			m_pEnemies[m_keys[1]]->setAnimationState(ENEMY_HURT_R);
		}
		else
		{
			m_pEnemies[m_keys[1]]->setAnimationState(ENEMY_HURT_L);
		}
	}

	if (EventManager::Instance().keyPressed(SDL_SCANCODE_7))
	{
		m_pEnemies[m_keys[1]]->takeDamage(25);
		m_pEnemies[m_keys[1]]->setIsHit(true);
	}

	// Enemies movement will be toggled between idle and patrol
	if (EventManager::Instance().keyPressed(SDL_SCANCODE_P))
	{

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
}

void PlayScene::start()
{
	// Movement
	m_moveAmount = 10;

	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_isHit = false;
	
	m_pBG = new Background();
	addChild(m_pBG);

	m_pEnemies.emplace(m_keys[0], new CloseCombatEnemy(this));
	m_pEnemies[m_keys[0]]->getTransform()->position = glm::vec2(600.f, 430.f);
	addChild(m_pEnemies[m_keys[0]], 3);

	m_pEnemies.emplace(m_keys[1], new RangedCombatEnemy(this));
	m_pEnemies[m_keys[1]]->getTransform()->position = glm::vec2(600.f, 230.f);
	addChild(m_pEnemies[m_keys[1]], 3);

	m_pPlayer = new Player();
	m_pPlayer->getTransform()->position = glm::vec2(110.f, 230.f); // y550
	m_pPlayer->setTargetPosition(m_pEnemies[m_keys[0]]->getTransform()->position);
	m_pPlayer->setHealth(100);
	addChild(m_pPlayer, 2);

	//m_pHealthBar = new HealthBar({ 10, 10, 100, 10 }, 100);
	//m_e1HealthBar = new HealthBar({ 690, 10, 100, 10 }, 100);
	//m_e2HealthBar = new HealthBar({ 580, 10, 100, 10 }, 100);

	// New Obstacle creation
	std::ifstream inFile("../Assets/data/obstacles.txt");
	while (!inFile.eof())
	{
		Obstacle* obstacle = new Obstacle();
		float x, y, w, h;
		inFile >> x >> y >> w >> h;
		obstacle->getTransform()->position = glm::vec2(x, y);
		obstacle->setWidth(w);
		obstacle->setHeight(h);
		addChild(obstacle, 4);
		m_pObstacles.push_back(obstacle);
	}
	inFile.close();

	// Setup a few fields
	m_LOSMode = 1;
	m_obstacleBuffer = 0;
	m_pathNodeLOSDistance = 1000;
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);

	// For LOS
	m_pPlayerClosest = new PathNode();
	m_pCCEClosest = new PathNode();
	m_pRCEClosest = new PathNode();

	// Setup the grid
	m_isGridEnabled = false;
	m_path_toggle = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	// bools for sound effects
	knifeSound = false;
	knifeThrowingSound = false;

	SoundManager::Instance().load("../Assets/audio/Knife.flac", "knife", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/KnifeThrow.mp3", "KnifeThrow", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/Hit.wav", "Hit", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/yay.ogg", "win", SOUND_SFX);

	SoundManager::Instance().load("../Assets/audio/music.mp3", "music", SOUND_MUSIC);
	SoundManager::Instance().playMusic("music");
	SoundManager::Instance().setMusicVolume(4);
	SoundManager::Instance().setSoundVolume(8);

	SpawnShield();

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::SpawnRangedAttack()
{
	// Set spawn point and direction
	//glm::vec2 spawn_pos = m_pEnemies[m_keys[1]]->getTransform()->position + m_pEnemies[m_keys[1]]->getCurrentDirection() * 30.0f;
	//glm::vec2 steering_direction = m_pPlayer->getTransform()->position - spawn_pos;
	//steering_direction = Util::normalize(steering_direction);

	// Spawn it
	SoundManager::Instance().playSound("KnifeThrow");
	m_pEnemyDaggers.push_back(new Weapon(m_pEnemies[m_keys[1]]->getTransform()->position + m_pEnemies[m_keys[1]]->getCurrentDirection() * 30.0f));
	m_pEnemyDaggers.shrink_to_fit();
	m_pEnemyDaggers.back()->setIsMoving(true);
	m_pEnemyDaggers.back()->setTargetPosition(m_pRCEClosest->getTransform()->position);
	addChild(m_pEnemyDaggers.back());
}

void PlayScene::SpawnShield()
{
	glm::vec2 temp;
	temp.x = m_pEnemies[m_keys[1]]->getTransform()->position.x - m_pEnemies[m_keys[1]]->getCurrentDirection().x * 100.0f;
	temp.y = m_pEnemies[m_keys[1]]->getTransform()->position.y;

	m_pShields.push_back(new Shield(temp));
	m_pShields.shrink_to_fit();
	addChild(m_pShields.back());
	//if (m_pShields.size() < 1)
	//{
	//	glm::vec2 temp;
	//	temp.x = m_pEnemies[m_keys[1]]->getTransform()->position.x - m_pEnemies[m_keys[1]]->getCurrentDirection().x * 200.0f;
	//	temp.y = m_pEnemies[m_keys[1]]->getTransform()->position.y;
	//	m_pShields.push_back(new Shield(temp));
	//	m_pShields.shrink_to_fit();
	//	addChild(m_pShields.back());
	//	if (m_pEnemies[m_keys[1]]->getTransform()->position.x < temp.x)
	//	{
	//		m_pEnemies[m_keys[1]]->setIsRight(true);
	//		temp.x += 40;
	//	}
	//	else
	//	{
	//		m_pEnemies[m_keys[1]]->setIsRight(false);
	//		temp.x -= 40;
	//	}
	//	m_pEnemies[m_keys[1]]->setTargetPosition(temp);

	//	if (CollisionManager::squaredDistance(m_pEnemies[m_keys[1]]->getTransform()->position, temp) >= 100000)
	//	{
	//		m_pEnemies[m_keys[1]]->setIsCovered(true);
	//	}
	//	std::cout << CollisionManager::squaredDistance(m_pEnemies[m_keys[1]]->getTransform()->position, temp) << std::endl;
	//}
}

void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("GAME3001_A4", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	if (ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		m_toggleGrid(m_isGridEnabled);
	}

	ImGui::Separator();

	if (ImGui::Button("Node LOS to Target", { 300,20 }))
	{
		m_LOSMode = 0;
	}

	if (m_LOSMode == 0)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to SpaceShip", { 300,20 }))
	{
		m_LOSMode = 1;
	}

	if (m_LOSMode == 1)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to both Target and SpaceShip", { 300,20 }))
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

	shipPosition[0] = m_pPlayer->getTransform()->position.x; 
	shipPosition[1] = m_pPlayer->getTransform()->position.y;
	if (ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	{
		m_pPlayer->getTransform()->position.x = shipPosition[0];
		m_pPlayer->getTransform()->position.y = shipPosition[1];
	}

	// allow ship rotation
	angle = m_pPlayer->getCurrentHeading();
	if (ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		m_pPlayer->setCurrentHeading(angle);
	}

	ImGui::Separator();

	//static int targetPosition[] = { m_pEnemies->getTransform()->position.x, m_pEnemies->getTransform()->position.y };
	//if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	//{
	//	m_pEnemies->getTransform()->position.x = targetPosition[0];
	//	m_pEnemies->getTransform()->position.y = targetPosition[1];
	//}

	/*ImGui::Separator();*/

	//for (unsigned i = 0; i < m_pObstacles.size(); i++)
	//{
	//	int obsPosition[] = { m_pObstacles[i]->getTransform()->position.x, m_pObstacles[i]->getTransform()->position.y };
	//	std::string label = "Obstacle" + std::to_string(i+1) + " Position";
	//	if (ImGui::SliderInt2(label.c_str(), obsPosition, 0, 800))
	//	{
	//		m_pObstacles[i]->getTransform()->position.x = obsPosition[0];
	//		m_pObstacles[i]->getTransform()->position.y = obsPosition[1];
	//		m_buildGrid();
	//	}
	//}

	ImGui::Separator();

	if (ImGui::SliderInt("Obstacle Buffer", &m_obstacleBuffer, 0, 100))
	{
		m_buildGrid();
	}

	ImGui::End();
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

void PlayScene::m_buildGrid()
{
	auto tileSize = Config::TILE_SIZE;
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	m_clearNodes(); // Because we rebuild/redraw the grid if we move an obstacle

	// add path_nodes to the Grid
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

		if (m_pEnemies[m_keys[0]] != nullptr)
		{
			bool LOSWithCCE = m_checkPathNodeLOS(path_node, m_pEnemies[m_keys[0]]);
			path_node->setHasLOS((LOSWithSpaceShip && LOSWithCCE ? true : false));
		}

		if (m_pEnemies[m_keys[1]] != nullptr)
		{
			bool LOSWithRCE = m_checkPathNodeLOS(path_node, m_pEnemies[m_keys[1]]);
			path_node->setHasLOS((LOSWithSpaceShip && LOSWithRCE ? true : false));
		}
	}
}

bool PlayScene::m_checkAgentLOS(Agent* agent, DisplayObject* target_object)
{
	bool hasLOS = false;
	agent->setHasLOS(hasLOS);
	// if ship to target distance is less than or equal to LOS Distance
	auto AgentToTargetDistance = Util::getClosestEdge(agent->getTransform()->position, target_object);
	if (AgentToTargetDistance <= agent->getLOSDistance())
	{
		std::vector<DisplayObject*> contactList;
		for (auto object : getDisplayList())
		{
			if (object->getType() == NONE) continue; // Added Lab 7.
			auto AgentToObjectDistance = Util::getClosestEdge(agent->getTransform()->position, object);
			if (AgentToObjectDistance > AgentToTargetDistance) continue;
			if ((object->getType() != AGENT) && (object->getType() != PATH_NODE) && (object->getType() != TARGET))
			{
				contactList.push_back(object);
			}
		}
		const glm::vec2 agentEndPoint = agent->getTransform()->position + agent->getCurrentDirection() * agent->getLOSDistance();
		hasLOS = CollisionManager::LOSCheck(agent,
			agentEndPoint, contactList, target_object);
		agent->setHasLOS(hasLOS);
	}
	return hasLOS;
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