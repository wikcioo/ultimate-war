#pragma once

#include <memory>
#include <string>

#include "game/tile.h"

class Player : public std::enable_shared_from_this<Player>
{
public:
    Player(const std::string& name, const glm::vec3& color, Resources resources = {0});
    ~Player() = default;

    inline std::string GetName() { return m_Name; }
    inline glm::vec3& GetColor() { return m_Color; }
    inline Resources GetResources() { return m_Resources; }
    inline std::vector<std::shared_ptr<Tile>>& GetOwnedTiles() { return m_OwnedTiles; }

    void AddResources(Resources& resources);
    bool SubtractResources(Resources& resources);
    void AddOwnedTile(std::shared_ptr<Tile> tile);
    void RemoveOwnedTile(std::shared_ptr<Tile> tile);
    void CollectResourcesFromOwnedTiles();

private:
    std::string m_Name;
    glm::vec3 m_Color;
    Resources m_Resources;
    std::vector<std::shared_ptr<Tile>> m_OwnedTiles;
};
