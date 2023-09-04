#include "application.h"

#include "debug/debug_layer.h"
#include "graphics/renderer.h"

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;

    Logger::Init();
    m_Window = std::make_unique<Window>();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

    m_Camera = std::make_shared<OrthographicCamera>((float)m_Window->GetWidth() / (float)m_Window->GetHeight());

    DebugLayer* debugLayer = new DebugLayer();
    debugLayer->OnAttach();
    m_LayerStack.PushOverlay(debugLayer);

    m_Shader = std::make_shared<Shader>("assets/shaders/color.glsl");

    float vertices[6 * 3] = {
        -0.5f,   0.0f, 0.0f,
        -0.25f,  0.5f, 0.0f,
         0.25f,  0.5f, 0.0f,
         0.5f,   0.0f, 0.0f,
         0.25f, -0.5f, 0.0f,
        -0.25f, -0.5f, 0.0f
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

Application::~Application()
{
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Application::OnMouseScrolled));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
    {
        if (event.Handled)
            break;
        (*it)->OnEvent(event);
    }
}

bool Application::OnWindowClose(WindowClosedEvent& event)
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizedEvent& event)
{
    m_Camera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
    return true;
}

bool Application::OnMouseScrolled(MouseScrolledEvent& event)
{
    m_Camera->SetZoom(m_Camera->GetZoom() + (event.getYOffset() / 10.0f));
    return true;
}

void Application::Run()
{
    static float lastTime = 0.0f;
    while (m_Running)
    {
        float now = (float)glfwGetTime();
        m_DeltaTime = now - lastTime;
        lastTime = now;

        Renderer::ClearColor({0.0f, 0.5f, 1.0f, 1.0f});

        Renderer::BeginScene(m_Camera);
        Renderer::Submit(m_Shader, m_VertexArray);
        Renderer::EndScene();

        for (auto layer : m_LayerStack)
        {
            layer->OnUpdate(m_DeltaTime);
        }

        m_Window->OnUpdate();
    }
}
