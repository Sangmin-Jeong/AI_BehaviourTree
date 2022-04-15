#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Heuristic.h"
#include "Scene.h"
#include "Target.h"
#include "PathNode.h"
#include "Obstacle.h"
#include "Background.h"
#include "DecisionTree.h"
#include "CloseCombatEnemy.h"
#include "Enemy.h"
#include "Player.h"
#include "RangedCombatEnemy.h"
#include "Torpedo.h"
#include "TorpedoK.h"

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

	// New lab 8
	void SpawnEnemyTorpedo();

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
	std::vector<Enemy*> m_pEnemies;
	PathNode* m_pPlayerClosest;
	PathNode* m_pEnemyClosest;
	
	std::vector<Obstacle*> m_pObstacles;
	std::vector<Torpedo*> m_pTorpedos;
	std::vector<TorpedoK*> m_pTorpedosK;

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

	
};

#endif /* defined (__PLAY_SCENE__) */