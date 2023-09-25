#include "map.h"

#include "game/tile.h"
#include "core/file_system.h"


static Tile g_BadTile(0, {0.0f, 0.0f});

GameMap::GameMap(MapData& mapData)
    : m_MapData(mapData)
{
}


Tile* GameMap::GetTile(int x, int y)
{
    if (x < GetTileCountX() && y < GetTileCountY() && x > -1 && y > -1)
        return m_MapData[y][x];

    auto pos = Tile::CalculateTilePosition(x, y);
    g_BadTile = Tile(0, pos);
    return &g_BadTile;
}

int GameMap::CalculatePlayerIncome(int playerID)
{
    int total = 0;
    for (int y = 0; y < GetTileCountY(); y++)
    {
        for (int x = 0; x < GetTileCountX(); x++)
        {
            Tile* tile = GetTile(x, y);
            if(tile->GetPlayerID() == playerID)
            {
                total += tile->GetIncomeValue();
            }
        }
    }

    return total;
}
