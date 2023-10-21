#pragma once

#include <string>
#include <unordered_map>

#include "game/resource.h"

enum class BuildingType
{
    WORKSHOP,
    GOLD_MINE,
    HARPY_TOWER,
    DEMON_CASTLE,
    DRAGON_LAIR,
    COUNT,
    NONE
};

struct BuildingData
{
    Resources Cost;
    std::string TextureName;
};

extern std::unordered_map<BuildingType, BuildingData> BuildingDataMap;

class Building
{
public:
    Building(BuildingType type);
    ~Building() = default;

    const BuildingType GetType() const { return m_Type; }

private:
    BuildingType m_Type;
};
