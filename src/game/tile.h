#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "game/unit.h"

#define tileWidth   2.0f
#define tileHeight (glm::sqrt(3))
#define tileOffset  0.1f

class Tile
{
public:
    Tile(int type, const glm::vec2& position);
    ~Tile();

    void AddUnit(UnitType type);
    void Draw(const glm::vec4& color);

    inline const int GetType() const { return m_Type; }
    inline const glm::vec2& GetPosition() const { return m_Position; }

    bool InRange(const glm::vec2& cursorPos);

private:
    int m_Type;
    glm::vec2 m_Position;
    std::vector<Unit*> m_Units;
};
