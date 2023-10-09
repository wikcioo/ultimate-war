#include "unit.h"

std::unordered_map<UnitType, UnitData> UnitDataMap = {
    {UnitType::SWORDSMAN, {10, {6,   3,  5}, "swordsman"}},
    {UnitType::ARCHER,    {8,  {5,   5,  3},    "archer"}},
    {UnitType::DWARF,     {7,  {3,   3,  3},     "dwarf"}},
    {UnitType::DEMON,     {11, {2,  10,  4},     "demon"}},
    {UnitType::HARPY,     {9,  {10,  1,  2},     "harpy"}}
};

Unit::Unit(UnitType type)
    : m_Type(type), m_Stats(UnitDataMap[type].Stats), m_IsSelected(false)
{
}
