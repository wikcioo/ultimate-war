#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "game/tile.h"

typedef std::vector<std::vector<std::shared_ptr<Tile>>> MapData;

class GameMap
{
public:
    GameMap(MapData& mapData);
    ~GameMap() = default;

    inline int GetTileCountX() const { return !m_MapData.empty() ? m_MapData[0].size() : 0; }
    inline int GetTileCountY() const { return m_MapData.size(); }

    std::shared_ptr<Tile> GetTile(int x, int y);

private:
    MapData m_MapData;
};
