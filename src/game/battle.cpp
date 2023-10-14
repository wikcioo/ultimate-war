#include "battle.h"

#include <vector>
#include <numeric>

#include <glm/glm.hpp>

#include "util/util.h"
#include "core/logger.h"

BattleOutcome Battle::CalculateBattleOutcome(std::shared_ptr<Tile> attacker, std::shared_ptr<Tile> defender)
{
    while (GetTotalUnitGroupHealth(attacker, true) > 0 && GetTotalUnitGroupHealth(defender, false) > 0)
    {
        SimulateBattleTick(attacker, defender);
        CleanupBattleField(attacker, defender);
    }

    return GetTotalUnitGroupHealth(attacker, true) > 0 ? BattleOutcome::ATTACKER_WON : BattleOutcome::DEFENDER_WON;
}

void Battle::SimulateBattleTick(std::shared_ptr<Tile> attacker, std::shared_ptr<Tile> defender)
{
    std::vector<UnitStats*> attackerUnitStats = GatherUnits(attacker, true);
    std::vector<UnitStats*> defenderUnitStats = GatherUnits(defender, false);

    BattleUnits(attackerUnitStats, defenderUnitStats);
}

void Battle::BattleUnits(const std::vector<UnitStats*>& attacker, const std::vector<UnitStats*>& defender)
{
    size_t minUnitStatsCount = glm::min(attacker.size(), defender.size());

    for (size_t i = 0; i < minUnitStatsCount; i++)
    {
        OneVOne(attacker[i], defender[i]);
    }

    if (attacker.size() > defender.size())
    {
        for (size_t i = minUnitStatsCount; i < attacker.size(); i++)
        {
            OneVOne(attacker[i], defender[i % minUnitStatsCount]);
        }
    }
    else
    {
        for (size_t i = minUnitStatsCount; i < defender.size(); i++)
        {
            OneVOne(defender[i], attacker[i % minUnitStatsCount]);
        }
    }
}

std::vector<UnitStats*> Battle::GatherUnits(std::shared_ptr<Tile> tile, bool checkSelectedOnly)
{
    std::vector<UnitStats*> unitStats;

    for (UnitGroup* unitGroup : tile->GetUnitGroups())
    {
        if (checkSelectedOnly && !unitGroup->IsSelected())
            continue;

        unitStats.insert(unitStats.end(), unitGroup->GetUnitStats().begin(), unitGroup->GetUnitStats().end());
    }

    return unitStats;
}

void Battle::OneVOne(UnitStats* us1, UnitStats* us2)
{
    if (us1->Health <= 0 || us2->Health <= 0) return;

    us2->Health -= glm::max(us1->Attack - us2->Defense, 1);
    us1->Health -= glm::max(us2->Attack - us1->Defense, 1);

    us2->Health = glm::max(us2->Health, 0);
    us1->Health = glm::max(us1->Health, 0);
}

void Battle::CleanupBattleField(std::shared_ptr<Tile> attacker, std::shared_ptr<Tile> defender)
{
    CleanupUnits(attacker, true);
    CleanupUnits(defender, false);
}

void Battle::CleanupUnits(std::shared_ptr<Tile> tile, bool checkSelectedOnly)
{
    for (UnitGroup* unitGroup : tile->GetUnitGroups())
    {
        if (checkSelectedOnly && !unitGroup->IsSelected())
            continue;

        auto& stats = unitGroup->GetUnitStats();
        Util::RemoveElementsFromContainerWithCondition<std::vector<UnitStats*>, UnitStats*>(
            stats,
            [](UnitStats* us) {
                return us->Health <= 0;
            }
        );
    }

    auto& unitGroups = tile->GetUnitGroups();
    Util::RemoveElementsFromContainerWithCondition<std::vector<UnitGroup*>, UnitGroup*>(
        unitGroups,
        [](UnitGroup* ug) {
            return ug->GetUnitStats().empty();
        }
    );
}

int Battle::GetTotalUnitGroupHealth(std::shared_ptr<Tile> tile, bool checkSelectedOnly)
{
    int totalHealth = 0;

    for (UnitGroup* unitGroup : tile->GetUnitGroups())
    {
        if (checkSelectedOnly && !unitGroup->IsSelected())
            continue;

        totalHealth = std::accumulate(
            unitGroup->GetUnitStats().begin(),
            unitGroup->GetUnitStats().end(),
            totalHealth,
            [](int acc, const UnitStats* const stats) {
                return acc + stats->Health;
            }
        );
    }

    return totalHealth;
}
