#include "tile.h"

#include <vector>

#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "debug/debug_data.h"
#include "game/player.h"

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

void Tile::Draw(const glm::vec4& color)
{
    if (!m_OwnedBy)
        DrawBase(color);
    else
    {
        auto c = m_OwnedBy->GetColor();
        DrawBase({c.r, c.g, c.b, 1.0f});
    }

    if (m_Units.empty()) return;

#if defined(DEBUG)
    float ratio = DebugData::Get()->TileData.HeightRatio;
    float L = TILE_WIDTH / 2 * ratio;
    float dx = glm::cos(glm::radians(60.0f)) * L;
    float dy = glm::sin(glm::radians(60.0f)) * L;
#else
    static float ratio = 1.0f;
    static float L = TILE_WIDTH / 2 * ratio;
    static float dx = glm::cos(glm::radians(60.0f)) * L;
    static float dy = glm::sin(glm::radians(60.0f)) * L;
#endif

    glm::vec2 bgPos = {m_Position.x, m_Position.y + dy / 2};
    glm::vec2 bgSize = {TILE_WIDTH - 2 * dx, dy};

    Renderer2D::DrawQuad(bgPos, bgSize, {0.0f, 0.4f, 0.0f, 0.4f});

#if defined(DEBUG)
    auto unitData = DebugData::Get()->UnitData;
    int unitsPerRow = unitData.UnitsPerRow;
    float unitOffsetWidth  = bgSize.x / ((unitData.UnitsPerRow * unitData.UnitWidthToOffsetRatio) + unitData.UnitsPerRow + 1);
    float unitWidth        = unitData.UnitWidthToOffsetRatio * unitOffsetWidth;
    float unitHeight       = glm::min(bgSize.y / 2, unitWidth);
    float unitOffsetHeight = glm::max(0.0f, (bgSize.y - (unitHeight * unitData.UnitRows)) / (unitData.UnitRows + 1));
#else
    static int unitRows = 2;
    static int unitsPerRow = 5;
    static int unitWidthToOffsetRatio = 10;
    static float unitOffsetWidth = bgSize.x / ((unitsPerRow * unitWidthToOffsetRatio) + unitsPerRow + 1);
    static float unitWidth = unitWidthToOffsetRatio * unitOffsetWidth;
    static float unitHeight = glm::min(bgSize.y / 2, unitWidth);
    static float unitOffsetHeight = glm::max(0.0f, (bgSize.y - (unitHeight * unitRows)) / (unitRows + 1));
#endif

    float initialX = bgPos.x - (bgSize.x - unitWidth) / 2 + unitOffsetWidth;

    float currentX = initialX;
    float currentY = bgPos.y + (bgSize.y - unitHeight) / 2 - unitOffsetHeight;

    for (int i = 0; i < m_Units.size(); i++)
    {
#if defined(DEBUG)
        if (DebugData::Get()->UnitData.ShowUnitBackground)
        {
            Renderer2D::DrawQuad(
                {currentX, currentY},
                {unitWidth, unitHeight},
                {0.8f, 0.1f, 0.1f, 0.6f}
            );
        }
#endif

        Renderer2D::DrawQuad(
            {currentX, currentY},
            {unitWidth, unitHeight},
            ResourceManager::GetTexture(UnitTextureMap[m_Units[i]->GetType()])
        );

        if ((i + 1) % unitsPerRow == 0)
        {
            currentX = initialX;
            currentY -= (unitOffsetHeight + unitHeight);
        }
        else
        {
            currentX += unitWidth + unitOffsetWidth;
        }
    }
}

void Tile::DrawBase(const glm::vec4& color)
{
    Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), color);
}

void Tile::SetOwnership(std::shared_ptr<Player> player)
{
    m_OwnedBy = player;
}

void Tile::TransferUnitsToTile(std::shared_ptr<Tile> destTile)
{
    if (destTile.get() == this) return;

    destTile->GetUnits().insert(destTile->GetUnits().end(), m_Units.begin(), m_Units.end());
    m_Units.clear();
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
