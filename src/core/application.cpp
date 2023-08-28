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
}

bool Application::OnWindowClose(WindowClosedEvent& event)
{
    LOG_DEBUG("{0}", event.ToString());

    m_Running = false;
    return true;
}

void Application::Run()
{
    while (m_Running)
    {
        m_Window->OnUpdate();
    }
}
