#include "shop_panel.h"

#include "util/util.h"
#include "core/core.h"
#include "core/logger.h"
#include "graphics/renderer.h"
#include "game/color_data.h"
#include "core/resource_manager.h"
#include "game/game_layer.h"

#include <GLFW/glfw3.h>

ShopPanel::ShopPanel(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset,
                     const glm::vec2& assetSize, float assetOffset)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset,
      glm::vec2(5 * (assetSize.x + assetOffset) + assetOffset, assetSize.y + assetOffset)),
      m_AssetSize(assetSize), m_AssetOffset(assetOffset), m_Offset(offset),
      m_UnitGroupCount((int)UnitGroupType::COUNT), m_BuildingCount((int)BuildingType::COUNT),
      m_AssetBorderMargin(0.015f), m_AssetBorderThickness(10.0f), m_AssetPriceSize(0.125f), m_AssetPriceFontName("rexlia")
{
}

void ShopPanel::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ShopPanel::OnKeyPressed));
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(ShopPanel::OnWindowResized));

    auto mousePos = m_UICamera->CalculateRelativeMousePosition();
    if (mousePos.x > m_Position.x && mousePos.x < m_Position.x + m_Size.x &&
        mousePos.y > m_Position.y && mousePos.y < m_Position.y + m_Size.y * 2)
    {
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(ShopPanel::OnMouseScrolled));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ShopPanel::OnMouseButtonPressedPanel));
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

    DrawBuildings(cursorPos);
    DrawUnitGroups(cursorPos);

    if (IsAssetAttachedToCursor())
        Renderer2D::DrawQuad(cursorPos, m_AssetSize * 0.5f, m_CursorAttachedAsset.Texture);

    Renderer2D::EndScene();
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

            if (!IsAssetAttachedToCursor())
            {
                Renderer2D::DrawTextStr(
                    GetCostText(unitData.Cost),
                    { cursorPos.x - m_AssetPriceSize, cursorPos.y + m_AssetPriceSize / 1.5f },
                    m_AssetPriceSize,
                    glm::vec3(1.0f),
                    TextAlignment::LEFT,
                    m_AssetPriceFontName
                );
            }
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

            if (!IsAssetAttachedToCursor())
            {
                Renderer2D::DrawTextStr(
                    GetCostText(buildingData.Cost),
                    { cursorPos.x - m_AssetPriceSize, cursorPos.y + m_AssetPriceSize / 1.5f },
                    m_AssetPriceSize,
                    glm::vec3(1.0f),
                    TextAlignment::LEFT,
                    m_AssetPriceFontName
                );
            }
        }
    }
}

std::string ShopPanel::GetCostText(Resources& cost)
{
    std::ostringstream oss;
    oss << cost.Wood << " Wood\n" << cost.Rock << " Rock\n" << cost.Steel << " Steel\n" << cost.Gold << " Gold";
    return oss.str();
}

bool ShopPanel::OnWindowResized(WindowResizedEvent& event)
{
    m_Position = m_UICamera->CalculateRelativeBottomLeftPosition() + m_Offset;
    return false;
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
                        currentPlayer->SubtractResources(UnitGroupDataMap[m_CursorAttachedAsset.UnitGroupType].Cost))
                    {
                        tile->CreateUnitGroup(m_CursorAttachedAsset.UnitGroupType);
                        return true;
                    }
                    else if (m_CursorAttachedAsset.BuildingType != BuildingType::NONE &&
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
