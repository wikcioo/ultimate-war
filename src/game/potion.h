#pragma once

#include <string>
#include <unordered_map>

#include "game/resource.h"

enum class PotionType
{
    HEALING,
    IMMUNITY,
    REDUCE_DAMAGE,
    INCREASE_YIELD,
    DEAL_DAMAGE,
    COUNT,
    NONE
};

struct PotionData
{
    Resources Cost;
    int CooldownInIterations;
    int DurationInIterations;
    std::string TextureName;
};

extern std::unordered_map<PotionType, PotionData> PotionDataMap;

class Potion
{
public:
    Potion();
    ~Potion() = default;

    const PotionType GetType() const { return m_Type; }

    bool Apply(PotionType type);
    void Tick();
    inline int GetCooldown(PotionType type) { return m_Cooldowns[type]; }
    inline bool IsApplied() const { return m_IsApplied; }
    inline bool CanApply(PotionType type) { return m_Cooldowns[type] <= 0; }

private:
    PotionType m_Type;
    bool m_IsApplied;
    int m_IterationsLeft;
    std::unordered_map<PotionType, int> m_Cooldowns;
};
