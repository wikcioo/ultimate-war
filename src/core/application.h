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
    bool OnWindowResize(WindowResizedEvent& event);
    bool OnMouseScrolled(MouseScrolledEvent& event);

private:
    std::unique_ptr<Window> m_Window;
    LayerStack m_LayerStack;
    float m_DeltaTime;
    bool m_Running = true;
    static Application* s_Instance;
    unsigned int m_VAO;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<OrthographicCamera> m_Camera;
};
