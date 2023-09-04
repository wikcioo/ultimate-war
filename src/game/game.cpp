#include "game.h"

#include "core/core.h"
#include "core/input.h"
#include "core/logger.h"
#include "graphics/renderer.h"

GameLayer::GameLayer()
    : Layer("GameLayer")
{
    auto window = Application::Get().GetWindow();
    m_Camera = std::make_shared<OrthographicCamera>((float)window->GetWidth() / (float)window->GetHeight());
    m_Shader = std::make_shared<Shader>("assets/shaders/color.glsl");
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

    m_VertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    m_IndexBuffer = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
    std::vector<int> layout = {3};

    m_VertexArray = std::make_shared<VertexArray>(m_VertexBuffer, m_IndexBuffer, layout);
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
    Renderer::Submit(m_Shader, m_VertexArray);
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
