#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "game/unit.h"

#define tileWidth   2.0f
#define tileHeight (glm::sqrt(3))
#define tileOffset  0.1f

class Player;

class Tile
{
public:
    Tile(int type, const glm::vec2& position);
    ~Tile();

    void AddUnit(UnitType type);
    void Draw(const glm::vec4& color);
    void DrawBase(const glm::vec3& color);
    void DrawBase(const glm::vec4& color);
    void SetOwnership(std::shared_ptr<Player> player);

    inline const int GetType() const { return m_Type; }
    inline const int GetValue() const { return m_Value; }
    inline const bool IsOwned() const { return m_OwnedBy.get() != nullptr; }
    inline const glm::vec2& GetPosition() const { return m_Position; }

    bool InRange(const glm::vec2& cursorPos);

public:
    static glm::vec2 CalculateTilePosition(int x, int y);

private:
    int m_Type;
    int m_Value;
    glm::vec2 m_Position;
    std::shared_ptr<Player> m_OwnedBy;
    std::vector<Unit*> m_Units;
};
