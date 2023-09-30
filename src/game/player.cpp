#include "player.h"

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

void Player::CollectGoldFromOwnedTiles()
{
    for (auto tile : m_OwnedTiles)
        m_Gold += tile->GetValue();
}
