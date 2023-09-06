#include "game.h"

#include "core/core.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"

GameLayer::GameLayer()
    : Layer("GameLayer")
{
    auto window = Application::Get().GetWindow();
    m_Camera = std::make_shared<OrthographicCamera>((float)window->GetWidth() / (float)window->GetHeight());

    m_ColorShader = ResourceManager::GetShader("color");
    m_TextureShader = ResourceManager::GetShader("texture");
    m_StarTexture = ResourceManager::GetTexture("star");
}

void GameLayer::OnAttach()
{
    float h = (0.5 * glm::sqrt(3)) / 2;

    float vertices[6 * 3] = {
        -0.5f,   0.0f, 0.0f,
        -0.25f,     h, 0.0f,
         0.25f,     h, 0.0f,
         0.5f,   0.0f, 0.0f,
         0.25f,    -h, 0.0f,
        -0.25f,    -h, 0.0f
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
    Renderer::ClearColor({0.0f, 0.5f, 1.0f, 1.0f});

    glm::vec3 cameraPosition = m_Camera->GetPosition();
    float cameraRotation = m_Camera->GetRotation();

    if (Input::IsKeyPressed(GLFW_KEY_UP))
        cameraPosition.y += m_CameraMovementSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_DOWN))
        cameraPosition.y -= m_CameraMovementSpeed * dt;

    if (Input::IsKeyPressed(GLFW_KEY_LEFT))
        cameraPosition.x -= m_CameraMovementSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
        cameraPosition.x += m_CameraMovementSpeed * dt;

    if (Input::IsKeyPressed(GLFW_KEY_Q))
        cameraRotation += m_CameraRotationSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_E))
        cameraRotation -= m_CameraRotationSpeed * dt;

    m_Camera->SetPosition(cameraPosition);
    m_Camera->SetRotation(cameraRotation);


    Renderer::BeginScene(m_Camera);

    m_ColorShader->Bind();
    Renderer::Submit(m_ColorShader, m_VertexArray);

    m_StarTexture->Bind(0);
    m_TextureShader->Bind();
    Renderer::Submit(m_TextureShader, m_QuadVA);

    Renderer::EndScene();
}

void GameLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(GameLayer::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(GameLayer::OnMouseScrolled));
}

bool GameLayer::OnWindowResize(WindowResizedEvent& event)
{
    m_Camera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
    return true;
}

bool GameLayer::OnMouseScrolled(MouseScrolledEvent& event)
{
    m_Camera->SetZoom(m_Camera->GetZoom() - (event.getYOffset() / 10.0f));
    return true;
}
