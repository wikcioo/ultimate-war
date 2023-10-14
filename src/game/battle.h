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
    static BattleOutcome CalculateBattleOutcome(std::shared_ptr<Tile> attacker, std::shared_ptr<Tile> defender);

private:
    static void SimulateBattleTick(std::shared_ptr<Tile> attacker, std::shared_ptr<Tile> defender);
    static void BattleUnits(const std::vector<UnitStats*>& attacker, const std::vector<UnitStats*>& defender);
    static std::vector<UnitStats*> GatherUnits(std::shared_ptr<Tile> tile, bool checkSelectedOnly);
    static void OneVOne(UnitStats* us1, UnitStats* us2);
    static void CleanupBattleField(std::shared_ptr<Tile> attacker, std::shared_ptr<Tile> defender);
    static void CleanupUnits(std::shared_ptr<Tile> tile, bool checkSelectedOnly);
    static int GetTotalUnitGroupHealth(std::shared_ptr<Tile> tile, bool checkSelectedOnly);
};
