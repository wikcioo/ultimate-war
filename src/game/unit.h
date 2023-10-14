#pragma once

#include <string>
#include <vector>
#include <unordered_map>

enum class UnitGroupType
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
    int Attack;
    int Defense;
    int Health;
};

struct UnitGroupData
{
    int Cost;
    UnitStats Stats;
    std::string TextureName;
};

extern std::unordered_map<UnitGroupType, UnitGroupData> UnitGroupDataMap;

class UnitGroup
{
public:
    UnitGroup(UnitGroupType type);
    ~UnitGroup() = default;

    void ToggleSelected() { m_IsSelected = !m_IsSelected; }
    void SetSelected(bool isSelected) { m_IsSelected = isSelected; }

    // TODO: Rethink
    void IncrementQuantity(int quantity = 1);

    std::vector<UnitStats*>& GetUnitStats() { return m_Stats; }
    const UnitGroupType GetType() const { return m_Type; }
    const bool IsSelected() const { return m_IsSelected; }

private:
    UnitGroupType m_Type;
    std::vector<UnitStats*> m_Stats;
    bool m_IsSelected;
};
