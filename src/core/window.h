#pragma once

#include <string>
#include <functional>

#include "event/event.h"
#include "event/window_event.h"
#include "event/mouse_event.h"
#include "event/key_event.h"

#include "renderer/graphics_context.h"

struct WindowProps
{
    std::string Title;
    unsigned int Width, Height;

    WindowProps(const std::string& title = "Application",
                unsigned int width = 1280,
                unsigned int height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

class Window
{
public:
    Window(const WindowProps& props = WindowProps());
    ~Window();

    void Init();
    void Shutdown();
    void OnUpdate();

    inline unsigned int GetWidth() const { return m_WindowData.Width; }
    inline unsigned int GetHeight() const { return m_WindowData.Height; }

    bool IsVSyncEnabled() const;
    void SetVSync(bool enabled);
    void SetEventCallback(std::function<void(Event&)> callback);

private:
    GLFWwindow* m_Window;
    GraphicsContext* m_GraphicsContext;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSyncEnabled;

        std::function<void(Event&)> EventCallback;
    } m_WindowData;
};
