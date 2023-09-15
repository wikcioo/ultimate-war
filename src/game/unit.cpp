#include "unit.h"

std::unordered_map<UnitType, std::string> UnitTextureMap = {
    {UnitType::NONE, "none"},
    {UnitType::SWORDSMAN, "swordsman"},
    {UnitType::ARCHER, "archer"},
    {UnitType::DWARF, "dwarf"},
    {UnitType::DEMON, "demon"},
    {UnitType::HARPY, "harpy"}
};

std::unordered_map<UnitType, UnitStats> UnitStatMap = {
    {UnitType::NONE, {0, 0, 0}},
    {UnitType::SWORDSMAN, {6, 3, 5}},
    {UnitType::ARCHER, {5, 5, 3}},
    {UnitType::DWARF, {3, 3, 3}},
    {UnitType::DEMON, {2, 10, 4}},
    {UnitType::HARPY, {10, 1, 2}}
};

Unit::Unit(UnitType type)
    : m_Type(type), m_Stats(UnitStatMap[type])
{
}
