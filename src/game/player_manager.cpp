#include "player_manager.h"

#include "util/util.h"
#include "game/game_layer.h"

PlayerManager::PlayerManager()
    : m_CurrentPlayerIndex(0)
{
}

void PlayerManager::AddPlayer(const std::string& name, const glm::vec3& color)
{
    m_Players.emplace_back(std::make_shared<Player>(name.empty() ? Util::GenerateAnonymousName() : name, color));
}

void PlayerManager::NextTurn()
{
    GameLayer::Get().ResetArrow();

    m_CurrentPlayerIndex++;

    if (m_CurrentPlayerIndex % m_Players.size() == 0)
    {
        m_CurrentPlayerIndex = 0;
        GameLayer::Get().NextIteration();
    }

    m_Players[m_CurrentPlayerIndex]->CollectGoldFromOwnedTiles();
}
