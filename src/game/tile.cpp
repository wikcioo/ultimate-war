#include "tile.h"

#include <vector>
#include <numeric>
#include <algorithm>

#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "debug/debug_data.h"
#include "game/player.h"
#include "util/util.h"
#include "game/game_layer.h"
#include "game/battle.h"

float Tile::s_BackgroundHeightRatio = 0.8f;

int Tile::s_UnitGroupRows = 3;
int Tile::s_UnitGroupsPerRow = 6;
int Tile::s_UnitGroupWidthToOffsetRatio = 10;

int Tile::s_BuildingRows = 1;
int Tile::s_BuildingsPerRow = 5;
int Tile::s_BuildingWidthToOffsetRatio = 10;

const int Tile::s_StatCount = 3;
const char* Tile::s_StatTextures[s_StatCount] = { "swords", "shield", "heart" };
const std::array<glm::ivec2, 6> Tile::s_AdjacentTileOffsets = {
    glm::ivec2(-1, 1), {0,  1}, {1, 1},
              {-1, 0}, {0, -1}, {1, 0}
};

std::shared_ptr<Texture2D> Tile::s_UpgradeIconTexture;

std::unordered_map<TileEnvironment, Resources> EnvironmentResourcesMap = {
    { TileEnvironment::NONE,      { 0, 0, 0, 0 } },
    { TileEnvironment::OCEAN,     { 1, 1, 1, 1 } },
    { TileEnvironment::FOREST,    { 6, 2, 1, 1 } },
    { TileEnvironment::DESERT,    { 2, 1, 6, 1 } },
    { TileEnvironment::MOUNTAINS, { 1, 6, 2, 1 } }
};

Tile::Tile(TileEnvironment environment, const glm::ivec2& coords)
    : m_Environment(environment), m_Coords(coords), m_Position(Tile::CalculateTilePosition(coords.x, coords.y))
{
    m_Resources = EnvironmentResourcesMap[m_Environment];
    InitStaticRuntimeData();
}

void Tile::InitStaticRuntimeData()
{
    s_UpgradeIconTexture = ResourceManager::GetTexture("up_arrow");
}

Tile::~Tile()
{
    for (auto unit : m_UnitGroups)
        delete unit;
}

void Tile::CreateUnitGroup(UnitGroupType type)
{
    if (!HasSpaceForUnitGroups(1))
    {
        LOG_WARN("Not enough space to add unit group of type '{0}' to tile", UnitGroupDataMap[type].TextureName);
        return;
    }

    if (AssetsCanExist())
    {
        // When creating a unit group, check if unit stats should be upgraded
        // based on the upgrade level of the required building
        BuildingType requiredBuilding = UnitGroupDataMap[type].RequiredBuilding;
        unsigned int maxRequiredBuildingLevel = 0;
        for (auto building : m_Buildings)
        {
            if (building->GetType() == requiredBuilding)
                maxRequiredBuildingLevel = glm::max(building->GetLevel(), maxRequiredBuildingLevel);
        }

        if (maxRequiredBuildingLevel > 0)
        {
            UnitStats* upgradedUnitStats = new UnitStats(UnitGroupDataMap[type].Stats + maxRequiredBuildingLevel);
            m_UnitGroups.emplace_back(new UnitGroup(type, upgradedUnitStats, GameLayer::Get().GetIteration()));
        }
        else
        {
            m_UnitGroups.emplace_back(new UnitGroup(type, std::nullopt, GameLayer::Get().GetIteration()));
        }
    }
    else
        LOG_WARN("Trying to add unit group of type '{0}' to non-existent tile", UnitGroupDataMap[type].TextureName);
}

void Tile::CreateUnitGroup(UnitGroup unitGroup)
{
    if (!HasSpaceForUnitGroups(1))
    {
        LOG_WARN("Not enough space to add unit group object of type '{0}' to tile",
                 UnitGroupDataMap[unitGroup.GetType()].TextureName);
        return;
    }

    if (AssetsCanExist())
        m_UnitGroups.emplace_back(new UnitGroup(unitGroup));
    else
        LOG_WARN("Trying to add unit group object of type '{0}' to non-existent tile",
                 UnitGroupDataMap[unitGroup.GetType()].TextureName);
}

bool Tile::CanRecruitUnitGroup(UnitGroupType type)
{
    if (type == UnitGroupType::NONE || type == UnitGroupType::COUNT)
        return false;

    BuildingType requirement = UnitGroupDataMap[type].RequiredBuilding;
    if (requirement == BuildingType::NONE)
        return true;

    auto it = std::find_if(
        m_Buildings.begin(),
        m_Buildings.end(),
        [requirement](const Building* b)
        {
            return requirement == b->GetType();
        }
    );

    return it != m_Buildings.end();
}

bool Tile::HasSpaceForUnitGroups(int num)
{
    return m_UnitGroups.size() + num <= s_UnitGroupRows * s_UnitGroupsPerRow;
}

bool Tile::HasSpaceForBuildings(int num)
{
    return m_Buildings.size() + num <= s_BuildingRows * s_BuildingsPerRow;
}

void Tile::CreateBuilding(BuildingType type)
{
    if (!HasSpaceForBuildings(1))
    {
        LOG_WARN("Not enough space to add building of type '{0}' to tile", BuildingDataMap[type].TextureName);
        return;
    }

    if (AssetsCanExist())
        m_Buildings.emplace_back(new Building(type));
    else
        LOG_WARN("Trying to add building of type '{0}' to non-existent tile", BuildingDataMap[type].TextureName);
}

void Tile::CreateBuilding(Building building)
{
    if (!HasSpaceForBuildings(1))
    {
        LOG_WARN("Not enough space to add building of type '{0}' to tile",
                 BuildingDataMap[building.GetType()].TextureName);
        return;
    }

    if (AssetsCanExist())
        m_Buildings.emplace_back(new Building(building));
    else
        LOG_WARN("Trying to add building of type '{0}' to non-existent tile",
                 BuildingDataMap[building.GetType()].TextureName);
}

void Tile::Draw()
{
    DrawEnvironment();

    if (m_OwnedBy)
    {
        // If tile owned by player, draw border around the tile with player's color
        Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), glm::vec4(m_OwnedBy->GetColor(), 1.0f), 3.0f);
    }

    DrawUnitGroups();
    DrawBuildings();

    if (GameLayer::Get().IsEarnedResourcesInfoVisible() &&
        GameLayer::Get().GetPlayerManager()->GetCurrentPlayer() == m_OwnedBy)
    {
        DrawEarnedResourcesInfoOverlay();
    }
}

DrawData Tile::GetUnitGroupDrawData()
{
    float L = TILE_WIDTH / 2 * s_BackgroundHeightRatio;
    float dx = glm::cos(glm::radians(60.0f)) * L;
    float dy = glm::sin(glm::radians(60.0f)) * L;

    glm::vec2 bgPos = {m_Position.x, m_Position.y + dy / 2};
    glm::vec2 bgSize = {TILE_WIDTH - 2 * dx, dy};

    float unitOffsetWidth  = bgSize.x / ((s_UnitGroupsPerRow * s_UnitGroupWidthToOffsetRatio) + s_UnitGroupsPerRow + 1);
    float unitWidth        = s_UnitGroupWidthToOffsetRatio * unitOffsetWidth;
    float unitHeight       = glm::min(bgSize.y / s_UnitGroupRows, unitWidth);
    float unitOffsetHeight = glm::max(0.0f, (bgSize.y - (unitHeight * s_UnitGroupRows)) / (s_UnitGroupRows + 1));

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
    float buildingHeight       = glm::min(bgSize.y / s_BuildingRows, buildingWidth);
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

void Tile::DrawUnitGroups()
{
    if (m_UnitGroups.empty()) return;

    auto unitData = GetUnitGroupDrawData();
    float initialX = unitData.Position.x;
    bool isCurrentPlayer = GameLayer::Get().GetPlayerManager()->GetCurrentPlayer() == m_OwnedBy;

    int totalStats[s_StatCount] = { 0, 0, 0 };
    int selectedStats[s_StatCount] = { 0, 0, 0 };

    Renderer2D::DrawQuad(unitData.BackgroundPosition, unitData.BackgroundSize, {0.0f, 0.4f, 0.0f, 0.4f});

    for (int i = 0; i < m_UnitGroups.size(); i++)
    {
        auto unitStatsVector = m_UnitGroups[i]->GetUnitStats();

        if (m_UnitGroups[i]->IsSelected())
        {
            Renderer2D::DrawQuad(
                unitData.Position,
                unitData.Size,
                {0.8f, 0.1f, 0.1f, 0.6f}
            );

            for (auto unitStats : unitStatsVector)
            {
                selectedStats[0] += unitStats->Attack;
                selectedStats[1] += unitStats->Health;
                selectedStats[2] += unitStats->Defense;

                totalStats[0] += unitStats->Attack;
                totalStats[1] += unitStats->Health;
                totalStats[2] += unitStats->Defense;
            }
        }
        else
        {
            for (auto unitStats : unitStatsVector)
            {
                totalStats[0] += unitStats->Attack;
                totalStats[1] += unitStats->Health;
                totalStats[2] += unitStats->Defense;
            }
        }

        Renderer2D::DrawQuad(
            unitData.Position,
            unitData.Size,
            ResourceManager::GetTexture(UnitGroupDataMap[m_UnitGroups[i]->GetType()].TextureName)
        );

        if (m_UnitGroups[i]->UnitWasMovedInIteration(GameLayer::Get().GetIteration()) && isCurrentPlayer)
        {
            Renderer2D::DrawQuad(
                unitData.Position,
                unitData.Size,
                {0.2f, 0.2f, 0.2f, 0.5f}
            );
        }

        if ((i + 1) % s_UnitGroupsPerRow == 0)
        {
            unitData.Position.x = initialX;
            unitData.Position.y -= (unitData.OffsetSize.y + unitData.Size.y);
        }
        else
        {
            unitData.Position.x += unitData.Size.x + unitData.OffsetSize.x;
        }
    }

    DrawCountedStats(unitData, totalStats, selectedStats);
}

void Tile::DrawUnitGroupStats(DrawData& unitData, UnitGroup* unitGroup)
{
    auto unitType = unitGroup->GetType();
    int stats[s_StatCount] = { 0, 0, 0 };
    auto unitStatsVector = unitGroup->GetUnitStats();
    for (auto unitStats : unitStatsVector)
    {
        stats[0] += unitStats->Attack;
        stats[1] += unitStats->Defense;
        stats[2] += unitStats->Health;
    }

    static float hOffset = 0.035f;
    static float statSize = 0.05f;
    static float textScale = 0.2f;

    Renderer2D::DrawQuad(
        unitData.Position,
        unitData.Size,
        {0.0f, 0.0f, 0.0f, 0.6f}
    );

    int unitStatsVectorSize = unitStatsVector.size();
    int totalBaseStats[s_StatCount] = {
        UnitGroupDataMap[unitGroup->GetType()].Stats.Attack  * unitStatsVectorSize,
        UnitGroupDataMap[unitGroup->GetType()].Stats.Defense * unitStatsVectorSize,
        UnitGroupDataMap[unitGroup->GetType()].Stats.Health  * unitStatsVectorSize
    };

    for (int i = 0; i < s_StatCount; i++)
    {
        Renderer2D::DrawQuad(
            glm::vec2(unitData.Position.x - statSize, unitData.Position.y + statSize),
            glm::vec2(statSize),
            ResourceManager::GetTexture(s_StatTextures[i])
        );

        glm::vec3 statColor = glm::vec3(1.0f);

        if (stats[i] < totalBaseStats[i])
            statColor = glm::vec3(1.0f, 0.0f, 0.0f);
        else if (stats[i] > totalBaseStats[i])
            statColor = glm::vec3(0.0f, 1.0f, 0.0f);

        Renderer2D::DrawTextStr(
            std::to_string(stats[i]),
            { unitData.Position.x - statSize + hOffset, unitData.Position.y + statSize },
            textScale / GameLayer::Get().GetCameraController()->GetCamera()->GetZoom(),
            statColor, HTextAlign::LEFT, VTextAlign::MIDDLE, "rexlia"
        );

        unitData.Position.y -= statSize;
    }
}

void Tile::DrawCountedStats(DrawData& unitData, int totalStats[], int selectedStats[])
{
    static float yOffset = TILE_HEIGHT / 2.0f - 0.45f;
    static float statSize = 0.10f;
    static float textScale = 0.30f;

    auto currPlayer = GameLayer::Get().GetPlayerManager()->GetCurrentPlayer();

    glm::vec2 statPos = {m_Position.x - 0.45f, m_Position.y - yOffset};
    for (int i = 0; i < s_StatCount; i++)
    {
        std::string statText =
            m_OwnedBy == currPlayer ?
            std::to_string(selectedStats[i]) + " / " + std::to_string(totalStats[i]) :
            std::to_string(totalStats[i]);

        Renderer2D::DrawQuad(
            glm::vec2(statPos.x, statPos.y - statSize),
            glm::vec2(statSize),
            ResourceManager::GetTexture(s_StatTextures[i])
        );
        Renderer2D::DrawTextStr(
            statText,
            { statPos.x, statPos.y },
            textScale / GameLayer::Get().GetCameraController()->GetCamera()->GetZoom(),
            glm::vec3(1.0f), HTextAlign::MIDDLE, VTextAlign::MIDDLE, "rexlia"
        );
        statPos.x += 0.45;
    }
}

void Tile::CheckUnitGroupHover(const glm::vec2& relMousePos)
{
    auto unitData = GetUnitGroupDrawData();
    float initialX = unitData.Position.x;

    for (int i = 0; i < m_UnitGroups.size(); i++)
    {
        if (Util::IsPointInRectangle(unitData.Position, unitData.Size, relMousePos))
        {
            DrawUnitGroupStats(unitData, m_UnitGroups[i]);
            return;
        }

        if ((i + 1) % s_UnitGroupsPerRow == 0)
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

void Tile::CheckBuildingHover(const glm::vec2& relMousePos)
{
    if (GameLayer::Get().GetPlayerManager()->GetCurrentPlayer() != m_OwnedBy) return;

    auto buildingData = GetBuildingDrawData();
    float initialX = buildingData.Position.x;
    static glm::vec2 upgradeIconSize = buildingData.Size * 0.3f;

    for (int i = 0; i < m_Buildings.size(); i++)
    {
        glm::vec2 upgradeIconPosition = buildingData.Position + buildingData.Size / 2.0f - upgradeIconSize / 2.0f;

        if (Util::IsPointInRectangle(buildingData.Position, buildingData.Size, relMousePos))
        {
            // Upgrade icon
            Renderer2D::DrawQuad(
                upgradeIconPosition,
                upgradeIconSize,
                s_UpgradeIconTexture
            );

            if (Util::IsPointInRectangle(upgradeIconPosition, upgradeIconSize, relMousePos))
            {
                // Draw bigger upgrade icon to imitate bolding
                Renderer2D::DrawQuad(
                    upgradeIconPosition,
                    glm::vec2(upgradeIconSize * 1.1f),
                    s_UpgradeIconTexture
                );
            }
        }

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

void Tile::DrawBuildings()
{
    if (m_Buildings.empty()) return;

    auto buildingData = GetBuildingDrawData();
    float initialX = buildingData.Position.x;
    auto camera = GameLayer::Get().GetCameraController()->GetCamera();

    Renderer2D::DrawQuad(buildingData.BackgroundPosition, buildingData.BackgroundSize, {0.4f, 0.4f, 0.4f, 1.0f});

    for (int i = 0; i < m_Buildings.size(); i++)
    {
        // Building texture
        Renderer2D::DrawQuad(
            buildingData.Position,
            buildingData.Size,
            ResourceManager::GetTexture(BuildingDataMap[m_Buildings[i]->GetType()].TextureName)
        );

        // Level number
        // TODO(Viktor): Remove once having separater textures for different building levels
        Renderer2D::DrawTextStr(
            "lvl " + std::to_string(m_Buildings[i]->GetLevel()),
            buildingData.Position - buildingData.Size / 2.0f,
            0.3f / camera->GetZoom(),
            glm::vec3(1.0f),
            HTextAlign::LEFT,
            VTextAlign::BOTTOM,
            "rexlia"
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

void Tile::DrawEarnedResourcesInfoOverlay()
{
    auto camera = GameLayer::Get().GetCameraController()->GetCamera();

    Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));

    static auto resourceData = Resources::GetResourceData();
    auto resources = GetResources();

    int resourceNumbers[resourceData.NumResources] = {
        resources.Wood,
        resources.Rock,
        resources.Steel,
        resources.Gold
    };

    float yStartOffset = 0.3f;
    float yOffset = 0.2f;
    for (int i = 0; i < resourceData.NumResources; i++)
    {
        Renderer2D::DrawQuad(
            {
                m_Position.x - 0.1f,
                m_Position.y + yStartOffset - i * yOffset
            },
            glm::vec2(0.15f * resourceData.ResourceTextureScales[i]),
            resourceData.ResourceTextures[i]
        );

        Renderer2D::DrawTextStr(
            std::to_string(resourceNumbers[i]),
            {
                m_Position.x + 0.02f,
                m_Position.y + yStartOffset - i * yOffset
            },
            0.5f / camera->GetZoom(),
            resourceData.ResourceNumberColors[i],
            HTextAlign::LEFT,
            VTextAlign::MIDDLE,
            "rexlia"
        );
    }
}

void Tile::DrawEnvironment()
{
    if (m_Environment != TileEnvironment::NONE)
    {
        glm::vec3 color;
        float yOffset = TILE_HEIGHT / 2.0f - 0.15f;
        static auto tileColors = ColorData::Get().TileColors;
        switch (m_Environment)
        {
            case TileEnvironment::OCEAN:
            {
                static auto waterShader = ResourceManager::GetShader("water");
                Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), waterShader);
                return;
            }
            case TileEnvironment::FOREST:
            {
                Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), { tileColors.ForestColor, 1.0f });
                Renderer2D::DrawQuad({m_Position.x, m_Position.y - yOffset}, glm::vec2(0.2f), ResourceManager::GetTexture("tree"));
                break;
            }
            case TileEnvironment::DESERT:
            {
                Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), { tileColors.DesertColor, 1.0f });
                Renderer2D::DrawQuad({m_Position.x, m_Position.y - yOffset}, glm::vec2(0.2f), ResourceManager::GetTexture("sand"));
                break;
            }
            case TileEnvironment::MOUNTAINS:
            {
                Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), { tileColors.MountainsColor, 1.0f });
                Renderer2D::DrawQuad({m_Position.x, m_Position.y - yOffset}, glm::vec2(0.2f), ResourceManager::GetTexture("stone"));
                break;
            }
            case TileEnvironment::HIGHLIGHT:
            {
                Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), { 0.5f, 0.5f, 0.5f, 1.0f }, 5.0f);
                break;
            }
            default:
            {
                Renderer2D::DrawHexagon(m_Position, glm::vec2(1.0f), { 1.0f, 0.0f, 1.0f, 1.0f });
            }
        }
    }
}

const Resources Tile::GetResources() const
{
    int extraGold = 0;
    for (auto building : m_Buildings)
    {
        if (building->GetType() == BuildingType::GOLD_MINE)
            extraGold += building->GetLevel() * 2 + 2;
    }

    return {
        m_Resources.Wood,
        m_Resources.Rock,
        m_Resources.Steel,
        m_Resources.Gold + extraGold
    };
}

int Tile::GetNumSelectedUnitGroups()
{
    return std::accumulate(m_UnitGroups.begin(), m_UnitGroups.end(), 0, [](int sum, UnitGroup* ug) {
        if (ug->IsSelected())
            return sum + 1;
        return sum;
    });
}

void Tile::SetOwnership(const std::shared_ptr<Player>& player)
{
    m_OwnedBy = player;
}

void Tile::ChangeOwnership(const std::shared_ptr<Player>& player)
{
    if(m_OwnedBy != nullptr)
        m_OwnedBy->RemoveOwnedTile(shared_from_this());

    player->AddOwnedTile(shared_from_this());
}

void Tile::MoveToTile(const std::shared_ptr<Tile>& destTile)
{
    if(destTile->m_OwnedBy == m_OwnedBy)
    {
        TransferUnitGroupsToTile(destTile);
        return;
    }

    if(Battle::CalculateBattleOutcome(shared_from_this(), destTile) == BattleOutcome::ATTACKER_WON)
    {
        destTile->GetUnitGroups().clear();
        auto defender = destTile->GetOwnedBy();
        destTile->ChangeOwnership(this->m_OwnedBy);
        TransferUnitGroupsToTile(destTile);

        GameLayer::Get().GetPlayerManager()->UpdatePlayerStatus(defender);
    }
}

void Tile::TransferUnitGroupsToTile(const std::shared_ptr<Tile>& destTile)
{
    if (destTile.get() == this) return;

    for (auto unit : m_UnitGroups)
    {
        if (!unit->IsSelected()) continue;

        destTile->GetUnitGroups().push_back(unit);
        unit->SetMovedOnIteration(GameLayer::Get().GetIteration());
    }

    EraseSelectedUnitGroups();

    destTile->DeselectAllUnitGroups();
}

void Tile::EraseSelectedUnitGroups()
{
    m_UnitGroups.erase(std::remove_if(m_UnitGroups.begin(), m_UnitGroups.end(), [](UnitGroup* unit) {
        return unit->IsSelected();
    }), m_UnitGroups.end());
}

bool Tile::HandleUnitGroupMouseClick(const glm::vec2& relMousePos)
{
    auto unitData = GetUnitGroupDrawData();
    float initialX = unitData.Position.x;

    for (int i = 0; i < m_UnitGroups.size(); i++)
    {
        if (Util::IsPointInRectangle(
            unitData.Position,
            unitData.Size,
             relMousePos))
        {
            if (!m_UnitGroups[i]->UnitWasMovedInIteration(GameLayer::Get().GetIteration()))
            {
                m_UnitGroups[i]->ToggleSelected();
            }

            return true;
        }

        if ((i + 1) % s_UnitGroupsPerRow == 0)
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

bool Tile::HandleBuildingUpgradeIconMouseClick(const glm::vec2& relMousePos)
{
    auto buildingData = GetBuildingDrawData();
    float initialX = buildingData.Position.x;
    static glm::vec2 upgradeIconSize = buildingData.Size * 0.3f;

    for (int i = 0; i < m_Buildings.size(); i++)
    {
        if (Util::IsPointInRectangle(
            buildingData.Position + buildingData.Size / 2.0f - upgradeIconSize / 2.0f,
            upgradeIconSize,
            relMousePos))
        {
            // NOTE: Consider using non-linear function for calculating cost based on current building upgrade level
            Resources upgradeCost = BuildingDataMap[m_Buildings[i]->GetType()].BaseUpgradeCost * (m_Buildings[i]->GetLevel() + 1);
            if (GameLayer::Get().GetPlayerManager()->GetCurrentPlayer()->SubtractResources(upgradeCost))
            {
                m_Buildings[i]->Upgrade();
                return true;
            }
        }

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

    return false;
}

bool Tile::IsMouseClickedInsideUnitGroupsBox(const glm::vec2& relMousePos)
{
    auto unitData = GetUnitGroupDrawData();
    return Util::IsPointInRectangle(unitData.BackgroundPosition, unitData.BackgroundSize, relMousePos);
}

void Tile::DeselectAllUnitGroups()
{
    for (auto unit : m_UnitGroups)
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

bool Tile::HasSelectedUnitGroups()
{
    for (auto unit : m_UnitGroups)
    {
        if (unit->IsSelected())
            return true;
    }

    return false;
}

bool Tile::IsAdjacent(const glm::ivec2& tile1, const glm::ivec2& tile2)
{
    int offset = 0;
    if (tile1.x % 2 == 0)
        offset = -1;

    for (auto tileOffset : s_AdjacentTileOffsets)
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

std::string Tile::GetEnvironmentName(TileEnvironment environment)
{
    switch (environment)
    {
        case TileEnvironment::NONE:      return "None";
        case TileEnvironment::OCEAN:     return "Ocean";
        case TileEnvironment::FOREST:    return "Forest";
        case TileEnvironment::DESERT:    return "Desert";
        case TileEnvironment::MOUNTAINS: return "Mountains";
        case TileEnvironment::HIGHLIGHT: return "Highlight";
    }

    return "unknown";
}

void Tile::AddRandomUnits()
{
    int unitGroupCount = (int)Util::GenerateRandomNumber(2, 6);

    for (int i = 0; i < unitGroupCount; i++)
    {
        UnitGroupType unitGroupType = (UnitGroupType)Util::GenerateRandomNumber(0, (double)UnitGroupType::COUNT);
        this->CreateUnitGroup(unitGroupType);
    }
}
