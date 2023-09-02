#pragma once

#include "core/logger.h"
#include "core/window.h"
#include "layer/layer_stack.h"
#include "graphics/shader.h"

class Application
{
public:
    Application();
    ~Application();

    void OnEvent(Event& event);
    void Run();

    Window* GetWindow() { return m_Window.get(); }
    static Application& Get() { return *s_Instance; }

private:
    bool OnWindowClose(WindowClosedEvent& event);
    bool OnWindowResized(WindowResizedEvent& event);
    bool OnWindowMinimized(WindowMinimizedEvent& event);
    bool OnWindowMaximized(WindowMaximizedEvent& event);

private:
    std::unique_ptr<Window> m_Window;
    LayerStack m_LayerStack;
    float m_DeltaTime;
    bool m_Running = true;
    static Application* s_Instance;
    unsigned int m_VAO;
    std::unique_ptr<Shader> m_Shader;
};
