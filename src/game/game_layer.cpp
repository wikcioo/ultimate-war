#include "game_layer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/tile.h"
#include "debug/debug_data.h"
#include "core/core.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"

GameLayer::GameLayer()
    : Layer("GameLayer")
{
    auto window = Application::Get().GetWindow();
    m_CameraController = std::make_shared<OrthographicCameraController>((float)window->GetWidth() / (float)window->GetHeight());
    m_GameMap = std::make_shared<GameMap>("");
    m_Arrow = std::make_shared<Arrow>();
}

void GameLayer::OnAttach()
{
    m_GameMap->SetTileDefaultColor(0, {0.2f, 0.2f, 0.2f, 0.2f});
    m_GameMap->SetTileDefaultColor(1, {0.2f, 0.3f, 0.8f, 1.0f});
    m_GameMap->SetTileHighlightColor(0, {0.2f, 0.2f, 0.2f, 0.5f});
    m_GameMap->SetTileHighlightColor(1, {0.1f, 0.8f, 0.2f, 1.0f});
    m_GameMap->Load("simple");
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    m_CameraController->OnUpdate(dt);

    Renderer2D::ClearColor({0.0f, 0.5f, 1.0f, 1.0f});

    Renderer2D::BeginScene(m_CameraController->GetCamera());

    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();
    bool isCursorInRange = false;
    for (int y = 0; y < m_GameMap->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMap->GetTileCountX(); x++)
        {
            Tile* tile = m_GameMap->GetTile(x, y);

            glm::vec4 tileColor;
            if (!isCursorInRange && tile->InRange(relMousePos))
            {
                isCursorInRange = true;
                m_Arrow->SetEndPosition(tile->GetPosition());
                tileColor = m_GameMap->GetTileHighlightColor(tile->GetType());
            }
            else
            {
                tileColor = m_GameMap->GetTileDefaultColor(tile->GetType());
            }

            tile->Draw(tileColor);
        }
    }

    if (!isCursorInRange)
        m_Arrow->SetEndPosition(relMousePos);

    m_Arrow->Update();

    static auto starTexture = ResourceManager::GetTexture("star");
    Renderer2D::DrawQuad(m_StarPosition, glm::vec2(0.6f), starTexture);

    Renderer2D::EndScene();
}

void GameLayer::OnEvent(Event& event)
{
    m_CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
}

bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    static bool arrowClickedOnStarTile = false;
    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();

    for (int y = 0; y < m_GameMap->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMap->GetTileCountX(); x++)
        {
            Tile* tile = m_GameMap->GetTile(x, y);
            if (tile->InRange(relMousePos))
            {
                if (m_Arrow->IsVisible() && arrowClickedOnStarTile)
                {
                    m_StarPosition = tile->GetPosition();
                }
                else
                {
                    if (tile->GetPosition() == m_StarPosition)
                        arrowClickedOnStarTile = true;
                    else
                        arrowClickedOnStarTile = false;
                }

                m_Arrow->SetVisible(!m_Arrow->IsVisible());
                m_Arrow->SetStartPosition(tile->GetPosition());
                return false;
            }
        }
    }

    m_Arrow->SetVisible(false);
    return false;
}
