#include "player.h"

#include <algorithm>

Player::Player(const std::string& name, const glm::vec3& color, int gold)
    :  m_Name(name), m_Color(color), m_Gold(gold)
{
}

void Player::AddGold(int amount)
{
    m_Gold += amount;
}

void Player::AddOwnedTile(std::shared_ptr<Tile> tile)
{
    m_OwnedTiles.emplace_back(tile);
    tile->SetOwnership(shared_from_this());
}

void Player::RemoveOwnedTile(std::shared_ptr<Tile> tile)
{
    auto it = std::find(m_OwnedTiles.begin(), m_OwnedTiles.end(), tile);
    if (it != m_OwnedTiles.end())
        m_OwnedTiles.erase(it);
}

void Player::CollectGoldFromOwnedTiles()
{
    for (auto tile : m_OwnedTiles)
        m_Gold += tile->GetValue();
}
