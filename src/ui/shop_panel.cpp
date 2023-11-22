#include "shop_panel.h"

#include "util/util.h"
#include "core/logger.h"
#include "graphics/renderer.h"
#include "game/color_data.h"
#include "core/resource_manager.h"
#include "game/game_layer.h"

#include <GLFW/glfw3.h>

static glm::vec2 GetShopPanelSize(const glm::vec2& assetSize, float assetOffset)
{
    return glm::vec2(5 * (assetSize.x + assetOffset) + assetOffset, assetSize.y + assetOffset);
}

ShopPanel::ShopPanel(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset,
                     const glm::vec2& assetSize, float assetOffset)
    : UIElement(
        UICamera,
        UICamera->CalculateRelativeBottomLeftPosition() + glm::vec2(offset.x - GetShopPanelSize(assetSize, assetOffset).x / 2.0f, offset.y),
        GetShopPanelSize(assetSize, assetOffset)
      ),
      m_AssetSize(assetSize), m_AssetOffset(assetOffset), m_Offset(offset),
      m_UnitGroupCount((int)UnitGroupType::COUNT), m_BuildingCount((int)BuildingType::COUNT),
      m_AssetBorderMargin(0.015f), m_AssetBorderThickness(10.0f), m_AssetPriceSize(0.125f),
      m_AssetPriceFontName("rexlia"), m_Hidden(true)
{
    m_ShopPanelIcon.CornerOffset = glm::vec2(0.05f);
    m_ShopPanelIcon.Size = glm::vec2(0.15f);
    m_ShopPanelIcon.Position = {
        m_UICamera->GetHalfOfRelativeWidth() - m_ShopPanelIcon.Size.x - m_ShopPanelIcon.CornerOffset.x,
        -m_UICamera->GetHalfOfRelativeHeight() + m_ShopPanelIcon.Size.y + m_ShopPanelIcon.CornerOffset.y
    };
}

void ShopPanel::OnEvent(Event& event)
{
    if (event.GetCategory() == EventCategory::Window)
        OnWindowSizeChanged();

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ShopPanel::OnKeyPressed));

    auto mousePos = m_UICamera->CalculateRelativeMousePosition();

    if (mousePos.x > m_Position.x && mousePos.x < m_Position.x + m_Size.x &&
        mousePos.y > m_Position.y && mousePos.y < m_Position.y + m_Size.y * 2)
    {
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(ShopPanel::OnMouseScrolled));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ShopPanel::OnMouseButtonPressedPanel));
    }
    else if (mousePos.x > m_ShopPanelIcon.Position.x - m_ShopPanelIcon.Size.x / 2.0f &&
             mousePos.x < m_ShopPanelIcon.Position.x + m_ShopPanelIcon.Size.x / 2.0f &&
             mousePos.y > m_ShopPanelIcon.Position.y - m_ShopPanelIcon.Size.y / 2.0f &&
             mousePos.y < m_ShopPanelIcon.Position.y + m_ShopPanelIcon.Size.y / 2.0f)
    {
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ShopPanel::OnMouseButtonShopPanelIconPressed));
    }
    else
    {
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ShopPanel::OnMouseButtonPressedGame));
    }
}

void ShopPanel::Draw()
{
    Renderer2D::BeginScene(m_UICamera);

    auto cursorPos = m_UICamera->CalculateRelativeMousePosition();

    DrawShopPanelIcon(cursorPos);

    if (!m_Hidden)
    {
        DrawBuildings(cursorPos);
        DrawUnitGroups(cursorPos);

        if (IsAssetAttachedToCursor())
        {
            Renderer2D::DrawQuad(cursorPos, m_AssetSize * 0.5f, m_CursorAttachedAsset.Texture);
            ProcessInvalidAssetPlacement(cursorPos);
        }
    }

    Renderer2D::EndScene();
}

void ShopPanel::ProcessInvalidAssetPlacement(const glm::vec2& cursorPos)
{
    static auto crossTexture = ResourceManager::GetTexture("cross");

    auto relMousePos = GameLayer::Get().GetCameraController()->GetCamera()->CalculateRelativeMousePosition();
    auto currentPlayer = GameLayer::Get().GetPlayerManager()->GetCurrentPlayer();
    auto gameMap = GameLayer::Get().GetGameMapManager()->GetGameMap();

    for (int y = 0; y < gameMap->GetTileCountY(); y++)
    {
        for (int x = 0; x < gameMap->GetTileCountX(); x++)
        {
            auto tile = gameMap->GetTile(x, y);
            if (tile->InRange(relMousePos))
            {
                if (tile->GetOwnedBy() == currentPlayer)
                {
                    if (m_CursorAttachedAsset.UnitGroupType != UnitGroupType::NONE &&
                       (!tile->HasSpaceForUnitGroups(1) || !tile->CanRecruitUnitGroup(m_CursorAttachedAsset.UnitGroupType)) ||
                        m_CursorAttachedAsset.BuildingType != BuildingType::NONE && !tile->HasSpaceForBuildings(1) ||
                        !tile->AssetsCanExist())
                    {
                        Renderer2D::DrawQuad(cursorPos, glm::vec2(m_AssetPriceSize * 0.5f), crossTexture);
                    }
                }
                else
                {
                    Renderer2D::DrawQuad(cursorPos, glm::vec2(m_AssetPriceSize * 0.5f), crossTexture);
                }
            }
        }
    }
}

void ShopPanel::DrawShopPanelIcon(const glm::vec2& cursorPos)
{
    static auto openShopPanelIcon = ResourceManager::GetTexture("chest_open");
    static auto closedShopPanelIcon = ResourceManager::GetTexture("chest_closed");

    auto shopPanelIcon = m_Hidden ? closedShopPanelIcon : openShopPanelIcon;
    Renderer2D::DrawQuad(
        m_ShopPanelIcon.Position,
        m_ShopPanelIcon.Size,
        shopPanelIcon
    );
}

void ShopPanel::DrawUnitGroups(const glm::vec2& cursorPos)
{
    Renderer2D::DrawQuad(
        m_Position + m_Size * 0.5f,
        m_Size,
        ColorData::Get().UITheme.ShopPanelBackgroundColor
    );

    for (int i = 0; i < m_UnitGroupCount; i++)
    {
        glm::vec2 unitPos = {
            m_Position.x + m_AssetSize.x / 2 + m_AssetOffset + (m_AssetSize.x + m_AssetOffset) * i,
            m_Position.y + m_Size.y / 2
        };

        auto unitData = UnitGroupDataMap[(UnitGroupType)i];

        Renderer2D::DrawQuad(unitPos, m_AssetSize, ResourceManager::GetTexture(unitData.TextureName));

        if (Util::IsPointInRectangle(unitPos, m_AssetSize, cursorPos) || (UnitGroupType)i == m_CursorAttachedAsset.UnitGroupType)
        {
            Renderer2D::DrawQuad(
                unitPos,
                m_AssetSize + m_AssetBorderMargin,
                ColorData::Get().UITheme.ShopPanelHighlighUnitGroupColor,
                m_AssetBorderThickness
            );

            DrawAssetInfo(unitData.TextureName, unitData.Cost, unitData.RequiredBuilding);
        }
    }
}

void ShopPanel::DrawBuildings(const glm::vec2& cursorPos)
{
    Renderer2D::DrawQuad(
        glm::vec2(m_Position.x + m_Size.x * 0.5f, m_Position.y + m_Size.y * 1.5f),
        m_Size,
        ColorData::Get().UITheme.ShopPanelBackgroundColor
    );

    for (int i = 0; i < m_BuildingCount; i++)
    {
        glm::vec2 buildingPos = {
            m_Position.x + m_AssetSize.x / 2 + m_AssetOffset + (m_AssetSize.x + m_AssetOffset) * i,
            m_Position.y + m_Size.y / 2 + m_Size.y
        };

        auto buildingData = BuildingDataMap[(BuildingType)i];

        Renderer2D::DrawQuad(buildingPos, m_AssetSize, ResourceManager::GetTexture(buildingData.TextureName));

        if (Util::IsPointInRectangle(buildingPos, m_AssetSize, cursorPos) || (BuildingType)i == m_CursorAttachedAsset.BuildingType)
        {
            Renderer2D::DrawQuad(
                buildingPos,
                m_AssetSize + m_AssetBorderMargin,
                ColorData::Get().UITheme.ShopPanelHighlighUnitGroupColor,
                m_AssetBorderThickness
            );

            DrawAssetInfo(buildingData.TextureName, buildingData.Cost);
        }
    }
}

void ShopPanel::DrawAssetInfo(const std::string& name, const Resources& cost,
                              std::optional<BuildingType> requiredBuilding)
{
    glm::vec2 pos = {
        m_Position.x + m_Size.x + m_Size.y + 0.05f,
        m_Position.y + m_Size.y
    };
    glm::vec2 size = glm::vec2(m_Size.y * 2);
    glm::vec4 color = ColorData::Get().UITheme.ShopPanelBackgroundColor;

    // Background
    Renderer2D::DrawQuad(pos, size, color);

    // Asset name
    Renderer2D::DrawTextStr(
        Util::ReplaceChar(name, '_', ' '),
        { pos.x, pos.y + 0.16f },
        0.15f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::MIDDLE,
        "rexlia"
    );

    // Asset texture
    Renderer2D::DrawQuad(
        { pos.x, pos.y + 0.08f },
        m_AssetSize * 0.7f,
        ResourceManager::GetTexture(name)
    );

    // Price
    static auto resourceData = Resources::GetResourceData();

    static float hOffset = 0.09f;
    static float resSize = 0.07f;
    static float resHOffset = 0.02f;
    static float textScale = 0.2f;

    int costValues[resourceData.NumResources] = {
        cost.Wood,
        cost.Rock,
        cost.Steel,
        cost.Gold,
    };

    for (int i = 0; i < resourceData.NumResources; i++)
    {
        Renderer2D::DrawQuad(
            glm::vec2(
                pos.x - hOffset * glm::pow(-1.0, (double)(i % 2)) - hOffset / 2.0f,
                pos.y - resHOffset - resSize * Util::Clamp<int>(i - 1, 0, 1)
            ),
            glm::vec2(resSize),
            resourceData.ResourceTextures[i]
        );

        Renderer2D::DrawTextStr(
            std::to_string(costValues[i]),
            glm::vec2(
                pos.x - resSize / 2.4f - hOffset * glm::pow(-1.0, (double)(i % 2)) + hOffset / 3.2f,
                pos.y - resHOffset - resSize * Util::Clamp<int>(i - 1, 0, 1)
            ),
            textScale,
            glm::vec3(1.0f),
            HTextAlign::LEFT, VTextAlign::MIDDLE,
            "rexlia"
        );
    }

    // Draw required building if necessary
    if (requiredBuilding.has_value() && requiredBuilding.value() != BuildingType::NONE)
    {
        Renderer2D::DrawTextStr(
            std::string("Needs " + Util::ReplaceChar(BuildingDataMap[requiredBuilding.value()].TextureName, '_', ' ')).c_str(),
            glm::vec2(pos.x, pos.y - 0.16f),
            0.12f,
            glm::vec3(1.0f),
            HTextAlign::MIDDLE,
            VTextAlign::MIDDLE,
            "rexlia"
        );
    }
}

std::string ShopPanel::GetCostText(Resources& cost)
{
    std::ostringstream oss;
    oss << cost.Wood << " Wood\n" << cost.Rock << " Rock\n" << cost.Steel << " Steel\n" << cost.Gold << " Gold";
    return oss.str();
}

void ShopPanel::ToggleShopPanelVisibility()
{
    m_Hidden = !m_Hidden;
    m_CursorAttachedAsset.UnitGroupType = UnitGroupType::NONE;
    m_CursorAttachedAsset.BuildingType = BuildingType::NONE;
    m_CursorAttachedAsset.Texture = nullptr;
}

void ShopPanel::OnWindowSizeChanged()
{
    glm::vec2 bottomLeft = m_UICamera->CalculateRelativeBottomLeftPosition();
    m_Position = glm::vec2(
        bottomLeft.x + m_UICamera->GetHalfOfRelativeWidth() - m_Size.x / 2.0f,
        bottomLeft.y + m_Offset.y
    );

    m_ShopPanelIcon.Position = {
        m_UICamera->GetHalfOfRelativeWidth() - m_ShopPanelIcon.Size.x - m_ShopPanelIcon.CornerOffset.x,
        -m_UICamera->GetHalfOfRelativeHeight() + m_ShopPanelIcon.Size.y + m_ShopPanelIcon.CornerOffset.y
    };
}

bool ShopPanel::OnMouseScrolled(MouseScrolledEvent& event)
{
    return true;
}

bool ShopPanel::OnKeyPressed(KeyPressedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_ESCAPE)
    {
        m_CursorAttachedAsset.UnitGroupType = UnitGroupType::NONE;
        m_CursorAttachedAsset.BuildingType = BuildingType::NONE;
        m_CursorAttachedAsset.Texture.reset();
        return true;
    }
    else if (event.GetKeyCode() == GLFW_KEY_B)
    {
        ToggleShopPanelVisibility();
        return true;
    }

    return false;
}

bool ShopPanel::OnMouseButtonPressedPanel(MouseButtonPressedEvent& event)
{
    auto cursorPos = m_UICamera->CalculateRelativeMousePosition();
    for (int i = 0; i < m_UnitGroupCount; i++)
    {
        glm::vec2 unitPos = {
            m_Position.x + m_AssetSize.x / 2 + m_AssetOffset + (m_AssetSize.x + m_AssetOffset) * i,
            m_Position.y + m_Size.y / 2
        };

        if (Util::IsPointInRectangle(unitPos, m_AssetSize, cursorPos))
        {
            SetCursorAttachedAsset((UnitGroupType)i);
            return true;
        }
    }

    for (int i = 0; i < m_BuildingCount; i++)
    {
        glm::vec2 buildingPos = {
            m_Position.x + m_AssetSize.x / 2 + m_AssetOffset + (m_AssetSize.x + m_AssetOffset) * i,
            m_Position.y + m_Size.y / 2 + m_Size.y
        };

        if (Util::IsPointInRectangle(buildingPos, m_AssetSize, cursorPos))
        {
            SetCursorAttachedAsset((BuildingType)i);
            return true;
        }
    }

    return false;
}

bool ShopPanel::OnMouseButtonShopPanelIconPressed(MouseButtonPressedEvent& event)
{
    ToggleShopPanelVisibility();
    return true;
}

bool ShopPanel::OnMouseButtonPressedGame(MouseButtonPressedEvent& event)
{
    if (!m_CursorAttachedAsset.Texture.get() || !GameLayer::Get().IsGameActive()) return false;

    auto relMousePos = GameLayer::Get().GetCameraController()->GetCamera()->CalculateRelativeMousePosition();
    auto currentPlayer = GameLayer::Get().GetPlayerManager()->GetCurrentPlayer();
    auto gameMap = GameLayer::Get().GetGameMapManager()->GetGameMap();

    for (int y = 0; y < gameMap->GetTileCountY(); y++)
    {
        for (int x = 0; x < gameMap->GetTileCountX(); x++)
        {
            auto tile = gameMap->GetTile(x, y);
            if (tile->InRange(relMousePos))
            {
                if (tile->GetOwnedBy() == currentPlayer)
                {
                    if (m_CursorAttachedAsset.UnitGroupType != UnitGroupType::NONE &&
                        tile->CanRecruitUnitGroup(m_CursorAttachedAsset.UnitGroupType) &&
                        tile->HasSpaceForUnitGroups(1) &&
                        currentPlayer->SubtractResources(UnitGroupDataMap[m_CursorAttachedAsset.UnitGroupType].Cost))
                    {
                        tile->CreateUnitGroup(m_CursorAttachedAsset.UnitGroupType);
                        return true;
                    }
                    else if (m_CursorAttachedAsset.BuildingType != BuildingType::NONE &&
                             tile->HasSpaceForBuildings(1) &&
                             currentPlayer->SubtractResources(BuildingDataMap[m_CursorAttachedAsset.BuildingType].Cost))
                    {
                        tile->CreateBuilding(m_CursorAttachedAsset.BuildingType);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void ShopPanel::SetCursorAttachedAsset(std::variant<UnitGroupType, BuildingType> type)
{
    if (std::holds_alternative<UnitGroupType>(type))
    {
        m_CursorAttachedAsset.UnitGroupType = std::get<UnitGroupType>(type);
        if (m_CursorAttachedAsset.UnitGroupType == UnitGroupType::NONE)
            m_CursorAttachedAsset.Texture.reset();
        else
        {
            m_CursorAttachedAsset.Texture = ResourceManager::GetTexture(UnitGroupDataMap[m_CursorAttachedAsset.UnitGroupType].TextureName);
            m_CursorAttachedAsset.BuildingType = BuildingType::NONE;
        }
    }
    else if (std::holds_alternative<BuildingType>(type))
    {
        m_CursorAttachedAsset.BuildingType = std::get<BuildingType>(type);
        if (m_CursorAttachedAsset.BuildingType == BuildingType::NONE)
            m_CursorAttachedAsset.Texture.reset();
        else
        {
            m_CursorAttachedAsset.Texture = ResourceManager::GetTexture(BuildingDataMap[m_CursorAttachedAsset.BuildingType].TextureName);
            m_CursorAttachedAsset.UnitGroupType = UnitGroupType::NONE;
        }
    }
    else
    {
        LOG_ERROR("Invalid asset type variant");
    }
}
