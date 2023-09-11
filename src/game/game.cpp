#include "game.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/tile.h"
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

    m_ColorShader = ResourceManager::GetShader("color");

    m_GameMap = std::make_unique<GameMap>("");
    m_GameMap->SetTileDefaultColor(0, {0.2f, 0.2f, 0.2f, 0.2f});
    m_GameMap->SetTileDefaultColor(1, {0.2f, 0.3f, 0.8f, 1.0f});
    m_GameMap->SetTileHighlightColor(0, {0.2f, 0.2f, 0.2f, 0.5f});
    m_GameMap->SetTileHighlightColor(1, {0.1f, 0.8f, 0.2f, 1.0f});

    m_Arrow = std::make_unique<Arrow>();
}

void GameLayer::OnAttach()
{
    float w = tileWidth;
    float h = tileHeight;

    float h2 = h / 2;
    float w2 = w / 2;
    float vertices[6 * 3] = {
        -w2,   0.0f, 0.0f,
        -w2/2,   h2, 0.0f,
         w2/2,   h2, 0.0f,
         w2,   0.0f, 0.0f,
         w2/2,  -h2, 0.0f,
        -w2/2,  -h2, 0.0f
    };

    unsigned int indices[4 * 3] = {
        0, 2, 1,
        0, 3, 2,
        0, 5, 3,
        5, 4, 3
    };

    std::vector<int> layout = {3};
    auto vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    auto indexBuffer = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
    m_VertexArray = std::make_shared<VertexArray>(vertexBuffer, indexBuffer, layout);
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    m_CameraController->OnUpdate(dt);

    Renderer2D::ClearColor({0.0f, 0.5f, 1.0f, 1.0f});

    Renderer2D::BeginScene(m_CameraController->GetCamera());

    m_ColorShader->Bind();

    auto [relX, relY] = CalculateRelativeMousePosition();
    bool isCursorInRange = false;
    for (int y = 0; y < m_GameMap->GetHeight(); y++)
    {
        for (int x = 0; x < m_GameMap->GetWidth(); x++)
        {
            Tile* tile = m_GameMap->GetTile(x, y);

            glm::vec4 tileColor;
            if (!isCursorInRange && tile->InRange({relX, relY}))
            {
                isCursorInRange = true;
                m_Arrow->SetEndPosition(tile->GetPosition());
                tileColor = m_GameMap->GetTileHighlightColor(tile->GetType());
            }
            else
            {
                tileColor = m_GameMap->GetTileDefaultColor(tile->GetType());
            }

            m_ColorShader->SetFloat4("u_Color", tileColor);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(tile->GetPosition(), 0.0f));
            Renderer2D::Submit(m_ColorShader, m_VertexArray, model);
        }
    }

    if (!isCursorInRange)
    {
        auto relativePosition = CalculateRelativeMousePosition();
        m_Arrow->SetEndPosition({relativePosition.first, relativePosition.second});
    }

    m_Arrow->Update();

    static auto starTexture = ResourceManager::GetTexture("star");
    Renderer2D::DrawQuad(m_StarPosition, glm::vec2(0.3f), starTexture);

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
    auto [relX, relY] = CalculateRelativeMousePosition();

    for (int y = 0; y < m_GameMap->GetHeight(); y++)
    {
        for (int x = 0; x < m_GameMap->GetWidth(); x++)
        {
            Tile* tile = m_GameMap->GetTile(x, y);
            if (tile->InRange({relX, relY}))
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

void GameLayer::OnDebugRender()
{
    static bool show_settings = true;
    ImGui::Begin("Settings", &show_settings);

    static bool polygon_mode = false;
    static bool polygon_mode_active_last_frame = false;
    ImGui::Checkbox("Polygon mode", &polygon_mode);
    if (polygon_mode != polygon_mode_active_last_frame)
    {
        if (polygon_mode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            polygon_mode_active_last_frame = true;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            polygon_mode_active_last_frame = false;
        }
    }

    if (ImGui::Button("Select map.."))
        ImGui::OpenPopup("available_maps_popup");

    if (ImGui::BeginPopup("available_maps_popup"))
    {
        for (std::string mapName : m_GameMap->GetAvailableMaps())
            if (ImGui::Selectable(mapName.c_str()))
                m_GameMap->Load(mapName);
        ImGui::EndPopup();
    }

    std::string selectedMap = m_GameMap->GetSelectedMapName();
    ImGui::Text(std::string("Currently selected map: " + (selectedMap.empty() ? "None" : selectedMap)).c_str());

    ImGui::Separator();

    auto camera = m_CameraController->GetCamera();
    auto pos = camera->GetPosition();
    ImGui::Text("camera");
    ImGui::Text("position: %f, %f, %f", pos.x, pos.y, pos.z);
    ImGui::Text("aspect ratio: %f", camera->GetAspectRatio());

    ImGui::Separator();

    auto window = Application::Get().GetWindow();
    float pixelWidth = (float)window->GetWidth();
    float pixelHeight = (float)window->GetHeight();
    ImGui::Text("window");
    ImGui::Text("width: %f", pixelWidth);
    ImGui::Text("height: %f", pixelHeight);

    ImGui::End();
}

std::pair<float, float> GameLayer::CalculateRelativeMousePosition()
{
    auto [x, y] = Input::GetMousePosition();

    auto window = Application::Get().GetWindow();
    float pixelWidth = (float)window->GetWidth();
    float pixelHeight = (float)window->GetHeight();

    auto camera = m_CameraController->GetCamera();
    float relWidth = camera->GetZoom() * camera->GetAspectRatio() * 2;
    float relHeight = camera->GetZoom() * 2;

    float relX = (x * relWidth / pixelWidth) - camera->GetZoom() * camera->GetAspectRatio() + camera->GetPosition().x;
    float relY = ((y * relHeight / pixelHeight) - camera->GetZoom() - camera->GetPosition().y) * -1;

    return {relX, relY};
}
