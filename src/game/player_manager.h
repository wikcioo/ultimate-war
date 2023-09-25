#pragma once

#include <vector>
#include <memory>

#include "game/player.h"
#include "game/map_manager.h"

class PlayerManager
{
public:
    PlayerManager(const std::shared_ptr<GameMapManager>& gameMapManager);
    ~PlayerManager();

    std::vector<Player*>& GetPlayers() { return m_Players; }
    void AddPlayer();
    void Update();
    void NextTurn();
private:
    const std::shared_ptr<GameMapManager> m_GameMapManager;
    std::vector<Player*> m_Players;
    int m_CurrentPlayer;
    int m_TurnNumber;
};
