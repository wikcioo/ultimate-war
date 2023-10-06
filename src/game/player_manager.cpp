#include "player_manager.h"

#include "util/util.h"
#include "game/game_layer.h"

PlayerManager::PlayerManager()
    : m_CurrentPlayerIndex(0), m_ActivePlayerCount(0)
{
}

void PlayerManager::AddPlayer(const std::string& name, const glm::vec3& color)
{
    m_Players.emplace_back(std::make_shared<Player>(name.empty() ? Util::GenerateAnonymousName() : name, color));
    m_ActivePlayerCount++;
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

    if(IsInactivePlayer(GetCurrentPlayer()))
        NextTurn();

    m_Players[m_CurrentPlayerIndex]->CollectGoldFromOwnedTiles();
}

bool PlayerManager::IsInactivePlayer(const std::shared_ptr<Player> player)
{
    return player && player->GetOwnedTiles().size() == 0;
}

void PlayerManager::UpdatePlayerStatus(const std::shared_ptr<Player> player)
{
    if(IsInactivePlayer(player))
    {
        m_ActivePlayerCount--;

        if (m_ActivePlayerCount == 1)
            GameLayer::Get().EndGame();
    }
}

