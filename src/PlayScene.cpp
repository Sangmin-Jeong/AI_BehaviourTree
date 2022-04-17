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
	for (auto element : m_pObstacles)
	{
		Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	}

	//for (auto element : m_pInvisObstacles)
	//{
	//	Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	//}

	//for (auto element : m_pLadders)
	//{
	//	Util::DrawRect(glm::vec2(element->getTransform()->position.x - (element->getWidth() / 2), element->getTransform()->position.y - (element->getHeight() / 2)), element->getWidth(), element->getHeight());
	//}

	Util::DrawRect(glm::vec2(m_pPlayer->getTransform()->position.x - (m_pPlayer->getWidth() / 2), m_pPlayer->getTransform()->position.y - (m_pPlayer->getHeight() / 2)), m_pPlayer->getWidth(), m_pPlayer->getHeight());
	Util::DrawRect(glm::vec2(m_pEnemies[m_keys[0]]->getTransform()->position.x - (m_pEnemies[m_keys[0]]->getWidth() / 2), m_pEnemies[m_keys[0]]->getTransform()->position.y - (m_pEnemies[m_keys[0]]->getHeight() / 2)), m_pEnemies[m_keys[0]]->getWidth(), m_pEnemies[m_keys[0]]->getHeight());
	Util::DrawRect(glm::vec2(m_pEnemies[m_keys[1]]->getTransform()->position.x - (m_pEnemies[m_keys[1]]->getWidth() / 2), m_pEnemies[m_keys[1]]->getTransform()->position.y - (m_pEnemies[m_keys[1]]->getHeight() / 2)), m_pEnemies[m_keys[1]]->getWidth(), m_pEnemies[m_keys[1]]->getHeight());

	

	drawDisplayList();

	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);

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
}

void PlayScene::update()
{
	updateDisplayList();
	// Set agent tree conditions
	// Radius condition (distance check)
	// Close combat condition
	//m_pPlayer->getTree()->getLOSNode()->setLOS(m_pPlayer->checkAgentLOSToTarget(m_pPlayer, m_pEnemies, m_pObstacles));
	/*m_checkAgentLOS(m_pPlayer, m_pEnemies);*/

	float CCE2Pdistance = Util::distance(m_pEnemies[m_keys[0]]->getTransform()->position, m_pPlayer->getTransform()->position);
	float RCE2Pdistance = Util::distance(m_pEnemies[m_keys[1]]->getTransform()->position, m_pPlayer->getTransform()->position);

	bool CCEisDetected = CCE2Pdistance <= m_pEnemies[m_keys[0]]->getLOSDistance();
	bool RCEisDetected = RCE2Pdistance <= m_pEnemies[m_keys[1]]->getLOSDistance();

	bool inClose = CCE2Pdistance <= 50;
	bool inRange = RCE2Pdistance <= 200 && RCE2Pdistance <= 350;

	// For CloseCombatEnemy
	if (m_pEnemies[m_keys[0]]->getTree()->getPlayerDetectedNode()->getDetected() == false)
		m_pEnemies[m_keys[0]]->getTree()->getPlayerDetectedNode()->setDetected(CCEisDetected); // #1
	m_pEnemies[m_keys[0]]->checkAgentLOSToTarget(m_pEnemies[m_keys[0]], m_pPlayer, m_pObstacles); // #2
	m_pEnemies[m_keys[0]]->getTree()->getCloseCombatNode()->setIsWithinCombatRange(inClose); // #3

	//// For RangedCombatEnemy
	m_pEnemies[m_keys[1]]->getTree()->getEnemyHealthNode()->setHealth(m_pEnemies[m_keys[1]]->getHealth() > 25); // #1
	m_pEnemies[m_keys[1]]->getTree()->getEnemyHitNode()->setIsHit(false); // #2
	m_pEnemies[m_keys[1]]->getTree()->getPlayerDetectedNode()->setDetected(RCEisDetected); // #3
	m_pEnemies[m_keys[1]]->checkAgentLOSToTarget(m_pEnemies[m_keys[1]], m_pPlayer, m_pObstacles); // #4/#5
	m_pEnemies[m_keys[1]]->getTree()->getRangedCombatNode()->setIsWithinCombatRange(inRange); // #6

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

	PathNode* temp1 = new PathNode;
	PathNode* temp2 = new PathNode;
	PathNode* temp3 = new PathNode;
	if (m_pEnemies.size() > 0)
	{
		for (auto path_node : m_pGrid)
		{
			if (path_node->hasLOS()) // check if path_node has LOS
			{
				auto CCE2ODistance = Util::getClosestEdge(m_pEnemies[m_keys[0]]->getTransform()->position, path_node);
				auto RCE2ODistance = Util::getClosestEdge(m_pEnemies[m_keys[1]]->getTransform()->position, path_node);
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
				else if (RCE2ODistance < temp3->getLOSDistance()) // find what path_node is the m_pCCEClosest from Enemy
				{
					temp3->setLOSDistance(RCE2ODistance);
					m_pRCEClosest->getTransform()->position = temp3->getTransform()->position = path_node->getTransform()->position;
				}
			}

		}
		// If path is clear or not
		//if (m_checkPathNodeLOS(m_pPlayerClosest, m_pPlayer) && m_checkPathNodeLOS(m_pPlayerClosest, m_pCCEClosest) && m_checkPathNodeLOS(m_pPlayerClosest, m_pCCEClosest))
		//{
		//	m_LOS_Clear = true;
		//}
		//else
		//{
		//	m_LOS_Clear = false;
		//}
	}
	delete temp1;
	delete temp2;
	delete temp3;
	temp1 = nullptr;
	temp2 = nullptr;
	temp3 = nullptr;
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

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

	//if (EventManager::Instance().keyPressed(SDL_SCANCODE_D)) // Damage Enemy
	//{
	//	m_pEnemies->takeDamage(25);
	//	std::cout << "Target at " << m_pEnemies->getHealth() << "%. " << std::endl;
	//	//m_pPlayer->getTree()->getEnemyHitNode()->setIsHit(true);
	//	//m_pPlayer->getTree()->getPlayerDetectedNode()->setDetected(true);
	//}

	//if (EventManager::Instance().keyPressed(SDL_SCANCODE_R)) // Reset Enemy conditions
	//{
	//	m_pEnemies->setHealth(100);
	//	//m_pPlayer->getTree()->getEnemyHitNode()->setIsHit(false);
	//	//m_pPlayer->getTree()->getPlayerDetectedNode()->setDetected(false);
	//	std::cout << "Target conditions reset" << std::endl;
	//}

	//if (EventManager::Instance().keyPressed(SDL_SCANCODE_F))
	//{
	//	m_pTorpedos.push_back(new Torpedo(5.0f));
	//	m_pTorpedos.back()->getTransform()->position = m_pEnemies->getTransform()->position;
	//	SoundManager::Instance().playSound("torpedo");
	//	addChild(m_pTorpedos.back(),2);
	//}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";
	
	m_pBG = new Background();
	addChild(m_pBG);

	m_pEnemies.emplace(m_keys[0], new CloseCombatEnemy(this));
	m_pEnemies[m_keys[0]]->getTransform()->position = glm::vec2(600.f, 430.f);
	addChild(m_pEnemies[m_keys[0]], 3);

	m_pEnemies.emplace(m_keys[1], new RangedCombatEnemy(this));
	m_pEnemies[m_keys[1]]->getTransform()->position = glm::vec2(600.f, 230.f);
	addChild(m_pEnemies[m_keys[1]], 3);

	m_pPlayer = new Player();
	m_pPlayer->getTransform()->position = glm::vec2(110.f, 430.f); // y550
	m_pPlayer->setTargetPosition(m_pEnemies[m_keys[0]]->getTransform()->position);
	addChild(m_pPlayer, 2);

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


	SoundManager::Instance().load("../Assets/audio/yay.ogg", "yay", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/thunder.ogg", "boom", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/torpedo.ogg", "torpedo", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/torpedo_k.ogg", "torpedo_k", SOUND_SFX);

	//SoundManager::Instance().load("../Assets/audio/mutara.mp3", "mutara", SOUND_MUSIC);
	//SoundManager::Instance().playMusic("mutara");
	SoundManager::Instance().load("../Assets/audio/klingon.mp3", "klingon", SOUND_MUSIC);
	SoundManager::Instance().playMusic("klingon");
	SoundManager::Instance().setMusicVolume(16);
	SoundManager::Instance().setSoundVolume(16);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::SpawnEnemyTorpedo()
{
	//// Set spawn point and direction
	//glm::vec2 spawn_pos = m_pPlayer->getTransform()->position + m_pPlayer->getCurrentDirection() * 30.0f;
	//glm::vec2 steering_direction = m_pEnemies->getTransform()->position - spawn_pos;
	//steering_direction = Util::normalize(steering_direction);

	//// Spawn it
	//m_pTorpedosK.push_back(new TorpedoK(5.0f, steering_direction));
	//m_pTorpedosK.back()->getTransform()->position = spawn_pos;
	//SoundManager::Instance().playSound("torpedo_k");
	//addChild(m_pTorpedosK.back(), 2);
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
		bool LOSWithCCE = m_checkPathNodeLOS(path_node, m_pEnemies[m_keys[0]]);
		bool LOSWithRCE = m_checkPathNodeLOS(path_node, m_pEnemies[m_keys[1]]);
		path_node->setHasLOS((LOSWithSpaceShip && LOSWithCCE && LOSWithRCE ? true : false));
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