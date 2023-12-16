#include "ai.h"

#include <numeric>
#include <vector>

#include "game/tile.h"
#include "game/game_layer.h"
#include "widgets/notification.h"

const int MAX_INT = std::numeric_limits<int>::max();

AI *AI::s_Instance = nullptr;

AI::AI() { s_Instance = this; }

void AI::MakeMove(const std::shared_ptr<Player> &player)
{
    auto gameLayer = GameLayer::Get();

    auto tiles = player->GetOwnedTiles();
    auto map = gameLayer.GetGameMapManager();

    std::vector<std::shared_ptr<Tile>> tilesToPlaceUnitGroupsOn{};
    for (auto tile : tiles)
    {
        auto coords = tile->GetCoords();
        int offset = 0;
        if (coords.x % 2 == 0)
            offset = -1;

        bool hasNeighboringOpponent = false;
        std::shared_ptr<Tile> targetTile;
        UnitStats minUnitStats = { MAX_INT, MAX_INT, MAX_INT };
        for (auto tileOffset : Tile::s_AdjacentTileOffsets)
        {
            glm::ivec2 location;
            if (tileOffset.x != 0)
            {
                location = glm::ivec2(coords.x + tileOffset.x, coords.y + offset + tileOffset.y);
            }
            else
            {
                location = glm::ivec2(coords.x + tileOffset.x, coords.y + tileOffset.y);
            }

            auto adjTile = map->GetGameMap()->GetTile(location.x, location.y);
            if (adjTile->AssetsCanExist() && adjTile->GetOwnedBy() != player)
            {
                UnitStats stats = adjTile->GetTotalUnitStats();
                if (minUnitStats > stats)
                {
                    hasNeighboringOpponent = true;
                    minUnitStats = stats;
                    targetTile = adjTile;
                }
            }
        }

        if (hasNeighboringOpponent)
        {
            if (tile->GetTotalUnitStats() > minUnitStats)
            {
                tile->SelectAllUnitGroups();
                tile->MoveToTile(targetTile);
            }
            else
            {
                tilesToPlaceUnitGroupsOn.push_back(tile);
            }
        }
    }

    auto unitGroupType = UnitGroupType::SWORDSMAN;
    while (tilesToPlaceUnitGroupsOn.size() > 0)
    {
        for (int i = 0; i < tilesToPlaceUnitGroupsOn.size(); i++)
        {
            auto tile = tilesToPlaceUnitGroupsOn[i];

            if (tile->CanRecruitUnitGroup(unitGroupType) &&
                tile->HasSpaceForUnitGroups(1) &&
                player->SubtractResources(UnitGroupDataMap[unitGroupType].Cost))
            {
                tile->CreateUnitGroup(unitGroupType);
            }
            else
            {
                tilesToPlaceUnitGroupsOn[i] = nullptr;
            }
        }

        tilesToPlaceUnitGroupsOn.erase(
            std::remove(tilesToPlaceUnitGroupsOn.begin(), tilesToPlaceUnitGroupsOn.end(), nullptr),
            tilesToPlaceUnitGroupsOn.end()
        );
    }

    std::ostringstream oss;
    oss << player->GetName() << " made a move!";
    Notification::Create(oss.str(), NotificationLevel::INFO);
}
