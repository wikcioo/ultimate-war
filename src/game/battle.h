#pragma once

#include <memory>

#include "game/tile.h"

enum class BattleOutcome
{
    ATTACKER_WON,
    DEFENDER_WON
};

class Battle
{
public:
    static BattleOutcome CalculateBattleOutcome(const std::shared_ptr<Tile>& attacker, const std::shared_ptr<Tile>& defender);

private:
    static void SimulateBattleTick(const std::shared_ptr<Tile>& attacker, const std::shared_ptr<Tile>& defender);
    static void BattleUnits(const std::vector<UnitStats*>& attacker, const std::vector<UnitStats*>& defender, bool defenderTakesReducedDamage = false);
    static std::vector<UnitStats*> GatherUnits(const std::shared_ptr<Tile>& tile, bool checkSelectedOnly);
    static void OneVOne(UnitStats* us1, UnitStats* us2, int reducedDefenderDamageValue);
    static void CleanupBattleField(const std::shared_ptr<Tile>& attacker, const std::shared_ptr<Tile>& defender);
    static void CleanupUnits(const std::shared_ptr<Tile>& tile, bool checkSelectedOnly);
    static int GetTotalUnitGroupHealth(const std::shared_ptr<Tile>& tile, bool checkSelectedOnly);
};
