#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "game/unit.h"
#include "game/building.h"

#define TILE_WIDTH   2.0f
#define TILE_HEIGHT (glm::sqrt(3))
#define TILE_OFFSET  0.1f

class Player;

struct DrawData
{
    glm::vec2 Size;
    glm::vec2 OffsetSize;
    glm::vec2 Position;
    glm::vec2 BackgroundPosition;
    glm::vec2 BackgroundSize;
};

enum class TileEnvironment
{
    NONE,
    OCEAN,
    FOREST,
    DESERT,
    MOUNTAINS
};

class Tile : public std::enable_shared_from_this<Tile>
{
public:
    Tile(TileEnvironment environment, const glm::ivec2& coords);
    ~Tile();

    void MoveToTile(std::shared_ptr<Tile> destTile);
    void CreateUnitGroup(UnitGroupType type);
    bool CanRecruitUnitGroup(UnitGroupType type);
    bool HasSpaceForUnitGroups(int num);
    bool HasSpaceForBuildings(int num);
    void CreateBuilding(BuildingType type);
    void DeselectAllUnitGroups();
    void Draw();
    bool HasSelectedUnitGroups();
    bool InRange(const glm::vec2& cursorPos);
    bool HandleUnitGroupMouseClick(const glm::vec2& relMousePos);
    bool IsMouseClickedInsideUnitGroupsBox(const glm::vec2& relMousePos);
    bool AssetsCanExist() { return m_Environment != TileEnvironment::NONE && m_Environment != TileEnvironment::OCEAN; }
    void CheckUnitGroupHover(const glm::vec2& relMousePos);

    inline const TileEnvironment GetEnvironment() const { return m_Environment; }
    inline const Resources GetResources() const { return m_Resources; }
    inline const std::shared_ptr<Player> GetOwnedBy() const { return m_OwnedBy; }
    std::vector<UnitGroup*>& GetUnitGroups() { return m_UnitGroups; }
    inline const bool IsOwned() const { return m_OwnedBy.get() != nullptr; }
    inline const glm::vec2& GetPosition() const { return m_Position; }
    inline const glm::ivec2& GetCoords() const { return m_Coords; }
    int GetNumSelectedUnitGroups();

    void SetOwnership(std::shared_ptr<Player> player);
    void ChangeOwnership(std::shared_ptr<Player> player);

public:
    static bool IsAdjacent(const glm::ivec2& tile1, const glm::ivec2& tile2);
    static glm::vec2 CalculateTilePosition(int x, int y);

public:
    static float s_BackgroundHeightRatio;

    static int s_UnitGroupRows;
    static int s_UnitGroupsPerRow;
    static int s_UnitGroupWidthToOffsetRatio;

    static int s_BuildingRows;
    static int s_BuildingsPerRow;
    static int s_BuildingWidthToOffsetRatio;

private:
    void DrawUnitGroupStats(DrawData& unitData, UnitGroup* unitGroup);
    void DrawEnvironment();
    void DrawUnitGroups();
    void DrawBuildings();
    void EraseSelectedUnitGroups();
    void TransferUnitGroupsToTile(std::shared_ptr<Tile> destTile);
    DrawData GetUnitGroupDrawData();
    DrawData GetBuildingDrawData();

private:
    TileEnvironment m_Environment;
    Resources m_Resources;
    glm::ivec2 m_Coords;
    glm::vec2 m_Position;
    std::shared_ptr<Player> m_OwnedBy;
    std::vector<UnitGroup*> m_UnitGroups;
    std::vector<Building*> m_Buildings;
};
