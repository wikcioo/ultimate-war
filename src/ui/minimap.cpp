#include "minimap.h"

#include "core/core.h"
#include "core/logger.h"
#include "debug/debug_data.h"
#include "graphics/renderer.h"

Minimap::Minimap(const std::shared_ptr<OrthographicCamera>& UICamera, const std::shared_ptr<OrthographicCamera>& gameCamera,
                 const std::shared_ptr<GameMap>& gameMap, const glm::vec2& size)
    : UIElement(UICamera->CalculateRelativeBottomLeftPosition(), size), m_UICamera(UICamera),
      m_GameCamera(gameCamera), m_GameMap(gameMap), m_Size(size)
{
    m_MinimapCamera = std::make_shared<OrthographicCamera>(m_GameCamera->GetAspectRatio());
    m_MinimapCamera->SetZoom(5.0f);

    auto pixelSize = m_GameCamera->ConvertRelativeSizeToPixel(size);
    m_Framebuffer = std::make_unique<FrameBuffer>((unsigned int)pixelSize.x, (unsigned int)pixelSize.y);
}

void Minimap::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Minimap::OnMouseScrolled));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Minimap::OnMouseButtonPressed));
}

void Minimap::Draw()
{
    m_Framebuffer->Bind();

    Renderer2D::ClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    auto relTileWidthOffset = (tileOffset / 2.0f * glm::sqrt(3));
    auto relMapWidth = (m_GameMap->GetTileCountX() - 1) * (tileWidth - tileWidth / 4.0f + relTileWidthOffset);
    auto relMapHeight = m_GameMap->GetTileCountY() * (tileHeight + tileOffset) - tileHeight / 2.0f;

    m_MinimapCamera->SetPosition({relMapWidth / 2.0f, relMapHeight / 2.0f, 0.0f});

    Renderer2D::BeginScene(m_MinimapCamera);

    for (int y = 0; y < m_GameMap->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMap->GetTileCountX(); x++)
        {
            Tile* tile = m_GameMap->GetTile(x, y);
            if (tile->GetType())
                tile->DrawBase(m_GameMap->GetTileDefaultColor(tile->GetType()));
        }
    }

    Renderer2D::EndScene();

    m_Framebuffer->Unbind();

    Renderer2D::BeginScene(m_UICamera);

    glm::vec2 offset = glm::vec2(0.01f);
    float minimapPosX = m_Position.x + m_Size.x / 2 + offset.x;
    float minimapPosY = m_Position.y + m_Size.y / 2 + offset.y;

    Renderer2D::DrawQuad({minimapPosX, minimapPosY}, m_Size, m_Framebuffer->GetTexture());

    Renderer2D::EndScene();
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
