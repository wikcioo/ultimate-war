#pragma once

#include <memory>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "game/player.h"
#include "event/event.h"

class PlayerManager
{
public:
    PlayerManager();
    ~PlayerManager() = default;

    const std::shared_ptr<Player>& GetCurrentPlayer() { return m_Players[m_CurrentPlayerIndex]; }
    std::vector<std::shared_ptr<Player>>& GetAllPlayers() { return m_Players; }

    std::shared_ptr<Player> AddPlayer(PlayerDTO playerData);
    void UpdatePlayerStatus(const std::shared_ptr<Player>& player);
    void NextTurn();
    void SetCurrentPlayerIndex(int index) { m_CurrentPlayerIndex = index; }

private:
    bool IsInactivePlayer(const std::shared_ptr<Player>& player);

private:
    int m_CurrentPlayerIndex;
    int m_ActivePlayerCount;
    std::vector<std::shared_ptr<Player>> m_Players;
};
