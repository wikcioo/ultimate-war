#include "building.h"

std::unordered_map<BuildingType, BuildingData> BuildingDataMap = {
    { BuildingType::WORKSHOP,     { { 20, 15, 10,  5 },     "workshop" } },
    { BuildingType::GOLD_MINE,    { { 10, 25, 15, 40 },    "gold_mine" } },
    { BuildingType::HARPY_TOWER,  { { 25, 20, 10, 35 },  "harpy_tower" } },
    { BuildingType::DEMON_CASTLE, { { 30, 40, 45, 50 }, "demon_castle" } },
    { BuildingType::DRAGON_LAIR,  { { 40, 30, 25, 45 },  "dragon_lair" } }
};

Building::Building(BuildingType type)
    : m_Type(type)
{
}
