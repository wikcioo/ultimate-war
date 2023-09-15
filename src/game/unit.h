#pragma once

#include <string>
#include <unordered_map>

enum class UnitType
{
    NONE = 0,
    SWORDSMAN,
    ARCHER,
    DWARF,
    DEMON,
    HARPY
};

struct UnitStats
{
    int m_Quantity;
    int m_Attack;
    int m_UnitHealth;
};

extern std::unordered_map<UnitType, std::string> UnitTextureMap;
extern std::unordered_map<UnitType, UnitStats> UnitStatMap;

class Unit
{
public:
    Unit(UnitType type);
    ~Unit() = default;

    const UnitType GetType() const { return m_Type; }

private:
    UnitType m_Type;
    UnitStats m_Stats;
};
