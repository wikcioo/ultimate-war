#include "player.h"

#include <algorithm>

Player::Player(PlayerDTO playerData)
    :  m_Name(playerData.Name), m_Color(playerData.Color), m_Resources(playerData.ResourceData)
{
}

void Player::AddResources(Resources& resources)
{
    m_Resources += resources;
}

bool Player::SubtractResources(Resources& resources)
{
    if (m_Resources >= resources)
    {
        m_Resources -= resources;
        return true;
    }

    return false;
}

void Player::AddOwnedTile(const std::shared_ptr<Tile>& tile)
{
    m_OwnedTiles.emplace_back(tile);
    tile->SetOwnership(shared_from_this());
}

void Player::RemoveOwnedTile(const std::shared_ptr<Tile>& tile)
{
    auto it = std::find(m_OwnedTiles.begin(), m_OwnedTiles.end(), tile);
    if (it != m_OwnedTiles.end())
        m_OwnedTiles.erase(it);
}

void Player::CollectResourcesFromOwnedTiles()
{
    for (const auto& tile : m_OwnedTiles)
        m_Resources += tile->GetResources();
}
