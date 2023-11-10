#pragma once

#include "core/logger.h"
#include "core/window.h"
#include "core/camera.h"
#include "layer/layer_stack.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "graphics/vertex_array.h"

#include "ui/ui_layer.h"
#include "game/game_layer.h"
#include "debug/debug_layer.h"

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
    void InitializeColors();

private:
    static Application* s_Instance;

    std::shared_ptr<GameLayer> m_GameLayer;
    std::shared_ptr<GameLayer> m_LastGameLayer;
    std::shared_ptr<UILayer> m_UILayer;
#if defined(DEBUG)
    std::shared_ptr<DebugLayer> m_DebugLayer;
#endif
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<LayerStack> m_LayerStack;
    bool m_Running = true;
    float m_DeltaTime;
};
