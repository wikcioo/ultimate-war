#pragma once

#include "core/logger.h"
#include "core/window.h"

class Application
{
public:
    Application();
    ~Application();

    void OnEvent(Event& event);
    void Run();

private:
    bool OnWindowClose(WindowClosedEvent& event);

private:
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
};
