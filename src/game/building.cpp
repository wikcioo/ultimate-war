#include "building.h"

std::unordered_map<BuildingType, std::string> BuildingTextureMap = {
    {BuildingType::WORKSHOP, "workshop"},
    {BuildingType::GOLD_MINE, "gold_mine"},
    {BuildingType::DRAGON_LAIR, "dragon_lair"}
};

Building::Building(BuildingType type)
    : m_Type(type)
{
}
