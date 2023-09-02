#include "application.h"

#include "debug/debug_layer.h"

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;

    Logger::Init();
    m_Window = std::make_unique<Window>();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

    DebugLayer* debugLayer = new DebugLayer();
    debugLayer->OnAttach();
    m_LayerStack.PushOverlay(debugLayer);

    m_Shader = std::make_unique<Shader>("assets/shaders/color.glsl");

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

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

Application::~Application()
{
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowResized));
    dispatcher.Dispatch<WindowMinimizedEvent>(BIND_EVENT_FN(Application::OnWindowMinimized));
    dispatcher.Dispatch<WindowMaximizedEvent>(BIND_EVENT_FN(Application::OnWindowMaximized));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
    {
        if (event.Handled)
            break;
        (*it)->OnEvent(event);
    }
}

bool Application::OnWindowClose(WindowClosedEvent& event)
{
    LOG_DEBUG("{0}", event.ToString());

    m_Running = false;
    return true;
}

bool Application::OnWindowResized(WindowResizedEvent& event)
{
    LOG_DEBUG("{0}", event.ToString());
    return true;
}

bool Application::OnWindowMinimized(WindowMinimizedEvent& event)
{
    LOG_DEBUG("{0}", event.ToString());
    return true;
}

bool Application::OnWindowMaximized(WindowMaximizedEvent& event)
{
    LOG_DEBUG("{0}", event.ToString());
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

        glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->Bind();
        m_Shader->SetFloat4("u_Color", {0.2f, 0.7f, 0.7f, 1.0f});

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, 0);

        for (auto layer : m_LayerStack)
        {
            layer->OnUpdate(m_DeltaTime);
        }

        m_Window->OnUpdate();
    }
}
