#include "building.h"

std::unordered_map<BuildingType, BuildingData> BuildingDataMap = {
    { BuildingType::TARGET,       { { 40, 30, 25, 45 },       "target" } },
    { BuildingType::BLACKSMITH,   { { 20, 15, 10,  5 },   "blacksmith" } },
    { BuildingType::GOLD_MINE,    { { 10, 25, 15, 40 },    "gold_mine" } },
    { BuildingType::HARPY_TOWER,  { { 25, 20, 10, 35 },  "harpy_tower" } },
    { BuildingType::DEMON_CASTLE, { { 30, 40, 45, 50 }, "demon_castle" } }
};

Building::Building(BuildingType type)
    : m_Type(type)
{
}
