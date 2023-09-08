#include "game.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/core.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"

float h = (0.5 * glm::sqrt(3));
float w = 1.0f;

GameLayer::GameLayer()
    : Layer("GameLayer"), m_TileColor({0.1f, 0.8f, 0.8f})
{
    auto window = Application::Get().GetWindow();
    m_CameraController = std::make_shared<OrthographicCameraController>((float)window->GetWidth() / (float)window->GetHeight());

    m_ColorShader = ResourceManager::GetShader("color");
    m_TextureShader = ResourceManager::GetShader("texture");
    m_StarTexture = ResourceManager::GetTexture("star");

    m_GameMap = std::make_unique<GameMap>("");
}

void GameLayer::OnAttach()
{
    float h2 = h / 2;
    float vertices[6 * 3] = {
        -0.5f,   0.0f, 0.0f,
        -0.25f,    h2, 0.0f,
         0.25f,    h2, 0.0f,
         0.5f,   0.0f, 0.0f,
         0.25f,   -h2, 0.0f,
        -0.25f,   -h2, 0.0f
    };

    unsigned int indices[4 * 3] = {
        0, 2, 1,
        0, 3, 2,
        0, 5, 3,
        5, 4, 3
    };

    float quadVertices[4 * 5] = {
        -0.3f, -0.3f, 0.0f, 0.0f, 0.0f,
         0.3f, -0.3f, 0.0f, 1.0f, 0.0f,
         0.3f,  0.3f, 0.0f, 1.0f, 1.0f,
        -0.3f,  0.3f, 0.0f, 0.0f, 1.0f
    };

    unsigned int quadIndices[6] = {
        0, 2, 3,
        0, 1, 2
    };

    auto vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    auto indexBuffer = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
    std::vector<int> layout = {3};

    auto quadVB = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
    auto quadIB = std::make_shared<IndexBuffer>(quadIndices, sizeof(quadIndices) / sizeof(unsigned int));
    std::vector<int> quadLayout = {3, 2};

    m_VertexArray = std::make_shared<VertexArray>(vertexBuffer, indexBuffer, layout);
    m_QuadVA = std::make_shared<VertexArray>(quadVB, quadIB, quadLayout);
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    m_CameraController->OnUpdate(dt);

    Renderer::ClearColor({0.0f, 0.5f, 1.0f, 1.0f});

    Renderer::BeginScene(m_CameraController->GetCamera());

    m_ColorShader->Bind();

    float offset = 0.1f;
    int height = m_GameMap->GetHeight();
    int width = m_GameMap->GetWidth();
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (!m_GameMap->GetTile(x, y)) continue;

            float dx = (w-(w/4)) * x + (x * offset);
            float dy = (h * y) + (y * offset);
            if (x & 1)
            {
                dy += (h + offset) / 2;
            }
            m_ColorShader->SetFloat4("u_Color", glm::vec4(m_TileColor, 1.0f));
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0.0f));
            Renderer::Submit(m_ColorShader, m_VertexArray, model);
        }
    }

    m_StarTexture->Bind(0);
    m_TextureShader->Bind();
    m_TextureShader->SetInt("u_Texture", 0);
    Renderer::Submit(m_TextureShader, m_QuadVA, glm::mat4(1.0f));

    Renderer::EndScene();
}

void GameLayer::OnEvent(Event& event)
{
    m_CameraController->OnEvent(event);
}

void GameLayer::OnDebugRender()
{
    static bool show_settings = true;
    ImGui::Begin("Settings", &show_settings);

    ImGui::ColorEdit3("tile color", glm::value_ptr(m_TileColor));

    ImGui::Separator();

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

    ImGui::End();
}
