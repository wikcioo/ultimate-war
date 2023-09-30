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

    std::shared_ptr<Player> GetCurrentPlayer() { return m_Players[m_CurrentPlayerIndex]; }
    std::vector<std::shared_ptr<Player>>& GetAllPlayers() { return m_Players; }

    void AddPlayer(const std::string& name = "", const glm::vec3& color = glm::vec3(1.0f, 0.0f, 1.0f));
    void NextTurn();

private:
    int m_CurrentPlayerIndex;
    std::vector<std::shared_ptr<Player>> m_Players;
};
