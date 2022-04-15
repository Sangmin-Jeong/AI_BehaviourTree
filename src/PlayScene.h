#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Heuristic.h"
#include "Scene.h"
#include "Enemy.h"
#include "Player.h"
#include "Tile.h"
#include "Obstacle.h"
#include "InvisObstacle.h"
#include "Label.h"
#include "PathNode.h"
#include "PlayerHealth.h"
#include "EnemyHealth.h"
#include "Ladder.h"
#include "Weapon.h"

#define MELEE_DISTANCE_L 25
#define MELEE_DISTANCE_R 5

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

private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;

	// Game Objects
	Enemy* m_pEnemy;
	Player* m_pPlayer;
	PathNode* m_pPlayerClosest;
	PathNode* m_pEnemyClosest;
	std::vector<Weapon*> m_pDaggers;

	// Player Health
	PlayerHealth* m_pPlayerHealth;
	EnemyHealth* m_pEnemyHealth;

	// LOS Objects and Functions
	std::vector<PathNode*> m_pGrid;
	std::vector<Obstacle*> m_pObstacles;
	std::vector<InvisObstacle*> m_pInvisObstacles;
	std::vector<Ladder*> m_pLadders;
	void m_buildGrid();
	void m_toggleGrid(bool state);
	bool m_checkAgentLOS(Agent* agent, DisplayObject* target_object);
	bool m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object);
	void m_checkAllNodesWithTarget(DisplayObject* target_object);
	void m_checkAllNodesWithBoth();
	void m_clearNodes();
	void m_setPathNodeLOSDistance(int dist);


	Label* m_pScoreLabel{};

	int m_LOSMode; // 0 = nodes visible to target, 1 = to player, 2 = to both
	int m_obstacleBuffer;
	int m_pathNodeLOSDistance;
	int m_moveAmount;
	static inline bool m_path_toggle;
	int m_score = 0;
	bool m_LOS_Clear;

	// WASD
	std::vector <char> keyList;
	char key;
	bool isRight;
	bool isMeleeDamageTaken;
	bool enemyIsDead;
	bool playerIsDead;
	bool knifeSound;
	bool knifeThrowingSound;
	bool isMoving;
	bool isPatrolling;

	bool CheckKeyList(char c);
	bool DeleteKeyList(char c);

	// Time
	float time;
	float countTimer;
};

#endif /* defined (__PLAY_SCENE__) */