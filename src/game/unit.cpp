#include "unit.h"

#include "core/logger.h"

std::unordered_map<UnitGroupType, UnitGroupData> UnitGroupDataMap = {
    { UnitGroupType::SWORDSMAN, { { 3, 2, 1, 3 }, { 3,  4,  6 }, "swordsman", BuildingType::NONE         } },
    { UnitGroupType::ARCHER,    { { 4, 3, 2, 4 }, { 5,  2,  4 },    "archer", BuildingType::TARGET       } },
    { UnitGroupType::DWARF,     { { 2, 4, 6, 3 }, { 4,  6,  8 },     "dwarf", BuildingType::BLACKSMITH   } },
    { UnitGroupType::DEMON,     { { 0, 0, 3, 8 }, { 8,  3,  7 },     "demon", BuildingType::DEMON_CASTLE } },
    { UnitGroupType::HARPY,     { { 3, 3, 3, 5 }, { 6,  1,  5 },     "harpy", BuildingType::HARPY_TOWER  } }
};

UnitGroup::UnitGroup(UnitGroupType type, std::optional<UnitStats*> stats)
    : m_Type(type), m_IsSelected(false)
{
    auto unitStats = stats.has_value() ? stats.value() : new UnitStats(UnitGroupDataMap[type].Stats);
    m_Stats.push_back(unitStats);
}

void UnitGroup::IncrementQuantity(int quantity)
{
    if (quantity > 0)
        m_Stats.insert(m_Stats.end(), quantity, new UnitStats(UnitGroupDataMap[m_Type].Stats));
    else
        LOG_WARN("Trying to increment unit quantity with incorrect value: {0}", quantity);
}
