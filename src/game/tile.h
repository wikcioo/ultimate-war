#pragma once

#include <glm/glm.hpp>

#define tileWidth   1.0f
#define tileHeight (0.5 * glm::sqrt(3))
#define tileOffset  0.1f

class Tile
{
public:
    Tile(int type, const glm::vec2& position);
    ~Tile() = default;

    inline const int GetType() const { return m_Type; }
    inline const glm::vec2& GetPosition() const { return m_Position; }

    bool InRange(const glm::vec2& cursorPos);

private:
    int m_Type;
    glm::vec2 m_Position;
};
