#include "application.h"

#include "game/game.h"
#include "core/core.h"
#include "core/logger.h"
#include "debug/debug_layer.h"
#include "graphics/renderer.h"

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

    GameLayer* gameLayer = new GameLayer();
    gameLayer->OnAttach();
    m_LayerStack.PushLayer(gameLayer);
}

Application::~Application()
{
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClose));

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

void Application::Run()
{
    static float lastTime = 0.0f;
    while (m_Running)
    {
        float now = (float)glfwGetTime();
        m_DeltaTime = now - lastTime;
        lastTime = now;

        for (auto layer : m_LayerStack)
            layer->OnUpdate(m_DeltaTime);

        m_Window->OnUpdate();
    }
}
