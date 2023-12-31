#pragma once

#include <memory>
#include <string>

#include "game/tile.h"

struct PlayerDTO
{
    std::string Name;
    glm::vec3 Color;
    std::vector<glm::vec2> TileCoords;
    Resources ResourceData;
    bool IsAI;

    PlayerDTO() {}

    PlayerDTO(const std::string& name, const glm::vec3& color, const std::vector<glm::vec2>& tileCoords,
              Resources resources = {0}, bool isAI = false)
    {
        Name = name;
        Color = color;
        TileCoords = tileCoords;
        ResourceData = resources;
        IsAI = isAI;
    }
};

class Player : public std::enable_shared_from_this<Player>
{
public:
    Player(PlayerDTO playerData);
    ~Player() = default;

    inline std::string GetName() { return m_Name; }
    inline glm::vec3& GetColor() { return m_Color; }
    inline Resources GetResources() { return m_Resources; }
    inline std::vector<std::shared_ptr<Tile>>& GetOwnedTiles() { return m_OwnedTiles; }
    inline bool IsAIPlayer() { return m_IsAI; }

    void AddResources(Resources& resources);
    bool SubtractResources(Resources& resources);
    void AddOwnedTile(const std::shared_ptr<Tile>& tile);
    void RemoveOwnedTile(const std::shared_ptr<Tile>& tile);
    void CollectResourcesFromOwnedTiles();

private:
    std::string m_Name;
    bool m_IsAI;
    glm::vec3 m_Color;
    Resources m_Resources;
    std::vector<std::shared_ptr<Tile>> m_OwnedTiles;
};
