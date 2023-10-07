#include "minimap.h"

#include "util/util.h"
#include "core/core.h"
#include "core/logger.h"
#include "core/application.h"
#include "debug/debug_data.h"
#include "graphics/renderer.h"
#include "game/color_data.h"

Minimap::Minimap(const std::shared_ptr<OrthographicCamera>& UICamera,
            const std::shared_ptr<OrthographicCamera>& gameCamera,
            const std::shared_ptr<GameMapManager>& gameMapManager,
            const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * gameCamera->GetAspectRatio(), size.y}), m_Offset(offset),
      m_UICamera(UICamera), m_GameCamera(gameCamera), m_GameMapManager(gameMapManager), m_MinimapPos(m_Position + m_Size * 0.5f)
{
    m_MinimapCamera = std::make_shared<OrthographicCamera>(m_GameCamera->GetAspectRatio());
    m_MinimapCamera->SetZoom(5.0f);

    auto pixelSize = m_GameCamera->ConvertRelativeSizeToPixel(size);
    m_Framebuffer = std::make_unique<FrameBuffer>((unsigned int)pixelSize.x, (unsigned int)pixelSize.y);
}

void Minimap::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Minimap::OnWindowResized));

    auto mousePos = m_UICamera->CalculateScreenRelativeMousePosition();
    if (mousePos.x > m_Position.x && mousePos.x < m_Position.x + m_Size.x &&
        mousePos.y > m_Position.y && mousePos.y < m_Position.y + m_Size.y)
    {
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Minimap::OnMouseScrolled));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Minimap::OnMouseButtonPressed));
    }
}

void Minimap::Draw()
{
    m_Framebuffer->Bind();

    Renderer2D::ClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    m_MinimapCamera->SetPosition(m_GameCamera->GetPosition());
#if defined(DEBUG)
    m_MinimapCamera->SetZoom(m_GameCamera->GetZoom() * DebugData::Get()->MinimapData.Zoom);
#else
    m_MinimapCamera->SetZoom(m_GameCamera->GetZoom() * 3.0f);
#endif

    Renderer2D::BeginScene(m_MinimapCamera);

    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);
            if (tile->GetType())
                tile->DrawBase(ColorData::Get()->TileColors.MiniMapColor);
        }
    }

#if defined(DEBUG)
    Renderer2D::DrawQuad(m_GameCamera->GetPosition(), m_GameCamera->CalculateRelativeWindowSize(), glm::vec4(1.0f), DebugData::Get()->MinimapData.BorderThickness);
#else
    Renderer2D::DrawQuad(m_GameCamera->GetPosition(), m_GameCamera->CalculateRelativeWindowSize(), glm::vec4(1.0f), 5.0);
#endif

    Renderer2D::EndScene();

    m_Framebuffer->PostProcess();
    m_Framebuffer->Unbind();

    Renderer2D::BeginScene(m_UICamera);

    Renderer2D::DrawQuad(m_MinimapPos, m_Size, m_Framebuffer->GetTexture());

#if defined(DEBUG)
    Renderer2D::DrawTextStr(DebugData::Get()->Font.Text, DebugData::Get()->Font.Pos, DebugData::Get()->Font.Scale,
                           DebugData::Get()->Font.Color, DebugData::Get()->Font.Alignment, DebugData::Get()->Font.FontName);
#endif

    Renderer2D::EndScene();
}

bool Minimap::OnWindowResized(WindowResizedEvent& event)
{
    m_Position = m_UICamera->CalculateRelativeBottomLeftPosition() + m_Offset;
    m_MinimapPos = m_Position + m_Size * 0.5f;
    return false;
}

bool Minimap::OnMouseMoved(MouseMovedEvent& event)
{
    return true;
}

bool Minimap::OnMouseScrolled(MouseScrolledEvent& event)
{
    float zoom = m_MinimapCamera->GetZoom() - (event.getYOffset() / 5.0f);
    m_MinimapCamera->SetZoom(std::max(std::min(zoom, 15.0f), 0.1f));
    return true;
}

bool Minimap::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    return true;
}
