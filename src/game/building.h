#pragma once

#include <string>
#include <unordered_map>

enum class BuildingType
{
    WORKSHOP,
    GOLD_MINE,
    DRAGON_LAIR,
    COUNT,
    NONE
};

extern std::unordered_map<BuildingType, std::string> BuildingTextureMap;

class Building
{
public:
    Building(BuildingType type);
    ~Building() = default;

    const BuildingType GetType() const { return m_Type; }

private:
    BuildingType m_Type;
};
