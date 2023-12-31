#include "unit.h"

#include "core/logger.h"

std::unordered_map<UnitGroupType, UnitGroupData> UnitGroupDataMap = {
    { UnitGroupType::SWORDSMAN, { { 2, 2, 2, 3 }, { 3,  4,  6 }, "swordsman", BuildingType::NONE         } },
    { UnitGroupType::ARCHER,    { { 4, 3, 2, 4 }, { 5,  2,  4 },    "archer", BuildingType::TARGET       } },
    { UnitGroupType::DWARF,     { { 2, 4, 6, 3 }, { 3,  5,  6 },     "dwarf", BuildingType::BLACKSMITH   } },
    { UnitGroupType::HARPY,     { { 3, 3, 3, 5 }, { 6,  3,  5 },     "harpy", BuildingType::HARPY_TOWER  } },
    { UnitGroupType::DEMON,     { { 5, 4, 3, 6 }, { 8,  3,  7 },     "demon", BuildingType::DEMON_CASTLE } }
};

UnitStats UnitStats::operator+(int scalar)
{
    return {
        this->Attack  + scalar,
        this->Defense + scalar,
        this->Health  + scalar
    };
}

UnitStats UnitStats::operator+(UnitStats stats)
{
    return {
        this->Attack  + stats.Attack,
        this->Defense + stats.Defense,
        this->Health  + stats.Health
    };
}

bool UnitStats::operator>(UnitStats stats)
{
    return this->Attack + this->Defense + this->Health > stats.Attack + stats.Defense + stats.Health;
}

UnitGroup::UnitGroup(UnitGroupType type, std::optional<UnitStats*> stats, int movedOnIteration)
    : m_Type(type), m_IsSelected(false), m_MovedOnIteration(movedOnIteration)
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
