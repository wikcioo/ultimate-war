#include "application.h"

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

Application::Application()
{
    Logger::Init();
    m_Window = std::make_unique<Window>();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
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
    while (m_Running)
    {
        m_Window->OnUpdate();
    }
}
