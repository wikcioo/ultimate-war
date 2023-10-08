#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "game/unit.h"
#include "game/building.h"

#define TILE_WIDTH   2.0f
#define TILE_HEIGHT (glm::sqrt(3))
#define TILE_OFFSET  0.1f

struct DrawData
{
    glm::vec2 Size;
    glm::vec2 OffsetSize;
    glm::vec2 Position;
    glm::vec2 BackgroundPosition;
    glm::vec2 BackgroundSize;
};

class Player;

class Tile : public std::enable_shared_from_this<Tile>
{
public:
    Tile(int type, const glm::ivec2& coords);
    ~Tile();

    void MoveToTile(std::shared_ptr<Tile> destTile);
    void CreateUnit(UnitType type);
    void CreateBuilding(BuildingType type);
    void DeselectAllUnits();
    void Draw(const glm::vec4& color);
    void DrawBase(const glm::vec4& color);
    bool HasSelectedUnits();
    bool InRange(const glm::vec2& cursorPos);
    bool HandleUnitMouseClick(const glm::vec2& relMousePos);
    bool IsMouseClickedInsideUnitsBox(const glm::vec2& relMousePos);

    inline const int GetType() const { return m_Type; }
    inline const int GetValue() const { return m_Value; }
    inline const std::shared_ptr<Player> GetOwnedBy() const { return m_OwnedBy; }
    std::vector<Unit*>& GetUnits() { return m_Units; }
    inline const bool IsOwned() const { return m_OwnedBy.get() != nullptr; }
    inline const glm::vec2& GetPosition() const { return m_Position; }
    inline const glm::ivec2& GetCoords() const { return m_Coords; }

    void SetOwnership(std::shared_ptr<Player> player);
    void ChangeOwnership(std::shared_ptr<Player> player);

public:
    static bool IsAdjacent(const glm::ivec2& tile1, const glm::ivec2& tile2);
    static glm::vec2 CalculateTilePosition(int x, int y);

public:
    static float s_BackgroundHeightRatio;

    static int s_UnitRows;
    static int s_UnitsPerRow;
    static int s_UnitWidthToOffsetRatio;

    static int s_BuildingRows;
    static int s_BuildingsPerRow;
    static int s_BuildingWidthToOffsetRatio;

private:
    void DrawUnits();
    void DrawBuildings();
    void EraseSelectedUnits();
    void TransferUnitsToTile(std::shared_ptr<Tile> destTile);
    bool CalculateAttackOutcome(std::shared_ptr<Tile> destTile);
    DrawData GetUnitDrawData();
    DrawData GetBuildingDrawData();

private:
    int m_Type;
    int m_Value;
    glm::ivec2 m_Coords;
    glm::vec2 m_Position;
    std::shared_ptr<Player> m_OwnedBy;
    std::vector<Unit*> m_Units;
    std::vector<Building*> m_Buildings;
};
