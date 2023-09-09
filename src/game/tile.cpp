#include "tile.h"

#include <vector>

Tile::Tile(int type, const glm::vec2& position)
    : m_Type(type), m_Position(position)
{
}

bool Tile::InRange(const glm::vec2& cursorPos)
{
    float w = tileWidth;
    float h = tileHeight;

    std::vector<glm::vec2> vertices = {
        {m_Position.x - w/2, m_Position.y},
        {m_Position.x - w/4, m_Position.y + h/2},
        {m_Position.x + w/4, m_Position.y + h/2},
        {m_Position.x + w/2, m_Position.y},
        {m_Position.x + w/4, m_Position.y - h/2},
        {m_Position.x - w/4, m_Position.y - h/2}
    };

    std::vector<glm::vec3> equations;
    for (int i = 0; i < 6; i++)
    {
        glm::vec2 v1 = vertices[i];
        glm::vec2 v2 = vertices[(i + 1) % 6];
        float A = v1.y - v2.y;
        float B = v2.x - v1.x;
        float C = v1.x * v2.y - v2.x * v1.y;
        equations.push_back({A, B, C});
    }

    for (auto eq : equations)
    {
        if (eq.x * cursorPos.x + eq.y * cursorPos.y + eq.z > 0)
            return false;
    }

    return true;
}
