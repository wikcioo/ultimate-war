#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "game/tile.h"

typedef std::vector<std::vector<Tile*>> MapData;

class GameMap
{
public:
    GameMap(MapData& mapData);
    ~GameMap() = default;

    inline int GetTileCountX() const { return !m_MapData.empty() ? m_MapData[0].size() : 0; }
    inline int GetTileCountY() const { return m_MapData.size(); }

    Tile* GetTile(int x, int y);
    int CalculatePlayerIncome(int playerID);

private:
    MapData m_MapData;
};
