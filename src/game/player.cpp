#include "player.h"

#include <algorithm>

Player::Player(const std::string& name, const glm::vec3& color, Resources resources)
    :  m_Name(name), m_Color(color), m_Resources(resources)
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

void Player::CollectResourcesFromOwnedTiles()
{
    for (auto tile : m_OwnedTiles)
        m_Resources += tile->GetResources();
}
