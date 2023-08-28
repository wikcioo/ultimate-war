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
    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnKeyReleased(KeyReleasedEvent& event);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
    bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
    bool OnMouseMoved(MouseMovedEvent& event);
    bool OnMouseScrolled(MouseScrolledEvent& event);
    bool OnWindowClose(WindowClosedEvent& event);
    bool OnWindowResized(WindowResizedEvent& event);
    bool OnWindowMinimized(WindowMinimizedEvent& event);
    bool OnWindowMaximized(WindowMaximizedEvent& event);

private:
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
};
