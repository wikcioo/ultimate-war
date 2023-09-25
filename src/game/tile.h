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
    void DrawBase(const glm::vec4& color);

    inline const int GetType() const { return m_Type; }
    inline const int GetPlayerID() const { return m_PlayerID; }
    inline const int GetIncomeValue() const { return m_IncomeValue; }
    inline const glm::vec2& GetPosition() const { return m_Position; }

    inline void SetPlayerID(int playerID) { m_PlayerID = playerID; }

    bool InRange(const glm::vec2& cursorPos);
public:
    static glm::vec2 CalculateTilePosition(int x, int y);
private:
    int m_Type;
    int m_PlayerID;
    int m_IncomeValue;
    glm::vec2 m_Position;
    std::vector<Unit*> m_Units;
};
