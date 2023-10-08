#include "unit_panel.h"

#include "util/util.h"
#include "core/core.h"
#include "graphics/renderer.h"
#include "game/color_data.h"
#include "core/resource_manager.h"
#include "game/game_layer.h"

#include <GLFW/glfw3.h>

UnitPanel::UnitPanel(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset,
                     const glm::vec2& unitSize, float unitOffset)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset,
      glm::vec2((int)UnitType::COUNT * (unitSize.x + unitOffset) + unitOffset, unitSize.y + unitOffset)),
      m_UnitSize(unitSize), m_UnitOffset(unitOffset), m_UnitCount((int)UnitType::COUNT)
{
}

void UnitPanel::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(UnitPanel::OnKeyPressed));

    auto mousePos = m_UICamera->CalculateRelativeMousePosition();
    if (mousePos.x > m_Position.x && mousePos.x < m_Position.x + m_Size.x &&
        mousePos.y > m_Position.y && mousePos.y < m_Position.y + m_Size.y)
    {
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(UnitPanel::OnMouseScrolled));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(UnitPanel::OnMouseButtonPressedPanel));
    }
    else
    {
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(UnitPanel::OnMouseButtonPressedGame));
    }
}

void UnitPanel::Draw()
{
    Renderer2D::BeginScene(m_UICamera);

    auto cursorPos = m_UICamera->CalculateRelativeMousePosition();
    if (IsUnitAttachedToCursor())
        Renderer2D::DrawQuad(cursorPos, m_UnitSize * 0.5f, m_CursorAttachedUnit.Texture);

    Renderer2D::DrawQuad(m_Position + m_Size * 0.5f, m_Size, ColorData::Get().UITheme.UnitPanelBackgroundColor);

    for (int i = 0; i < m_UnitCount; i++)
    {
        glm::vec2 unitPos = {
            m_Position.x + m_UnitSize.x / 2 + m_UnitOffset + (m_UnitSize.x + m_UnitOffset) * i,
            m_Position.y + m_Size.y / 2
        };

        Renderer2D::DrawQuad(unitPos, m_UnitSize, ResourceManager::GetTexture(UnitTextureMap[(UnitType)i]));

        if (Util::IsPointInRectangle(unitPos, m_UnitSize, cursorPos) || (UnitType)i == m_CursorAttachedUnit.Type)
            Renderer2D::DrawQuad(unitPos, m_UnitSize + 0.015f, ColorData::Get().UITheme.UnitPanelHighlighUnitColor, 10.0f);
    }

    Renderer2D::EndScene();
}

bool UnitPanel::OnMouseScrolled(MouseScrolledEvent& event)
{
    return true;
}

bool UnitPanel::OnKeyPressed(KeyPressedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_ESCAPE)
    {
        SetCursorAttachedUnit(UnitType::NONE);
        return true;
    }

    return false;
}

bool UnitPanel::OnMouseButtonPressedPanel(MouseButtonPressedEvent& event)
{
    auto cursorPos = m_UICamera->CalculateRelativeMousePosition();
    for (int i = 0; i < m_UnitCount; i++)
    {
        glm::vec2 unitPos = {
            m_Position.x + m_UnitSize.x / 2 + m_UnitOffset + (m_UnitSize.x + m_UnitOffset) * i,
            m_Position.y + m_Size.y / 2
        };

        if (Util::IsPointInRectangle(unitPos, m_UnitSize, cursorPos))
        {
            SetCursorAttachedUnit((UnitType)i);
            return true;
        }
    }

    return false;
}

bool UnitPanel::OnMouseButtonPressedGame(MouseButtonPressedEvent& event)
{
    if (m_CursorAttachedUnit.Type == UnitType::NONE) return false;

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
                    if (currentPlayer->GetGold() >= 10)
                    {
                        currentPlayer->AddGold(-10);
                        tile->CreateUnit(m_CursorAttachedUnit.Type);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void UnitPanel::SetCursorAttachedUnit(UnitType type)
{
    m_CursorAttachedUnit.Type = type;

    if (type == UnitType::NONE)
        m_CursorAttachedUnit.Texture.reset();
    else
        m_CursorAttachedUnit.Texture = ResourceManager::GetTexture(UnitTextureMap[type]);
}
