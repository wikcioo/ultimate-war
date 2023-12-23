#include "building.h"

std::unordered_map<BuildingType, BuildingData> BuildingDataMap = {
    { BuildingType::GOLD_MINE,    { { 10, 25, 15, 10 }, { 3, 4, 3, 5 },    "gold_mine" } },
    { BuildingType::TARGET,       { { 30, 30, 25, 25 }, { 2, 4, 4, 3 },       "target" } },
    { BuildingType::BLACKSMITH,   { { 20, 35, 35, 20 }, { 4, 3, 3, 5 },   "blacksmith" } },
    { BuildingType::HARPY_TOWER,  { { 25, 20, 10, 35 }, { 4, 4, 3, 6 },  "harpy_tower" } },
    { BuildingType::DEMON_CASTLE, { { 30, 30, 35, 40 }, { 3, 6, 6, 5 }, "demon_castle" } }
};

Building::Building(BuildingType type)
    : m_Type(type), m_Level(0)
{
}

Resources Building::GetUpgradeCost() const
{
    return BuildingDataMap[m_Type].BaseUpgradeCost * pow(m_Level + 1, 1.2);
}
