#pragma once
#ifndef __TILE_GRID__
#define __TILE_GRID__
#include "DisplayObject.h"
#include "Tile.h"
#include <vector>

class TileGrid : public DisplayObject
{
private:
	int m_rows;
	int m_colums;
	int m_tileWidth;
	int m_tileHeight;

	std::vector<std::vector<Tile*>> m_pTiles;


public:
	TileGrid(std::string level_data);
	~TileGrid();

	void update() override;
	void draw() override;
	void clean() override;

private:

};
#endif 