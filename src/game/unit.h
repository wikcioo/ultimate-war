#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

#include "game/building.h"
#include "game/resource.h"

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

    UnitStats operator+(int scalar);
};

struct UnitGroupData
{
    Resources Cost;
    UnitStats Stats;
    std::string TextureName;
    BuildingType RequiredBuilding;
};

extern std::unordered_map<UnitGroupType, UnitGroupData> UnitGroupDataMap;

class UnitGroup
{
public:
    UnitGroup(UnitGroupType type, std::optional<UnitStats*> stats = std::nullopt);
    ~UnitGroup() = default;

    void ToggleSelected() { m_IsSelected = !m_IsSelected; }
    void SetSelected(bool isSelected) { m_IsSelected = isSelected; }
    void SetMovedOnIteration(int iteration) { m_MovedOnIteration = iteration; }

    // TODO: Rethink
    void IncrementQuantity(int quantity = 1);

    std::vector<UnitStats*>& GetUnitStats() { return m_Stats; }
    const UnitGroupType GetType() const { return m_Type; }
    const bool IsSelected() const { return m_IsSelected; }
    const bool UnitWasMovedInIteration(int iteration) { return iteration == m_MovedOnIteration; }

private:
    UnitGroupType m_Type;
    int m_MovedOnIteration;
    std::vector<UnitStats*> m_Stats;
    bool m_IsSelected;
};
