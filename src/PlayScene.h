#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Heuristic.h"
#include "Scene.h"
#include "PathNode.h"
#include "Obstacle.h"
#include "Background.h"
#include "HealthBar.h"
#include "DecisionTree.h"
#include "CloseCombatEnemy.h"
#include "Enemy.h"
#include "Player.h"
#include "RangedCombatEnemy.h"
#include "Shield.h"
#include <map>

#include "Weapon.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

	void SpawnRangedAttack();
	void SpawnShield();

	//Getter
	PathNode* GetPlayerClosest() {return m_pPlayerClosest;}
	PathNode* GetCCEClosest() { return m_pCCEClosest; }
	PathNode* GetRCEClosest() { return m_pRCEClosest; }
	Player* GetPlayer() { return m_pPlayer; }
	Enemy* GetCCE() { return m_pEnemies[m_keys[0]]; }
	Enemy* GetRCE() { return m_pEnemies[m_keys[1]]; }
	Shield* GetShield() { return m_pShields.back(); }

private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;
	// New for Lab 7
	int shipPosition[2];
	int angle;

	// Game Objects
	Background* m_pBG;
	Player* m_pPlayer;
	std::map<std::string, Enemy*> m_pEnemies;
	std::string m_keys[2] = { "CCE", "RCE" };
	PathNode* m_pPlayerClosest;
	PathNode* m_pCCEClosest;
	PathNode* m_pRCEClosest;

	HealthBar* m_pHealthBar;
	HealthBar* m_e1HealthBar;
	HealthBar* m_e2HealthBar;
	
	std::vector<Obstacle*> m_pObstacles;
	std::vector<Weapon*> m_pEnemyDaggers;
	std::vector<Shield*> m_pShields;

	// PathNode Objects and Functions
	std::vector<PathNode*> m_pGrid;
	void m_buildGrid();
	void m_toggleGrid(bool state);
	bool m_checkAgentLOS(Agent* agent, DisplayObject* target_object);
	bool m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object);
	void m_checkAllNodesWithTarget(DisplayObject* target_object);
	void m_checkAllNodesWithBoth();
	void m_clearNodes();
	void m_setPathNodeLOSDistance(int dist);

	// IMGUI
	int m_LOSMode; // 0 = nodes visible to target, 1 = nodes visible to player, 2 = nodes visible to both
	int m_obstacleBuffer;
	int m_pathNodeLOSDistance;
	static inline bool m_path_toggle;
	bool m_LOS_Clear;

	// WASD
	std::vector <char> keyList;
	char key;
	bool isRight;
	int m_moveAmount;
	bool isMoving;
	bool CheckKeyList(char c);
	bool DeleteKeyList(char c);

	// Health color
	glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 yellow = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);;
};

#endif /* defined (__PLAY_SCENE__) */