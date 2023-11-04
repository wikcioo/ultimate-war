#pragma once

#include <string>
#include <unordered_map>

#include "game/resource.h"

enum class BuildingType
{
    TARGET,
    BLACKSMITH,
    GOLD_MINE,
    HARPY_TOWER,
    DEMON_CASTLE,
    COUNT,
    NONE
};

struct BuildingData
{
    Resources Cost;
    Resources BaseUpgradeCost;
    std::string TextureName;
};

extern std::unordered_map<BuildingType, BuildingData> BuildingDataMap;

class Building
{
public:
    Building(BuildingType type);
    ~Building() = default;

    void Upgrade() { m_Level += 1; }

    const BuildingType GetType() const { return m_Type; }
    const unsigned int GetLevel() const { return m_Level; }

private:
    BuildingType m_Type;
    unsigned int m_Level;
};
