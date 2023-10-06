#pragma once

#include "core/logger.h"
#include "core/window.h"
#include "core/camera.h"
#include "layer/layer_stack.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "graphics/vertex_array.h"

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
    void LoadResources();

private:
    static Application* s_Instance;

    std::unique_ptr<Window> m_Window;
    LayerStack m_LayerStack;
    bool m_Running = true;
    float m_DeltaTime;
};
