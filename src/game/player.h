#pragma once

#include <memory>
#include <string>

#include "game/tile.h"

class Player : public std::enable_shared_from_this<Player>
{
public:
    Player(const std::string& name, const glm::vec3& color, int gold = 0);
    ~Player() = default;

    inline std::string GetName() { return m_Name; }
    inline glm::vec3& GetColor() { return m_Color; }
    inline int GetGold() { return m_Gold; }
    inline std::vector<std::shared_ptr<Tile>>& GetOwnedTiles() { return m_OwnedTiles; }

    void AddGold(int amount);
    void AddOwnedTile(std::shared_ptr<Tile> tile);
    void RemoveOwnedTile(std::shared_ptr<Tile> tile);
    void CollectGoldFromOwnedTiles();

private:
    std::string m_Name;
    glm::vec3 m_Color;
    int m_Gold;
    std::vector<std::shared_ptr<Tile>> m_OwnedTiles;
};
