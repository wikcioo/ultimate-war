#include "player_manager.h"

#include "game/ai.h"
#include "game/game_layer.h"

PlayerManager::PlayerManager()
    : m_CurrentPlayerIndex(0), m_ActivePlayerCount(0)
{
}

std::shared_ptr<Player> PlayerManager::AddPlayer(PlayerDTO playerData)
{
    auto player = std::make_shared<Player>(playerData);
    m_Players.emplace_back(player);
    m_ActivePlayerCount++;

    return player;
}

void PlayerManager::NextTurn()
{
    auto prevPlayer = GetCurrentPlayer();
    GameLayer::Get().ResetArrow();

    m_CurrentPlayerIndex++;

    if (m_CurrentPlayerIndex % m_Players.size() == 0)
    {
        m_CurrentPlayerIndex = 0;
        GameLayer::Get().NextIteration();
    }

    auto currPlayer = GetCurrentPlayer();
    int currPlayerIndex = m_CurrentPlayerIndex;
    while (IsInactivePlayer(currPlayer))
    {
        m_CurrentPlayerIndex++;

        if (m_CurrentPlayerIndex == currPlayerIndex)
            break;

        if (m_CurrentPlayerIndex % m_Players.size() == 0)
        {
            m_CurrentPlayerIndex = 0;
            GameLayer::Get().NextIteration();
        }

        currPlayer = GetCurrentPlayer();
    }

    if (currPlayer == prevPlayer)
    {
        return;
    }

    if (GameLayer::Get().GetIteration() != 0)
        m_Players[m_CurrentPlayerIndex]->CollectResourcesFromOwnedTiles();

    if (currPlayer->IsAIPlayer())
    {
        AI::Get().MakeMove(currPlayer);
        NextTurn();
    }
}

bool PlayerManager::IsInactivePlayer(const std::shared_ptr<Player>& player)
{
    return player && player->GetOwnedTiles().size() == 0;
}

void PlayerManager::UpdatePlayerStatus(const std::shared_ptr<Player>& player)
{
    if (IsInactivePlayer(player))
    {
        m_ActivePlayerCount--;

        if (m_ActivePlayerCount == 1)
            GameLayer::Get().EndGame();
    }
}
