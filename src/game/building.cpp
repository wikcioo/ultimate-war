#include "building.h"

std::unordered_map<BuildingType, BuildingData> BuildingDataMap = {
    {BuildingType::WORKSHOP,    {30,    "workshop"}},
    {BuildingType::GOLD_MINE,   {40,   "gold_mine"}},
    {BuildingType::DRAGON_LAIR, {50, "dragon_lair"}}
};

Building::Building(BuildingType type)
    : m_Type(type)
{
}
