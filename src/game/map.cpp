#include "map.h"

#include "game/tile.h"

GameMap::GameMap(MapData& mapData)
    : m_MapData(mapData)
{
}

const std::shared_ptr<Tile>& GameMap::GetTile(int x, int y)
{
    if (x < GetTileCountX() && y < GetTileCountY() && x > -1 && y > -1)
        return m_MapData[y][x];

    static auto tile = std::make_shared<Tile>(TileEnvironment::NONE, glm::ivec2(x, y));
    return tile;
}
