#include "tile.h"

#include <vector>

#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "debug/debug_data.h"
#include "game/player.h"
#include "util/util.h"
#include "game/game_layer.h"

float Tile::s_BackgroundHeightRatio = 0.8f;

int Tile::s_UnitRows = 2;
int Tile::s_UnitsPerRow = 5;
int Tile::s_UnitWidthToOffsetRatio = 10;

int Tile::s_BuildingRows = 1;
int Tile::s_BuildingsPerRow = 5;
int Tile::s_BuildingWidthToOffsetRatio = 10;

Tile::Tile(int type, const glm::ivec2& coords)
    : m_Type(type), m_Coords(coords), m_Position(Tile::CalculateTilePosition(coords.x, coords.y))
{
    // TODO(Viktor): Refactor the value to be based on buildings, once implemented
    switch (type)
    {
        case 0: m_Value = 0; break;
        case 1: m_Value = 10; break;
        case 2: m_Value = 20; break;
        default:
            m_Value = 0;
    }
}

Tile::~Tile()
{
    for (auto unit : m_Units)
        delete unit;
}

void Tile::CreateUnit(UnitType type)
{
    m_Units.emplace_back(new Unit(type));
}

void Tile::CreateBuilding(BuildingType type)
{
    m_Buildings.emplace_back(new Building(type));
}

void Tile::Draw(const glm::vec4& color)
{
    if (!m_OwnedBy)
        DrawBase(color);
    else
    {
        auto c = m_OwnedBy->GetColor();
        DrawBase({c.r, c.g, c.b, 1.0f});
    }


    DrawUnits();
    DrawBuildings();
}

DrawData Tile::GetUnitDrawData()
{
    float L = TILE_WIDTH / 2 * s_BackgroundHeightRatio;
    float dx = glm::cos(glm::radians(60.0f)) * L;
    float dy = glm::sin(glm::radians(60.0f)) * L;

    glm::vec2 bgPos = {m_Position.x, m_Position.y + dy / 2};
    glm::vec2 bgSize = {TILE_WIDTH - 2 * dx, dy};

    float unitOffsetWidth  = bgSize.x / ((s_UnitsPerRow * s_UnitWidthToOffsetRatio) + s_UnitsPerRow + 1);
    float unitWidth        = s_UnitWidthToOffsetRatio * unitOffsetWidth;
    float unitHeight       = glm::min(bgSize.y / 2, unitWidth);
    float unitOffsetHeight = glm::max(0.0f, (bgSize.y - (unitHeight * s_UnitRows)) / (s_UnitRows + 1));

    float currentX = bgPos.x - (bgSize.x - unitWidth) / 2 + unitOffsetWidth;
    float currentY = bgPos.y + (bgSize.y - unitHeight) / 2 - unitOffsetHeight;

    return
    {
        {unitWidth, unitHeight},
        {unitOffsetWidth, unitOffsetHeight},
        {currentX, currentY},
        bgPos,
        bgSize
    };
}

DrawData Tile::GetBuildingDrawData()
{
    float L = TILE_WIDTH / 2 * 0.4;
    float dx = glm::cos(glm::radians(60.0f)) * L;
    float dy = glm::sin(glm::radians(60.0f)) * L;

    glm::vec2 bgPos = {m_Position.x, m_Position.y - dy / 2};
    glm::vec2 bgSize = {TILE_WIDTH - 2 * dx, dy};

    float buildingOffsetWidth  = bgSize.x / ((s_BuildingsPerRow * s_BuildingWidthToOffsetRatio) + s_BuildingsPerRow + 1);
    float buildingWidth        = s_BuildingWidthToOffsetRatio * buildingOffsetWidth;
    float buildingHeight       = glm::min(bgSize.y / 2, buildingWidth);
    float buildingOffsetHeight = glm::max(0.0f, (bgSize.y - (buildingHeight * s_BuildingRows)) / (s_BuildingRows + 1));

    float currentX = bgPos.x - (bgSize.x - buildingWidth) / 2 + buildingOffsetWidth;
    float currentY = bgPos.y + (bgSize.y - buildingHeight) / 2 - buildingOffsetHeight;

    return
    {
        {buildingWidth, buildingHeight},
        {buildingOffsetWidth, buildingOffsetHeight},
        {currentX, currentY},
        bgPos,
        bgSize
    };
}

void Tile::DrawUnits()
{
    if (m_Units.empty()) return;

    auto unitData = GetUnitDrawData();
    float initialX = unitData.Position.x;

    Renderer2D::DrawQuad(unitData.BackgroundPosition, unitData.BackgroundSize, {0.0f, 0.4f, 0.0f, 0.4f});

    for (int i = 0; i < m_Units.size(); i++)
    {
        if (m_Units[i]->IsSelected())
        {
            Renderer2D::DrawQuad(
                unitData.Position,
                unitData.Size,
                {0.8f, 0.1f, 0.1f, 0.6f}
            );
        }

        Renderer2D::DrawQuad(
            unitData.Position,
            unitData.Size,
            ResourceManager::GetTexture(UnitTextureMap[m_Units[i]->GetType()])
        );

        if ((i + 1) % s_UnitsPerRow == 0)
        {
            unitData.Position.x = initialX;
            unitData.Position.y -= (unitData.OffsetSize.y + unitData.Size.y);
        }
        else
        {
            unitData.Position.x += unitData.Size.x + unitData.OffsetSize.x;
        }
    }
}

void Tile::DrawBuildings()
{
    if (m_Buildings.empty()) return;

    auto buildingData = GetBuildingDrawData();
    float initialX = buildingData.Position.x;

    Renderer2D::DrawQuad(buildingData.BackgroundPosition, buildingData.BackgroundSize, {0.4f, 0.4f, 0.4f, 1.0f});

    for (int i = 0; i < m_Buildings.size(); i++)
    {
        Renderer2D::DrawQuad(
            buildingData.Position,
            buildingData.Size,
            ResourceManager::GetTexture(BuildingTextureMap[m_Buildings[i]->GetType()])
        );

        if ((i + 1) % s_BuildingsPerRow == 0)
        {
            buildingData.Position.x = initialX;
            buildingData.Position.y -= (buildingData.OffsetSize.y + buildingData.Size.y);
        }
        else
        {
            buildingData.Position.x += buildingData.Size.x + buildingData.OffsetSize.x;
        }
    }
}

void Tile::DrawBase(const glm::vec4& color)
{
    if (m_Type != 0)
        Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), color);
    else
        Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), color, 10.0f);
}

void Tile::SetOwnership(std::shared_ptr<Player> player)
{
    m_OwnedBy = player;
}

void Tile::ChangeOwnership(std::shared_ptr<Player> player)
{
    if(m_OwnedBy != nullptr)
        m_OwnedBy->RemoveOwnedTile(shared_from_this());
    player->AddOwnedTile(shared_from_this());
}

void Tile::MoveToTile(std::shared_ptr<Tile> destTile)
{
    if(destTile->m_OwnedBy == m_OwnedBy)
    {
        TransferUnitsToTile(destTile);
        return;
    }

    if(CalculateAttackOutcome(destTile))
    {
        destTile->GetUnits().clear();
        auto defender = destTile->GetOwnedBy();
        destTile->ChangeOwnership(this->m_OwnedBy);
        TransferUnitsToTile(destTile);

        GameLayer::Get().GetPlayerManager()->UpdatePlayerStatus(defender);
    }
    else
    {
        EraseSelectedUnits();
    }
}

bool Tile::CalculateAttackOutcome(std::shared_ptr<Tile> destTile)
{
    int destUnitCount = destTile->GetUnits().size();
    int attackerUnitCount = 0;
    for (auto unit : m_Units)
    {
        if (unit->IsSelected())
            attackerUnitCount++;
    }

    return attackerUnitCount > destUnitCount;
}

void Tile::TransferUnitsToTile(std::shared_ptr<Tile> destTile)
{
    if (destTile.get() == this) return;

    for (auto unit : m_Units)
    {
        if (!unit->IsSelected()) continue;

        destTile->GetUnits().push_back(unit);
    }

    EraseSelectedUnits();

    destTile->DeselectAllUnits();
}

void Tile::EraseSelectedUnits()
{
    m_Units.erase(std::remove_if(m_Units.begin(), m_Units.end(), [](Unit* unit) {
        return unit->IsSelected();
    }), m_Units.end());
}
bool Tile::HandleUnitMouseClick(const glm::vec2& relMousePos)
{
    auto unitData = GetUnitDrawData();
    float initialX = unitData.Position.x;

    for (int i = 0; i < m_Units.size(); i++)
    {
        if (Util::IsPointInRectangle(
            unitData.Position,
            unitData.Size,
             relMousePos))
        {
            m_Units[i]->ToggleSelected();
            return true;
        }

        if ((i + 1) % s_UnitsPerRow == 0)
        {
            unitData.Position.x = initialX;
            unitData.Position.y -= (unitData.OffsetSize.y + unitData.Size.y);
        }
        else
        {
            unitData.Position.x += unitData.Size.x + unitData.OffsetSize.x;
        }
    }

    return false;
}

bool Tile::IsMouseClickedInsideUnitsBox(const glm::vec2& relMousePos)
{
    auto unitData = GetUnitDrawData();
    return Util::IsPointInRectangle(unitData.BackgroundPosition, unitData.BackgroundSize, relMousePos);
}

void Tile::DeselectAllUnits()
{
    for (auto unit : m_Units)
        unit->SetSelected(false);
}

bool Tile::InRange(const glm::vec2& cursorPos)
{
    static float w = TILE_WIDTH;
    static float h = TILE_HEIGHT;

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

bool Tile::HasSelectedUnits()
{
    for (auto unit : m_Units)
    {
        if (unit->IsSelected())
            return true;
    }

    return false;
}

bool Tile::IsAdjacent(const glm::ivec2& tile1, const glm::ivec2& tile2)
{
    static std::array<glm::ivec2, 6> adjacentTileOffsets = {
        glm::ivec2(-1, 1), {0,  1}, {1, 1},
                  {-1, 0}, {0, -1}, {1, 0}
    };

    int offset = 0;
    if (tile1.x % 2 == 0)
        offset = -1;

    for (auto tileOffset : adjacentTileOffsets)
    {
        if (tileOffset.x != 0)
        {
            if (tile1.y + offset + tileOffset.y == tile2.y && tile1.x + tileOffset.x == tile2.x) return true;
        }
        else
        {
            if (tile1.y + tileOffset.y == tile2.y && tile1.x + tileOffset.x == tile2.x) return true;
        }
    }

    return false;
}

glm::vec2 Tile::CalculateTilePosition(int x, int y)
{
    float w = TILE_WIDTH;
    float h = TILE_HEIGHT;

    float dx = (w-(w/4)) * x + (x * TILE_OFFSET / 2 * glm::sqrt(3));
    float dy = (h * y) + (y * TILE_OFFSET);

    if (x & 1)
    {
        dy += (h + TILE_OFFSET) / 2;
    }

    return { dx, dy };
}
