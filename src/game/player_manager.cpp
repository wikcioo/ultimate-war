#include "player_manager.h"


PlayerManager::PlayerManager(const std::shared_ptr<GameMapManager>& gameMapManager)
    : m_GameMapManager(gameMapManager), m_CurrentPlayer(-1), m_TurnNumber(0)
{
    AddPlayer();
    AddPlayer();
}

PlayerManager::~PlayerManager()
{
    for (auto player : m_Players)
        delete player;
}

void PlayerManager::AddPlayer()
{
    m_Players.push_back(new Player(m_Players.size()));
}

void PlayerManager::Update()
{

}

void PlayerManager::NextTurn()
{
    m_CurrentPlayer++;
    if (m_CurrentPlayer % m_Players.size() == 0)
    {
        m_CurrentPlayer = 0;
        m_TurnNumber++;
    }

    m_Players[m_CurrentPlayer]->AddGold(m_GameMapManager->GetGameMap()->CalculatePlayerIncome(m_CurrentPlayer));
}
