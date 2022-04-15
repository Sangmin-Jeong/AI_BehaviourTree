#include "TileGrid.h"
#include <fstream>
#include <iostream>
#include <map>

#include "TextureManager.h"

TileGrid::TileGrid(std::string level_data)
{
	

	std::ifstream file;
	file.open("../Assets/data/" + level_data);

	std::string level_layout;
	std::string tile_data;
	std::string texture_file_name;


	if (file.is_open())
	{

		int width, height;
		int rows, columns;
		glm::vec2 start;

		file >> width >> height >> rows >> columns >> start.x >> start.y;
		m_rows = rows;
		m_colums = columns;
		m_tileHeight = height;
		m_tileWidth = width;
		getTransform()->position = start;

		std::getline(file, tile_data); // To skip \n in the end of the line

		std::getline(file, tile_data);
		std::getline(file, level_layout);
		std::getline(file, texture_file_name);

	}
	else
	{
		std::cout << "Cannot open " << level_data << std::endl;
	}

	TextureManager::Instance().load("../Assets/textures/" + texture_file_name, "tiles");

	file.close();
	file.open("../Assets/data/" + tile_data);
	std::map<char, Tile*> tiles;
	if (file.is_open())
	{
		
		char key;
		int x, y;
		bool obs;

		while (!file.eof())
		{
			file >> key >> x >> y >> obs;
			tiles.emplace(key, new Tile({ x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight}, "tiles", obs));
		}
	}
	else
	{
		std::cout << "Cannot open " << level_data << std::endl;
	}

	file.close();
	file.open("../Assets/data/" + level_layout);
	if (file.is_open())
	{
		auto current_pos_x = getTransform()->position.x - (m_colums / 2.0f) * m_tileWidth + m_tileWidth / 2;
		auto current_pos_y = getTransform()->position.y - (m_rows / 2.0f) * m_tileHeight + m_tileHeight / 2;

		char key;
		m_pTiles.resize(m_rows);
		for (unsigned short row = 0; row < m_rows; row++)
		{
			m_pTiles[row].resize(m_colums);
			for (unsigned short col = 0; col < m_colums; col++)
			{
				file >> key;
				m_pTiles[row][col] = tiles[key]->Clone();
				m_pTiles[row][col]->getTransform()->position = glm::vec2(current_pos_x, current_pos_y);

				getParent()->addChild(m_pTiles[row][col], BACKGROUND);

				current_pos_x += m_tileWidth;
				//std::cout << m_pTiles[row][col]->getTransform()->position.x << " " << m_pTiles[row][col]->getTransform()->position.y << std::endl;
			}
			current_pos_y += m_tileHeight;
			current_pos_x = getTransform()->position.x - (m_colums / 2.0f) * m_tileWidth + m_tileWidth / 2;
		}
	}
	else
	{
		std::cout << "Cannot open " << level_data << std::endl;
	}

}

TileGrid::~TileGrid()
= default;

void TileGrid::update()
{
}

void TileGrid::draw()
{
}

void TileGrid::clean()
{
}

