#include "unit.h"

#include "core/logger.h"

std::unordered_map<UnitGroupType, UnitGroupData> UnitGroupDataMap = {
    {UnitGroupType::SWORDSMAN, {10, {6,   3,  5}, "swordsman"}},
    {UnitGroupType::ARCHER,    {8,  {5,   5,  3},    "archer"}},
    {UnitGroupType::DWARF,     {7,  {3,   3,  3},     "dwarf"}},
    {UnitGroupType::DEMON,     {11, {2,  10,  4},     "demon"}},
    {UnitGroupType::HARPY,     {9,  {10,  1,  2},     "harpy"}}
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
