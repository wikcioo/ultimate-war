#include "tile.h"

#include <vector>

#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "debug/debug_data.h"

Tile::Tile(int type, const glm::vec2& position)
    : m_Type(type), m_Position(position), m_IncomeValue(10), m_PlayerID(-1)
{
}

Tile::~Tile()
{
    for (auto unit : m_Units)
        delete unit;
}

void Tile::AddUnit(UnitType type)
{
    m_Units.emplace_back(new Unit(type));
}

void Tile::Draw(const glm::vec4& color)
{
    DrawBase(color);

#if defined(DEBUG)
    float ratio = DebugData::Get()->TileData.HeightRatio;
    float L = tileWidth / 2 * ratio;
    float dx = glm::cos(glm::radians(60.0f)) * L;
    float dy = glm::sin(glm::radians(60.0f)) * L;
#else
    static float ratio = 1.0f;
    static float L = tileWidth / 2 * ratio;
    static float dx = glm::cos(glm::radians(60.0f)) * L;
    static float dy = glm::sin(glm::radians(60.0f)) * L;
#endif

    glm::vec2 bgPos = {m_Position.x, m_Position.y + dy / 2};
    glm::vec2 bgSize = {tileWidth - 2 * dx, dy};

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
            ResourceManager::GetTexture(UnitTextureMap[(UnitType)((i % 5) + 1)])
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

bool Tile::InRange(const glm::vec2& cursorPos)
{
    static float w = tileWidth;
    static float h = tileHeight;

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


glm::vec2 Tile::CalculateTilePosition(int x, int y)
{
    float w = tileWidth;
    float h = tileHeight;

    float dx = (w-(w/4)) * x + (x * tileOffset / 2 * glm::sqrt(3));
    float dy = (h * y) + (y * tileOffset);

    if (x & 1)
    {
        dy += (h + tileOffset) / 2;
    }

    return { dx, dy };
}
