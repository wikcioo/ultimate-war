#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "game/unit.h"

#define TILE_WIDTH   2.0f
#define TILE_HEIGHT (glm::sqrt(3))
#define TILE_OFFSET  0.1f

struct UnitDrawData
{
    glm::vec2 Size;
    glm::vec2 OffsetSize;
    glm::vec2 Position;
    glm::vec2 UnitsBackgroundPosition;
    glm::vec2 UnitsBackgroundSize;
};

class Player;

class Tile
{
public:
    Tile(int type, const glm::ivec2& coords);
    ~Tile();

    void CreateUnit(UnitType type);
    void DeselectAllUnits();
    void Draw(const glm::vec4& color);
    void DrawBase(const glm::vec4& color);
    void TransferUnitsToTile(std::shared_ptr<Tile> destTile);
    bool HasSelectedUnits();
    bool InRange(const glm::vec2& cursorPos);
    bool HandleUnitMouseClick(const glm::vec2& relMousePos);
    bool IsMouseClickedInsideUnitsBox(const glm::vec2& relMousePos);

    inline const int GetType() const { return m_Type; }
    inline const int GetValue() const { return m_Value; }
    std::vector<Unit*>& GetUnits() { return m_Units; }
    inline const bool IsOwned() const { return m_OwnedBy.get() != nullptr; }
    inline const glm::vec2& GetPosition() const { return m_Position; }
    inline const glm::ivec2& GetCoords() const { return m_Coords; }

    void SetOwnership(std::shared_ptr<Player> player);

public:
    static bool IsAdjacent(const glm::ivec2& tile1, const glm::ivec2& tile2);
    static glm::vec2 CalculateTilePosition(int x, int y);

public:
    static float s_BackgroundHeightRatio;
    static int s_UnitRows;
    static int s_UnitsPerRow;
    static int s_UnitWidthToOffsetRatio;

private:
    void DrawUnits();
    UnitDrawData GetUnitDrawData();

private:
    int m_Type;
    int m_Value;
    glm::ivec2 m_Coords;
    glm::vec2 m_Position;
    std::shared_ptr<Player> m_OwnedBy;
    std::vector<Unit*> m_Units;
};
