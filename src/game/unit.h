#pragma once

#include <string>
#include <unordered_map>

enum class UnitType
{
    SWORDSMAN,
    ARCHER,
    DWARF,
    DEMON,
    HARPY,
    COUNT,
    NONE
};

struct UnitStats
{
    int m_Quantity;
    int m_Attack;
    int m_UnitHealth;
};

struct UnitData
{
    int Cost;
    UnitStats Stats;
    std::string TextureName;
};

extern std::unordered_map<UnitType, UnitData> UnitDataMap;

class Unit
{
public:
    Unit(UnitType type);
    ~Unit() = default;

    void ToggleSelected() { m_IsSelected = !m_IsSelected; }
    void SetSelected(bool isSelected) { m_IsSelected = isSelected; }

    const UnitType GetType() const { return m_Type; }
    const bool IsSelected() const { return m_IsSelected; }

private:
    UnitType m_Type;
    UnitStats m_Stats;
    bool m_IsSelected;
};
