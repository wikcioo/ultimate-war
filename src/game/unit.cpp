#include "unit.h"

#include "core/logger.h"

std::unordered_map<UnitGroupType, UnitGroupData> UnitGroupDataMap = {
    {UnitGroupType::SWORDSMAN, {5,  {3,  4,  6}, "swordsman"}},
    {UnitGroupType::ARCHER,    {6,  {5,  2,  4},    "archer"}},
    {UnitGroupType::DWARF,     {7,  {4,  6,  8},     "dwarf"}},
    {UnitGroupType::DEMON,     {10, {8,  3,  7},     "demon"}},
    {UnitGroupType::HARPY,     {9,  {6,  1,  5},     "harpy"}}
};

UnitGroup::UnitGroup(UnitGroupType type)
    : m_Type(type), m_IsSelected(false)
{
    m_Stats.push_back(new UnitStats(UnitGroupDataMap[type].Stats));
}

void UnitGroup::IncrementQuantity(int quantity)
{
    if (quantity > 0)
        m_Stats.insert(m_Stats.end(), quantity, new UnitStats(UnitGroupDataMap[m_Type].Stats));
    else
        LOG_WARN("Trying to increment unit quantity with incorrect value: {0}", quantity);
}
