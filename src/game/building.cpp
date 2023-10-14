#include "building.h"

std::unordered_map<BuildingType, BuildingData> BuildingDataMap = {
    {BuildingType::WORKSHOP,     {20,     "workshop"}},
    {BuildingType::GOLD_MINE,    {30,    "gold_mine"}},
    {BuildingType::HARPY_TOWER,  {40,  "harpy_tower"}},
    {BuildingType::DEMON_CASTLE, {40, "demon_castle"}},
    {BuildingType::DRAGON_LAIR,  {50,  "dragon_lair"}}
};

Building::Building(BuildingType type)
    : m_Type(type)
{
}
