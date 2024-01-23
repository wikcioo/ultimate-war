#include "potion.h"

std::unordered_map<PotionType, PotionData> PotionDataMap = {
    { PotionType::HEALING,        { { 20, 20, 20, 20 }, 3, 2, "healing"        } },
    { PotionType::IMMUNITY,       { { 20, 20, 20, 20 }, 5, 1, "immunity"       } },
    { PotionType::REDUCE_DAMAGE,  { { 20, 20, 20, 20 }, 3, 2, "reduce_damage"  } },
    { PotionType::INCREASE_YIELD, { { 20, 20, 20, 20 }, 5, 5, "increase_yield" } },
    { PotionType::DEAL_DAMAGE,    { { 20, 20, 20, 20 }, 5, 1, "deal_damage"    } }
};

Potion::Potion()
    : m_Type(PotionType::NONE), m_IsApplied(false), m_IterationsLeft(0)
{
    m_Cooldowns[PotionType::HEALING] = 0;
    m_Cooldowns[PotionType::IMMUNITY] = 0;
    m_Cooldowns[PotionType::REDUCE_DAMAGE] = 0;
    m_Cooldowns[PotionType::INCREASE_YIELD] = 0;
    m_Cooldowns[PotionType::DEAL_DAMAGE] = 0;
}

bool Potion::Apply(PotionType type)
{
    if (!CanApply(type))
        return false;

    m_Type = type;
    m_IsApplied = true;
    m_IterationsLeft = PotionDataMap[type].DurationInIterations;
    m_Cooldowns[type] = PotionDataMap[type].CooldownInIterations;
    return true;
}

void Potion::Tick()
{
    m_IterationsLeft--;
    m_Cooldowns[m_Type]--;
    if (m_IterationsLeft <= 0)
        m_IsApplied = false;
}
