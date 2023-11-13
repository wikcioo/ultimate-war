#include "minimap.h"

#include "util/util.h"
#include "core/logger.h"
#include "core/application.h"
#include "debug/debug_data.h"
#include "graphics/renderer.h"
#include "game/color_data.h"
#include "core/input.h"

Minimap::Minimap(const std::shared_ptr<OrthographicCamera>& UICamera,
                 const std::shared_ptr<OrthographicCamera>& gameCamera,
                 const std::shared_ptr<GameMapManager>& gameMapManager,
                 const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, size), m_Offset(offset),
      m_GameCamera(gameCamera), m_GameMapManager(gameMapManager), m_MinimapPos(m_Position + m_Size * 0.5f)
{
    m_MinimapCamera = std::make_shared<OrthographicCamera>(MINIMAP_ASPECT_RATIO);
    m_MinimapCamera->SetScale(1.0f);
    m_MinimapCamera->SetPosition(gameCamera->GetPosition());

    glm::vec2 mapZoom = {
        (m_GameMapManager->GetGameMap()->GetTileCountX() * (3.0f / 4.0f * TILE_WIDTH + TILE_OFFSET) + TILE_WIDTH) / 2.0f,
        (m_GameMapManager->GetGameMap()->GetTileCountY() * (TILE_HEIGHT + TILE_OFFSET) + (TILE_HEIGHT + TILE_OFFSET)) / 2.0f
    };

    float zoom = glm::max(mapZoom.x / gameCamera->GetAspectRatio(), mapZoom.y);
    m_MinimapCamera->SetZoom(zoom);

    auto pixelSize = m_GameCamera->ConvertRelativeSizeToPixel(size);
    m_Framebuffer = std::make_unique<FrameBuffer>((unsigned int)pixelSize.x, (unsigned int)pixelSize.y);

    m_MapSize = m_MinimapCamera->CalculateRelativeWindowSize();
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
    if (m_MouseWasPressed)
    {
        if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            MoveCameraToClickLocation();
        }
        else
        {
            m_MouseWasPressed = false;
        }
    }

    m_Framebuffer->Bind();

    Renderer2D::ClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    Renderer2D::BeginScene(m_MinimapCamera);
    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);
            if (tile->AssetsCanExist())
            {
                glm::vec4 color = ColorData::Get().TileColors.MiniMapColor;

                auto ownedBy = tile->GetOwnedBy();
                if (ownedBy.get())
                {
                    color = { ownedBy->GetColor(), 1.0f };
                }

                Renderer2D::DrawHexagon(tile->GetPosition(), glm::vec2(1.0f), color);
            }
            else if (tile->GetEnvironment() == TileEnvironment::OCEAN)
            {
                Renderer2D::DrawHexagon(tile->GetPosition(), glm::vec2(1.0f), glm::vec4(0.2f, 0.5f, 0.8f, 1.0f));
            }
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
    return true;
}

bool Minimap::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    m_MouseWasPressed = true;
    MoveCameraToClickLocation();
    return true;
}

void Minimap::MoveCameraToClickLocation()
{
    auto mouseClickPos = m_UICamera->CalculateRelativeMousePosition();
    auto minimapBottomLeftPos = m_UICamera->CalculateRelativeBottomLeftPosition();

    // Calculates distance in relative size from bottom left side to cursor position
    auto distance = mouseClickPos - minimapBottomLeftPos;

    // Gets proportion from -0.5 to 0.5 for where in the minimap mouse was clicked
    auto normalizedCursorPosition = glm::vec2(distance.x / m_Size.x - 0.5f, distance.y / m_Size.y - 0.5f);

    auto minimapCenter = m_MinimapCamera->GetPosition();

    // calculates final game camera position
    auto result = glm::vec2(minimapCenter.x + m_MapSize.x * normalizedCursorPosition.x,
                            minimapCenter.y + m_MapSize.y * normalizedCursorPosition.y);

    m_GameCamera->SetPosition({result, 0.0f});
}
