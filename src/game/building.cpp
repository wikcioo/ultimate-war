#include "building.h"

std::unordered_map<BuildingType, BuildingData> BuildingDataMap = {
    { BuildingType::TARGET,       { { 40, 30, 25, 45 }, { 8, 6, 4, 8 },       "target" } },
    { BuildingType::BLACKSMITH,   { { 20, 15, 15, 20 }, { 4, 2, 2, 2 },   "blacksmith" } },
    { BuildingType::GOLD_MINE,    { { 10, 25, 15,  5 }, { 2, 4, 2, 8 },    "gold_mine" } },
    { BuildingType::HARPY_TOWER,  { { 25, 20, 10, 35 }, { 4, 4, 2, 6 },  "harpy_tower" } },
    { BuildingType::DEMON_CASTLE, { { 30, 40, 45, 50 }, { 6, 8, 8, 9 }, "demon_castle" } }
};

Building::Building(BuildingType type)
    : m_Type(type), m_Level(0)
{
}
