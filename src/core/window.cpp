#include "window.h"

#include "core/logger.h"

Window::Window(const WindowProps& props)
    : m_WindowData({props.Title, props.Width, props.Height, true})
{
    Init();
}

Window::~Window()
{
    Shutdown();
}

void Window::Init()
{
    glfwSetErrorCallback([](int code, const char* description)
    {
        LOG_ERROR("GLFW Error ({0}): {1}", code, description);
    });

    if (glfwInit() != GLFW_TRUE)
    {
        LOG_CRITICAL("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

    m_Window = glfwCreateWindow(m_WindowData.Width,
                                m_WindowData.Height,
                                m_WindowData.Title.c_str(),
                                nullptr, nullptr);

    if (m_Window == nullptr)
    {
        LOG_CRITICAL("Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, (void*)&m_WindowData);
    SetVSync(true);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_CRITICAL("Failed to initialize GLAD");
        return;
    }

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        WindowClosedEvent event;
        data.EventCallback(event);
    });

    LOG_INFO("Initialized GLFW window");
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void Window::OnUpdate()
{
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

bool Window::IsVSyncEnabled() const
{
    return m_WindowData.VSyncEnabled;
}

void Window::SetVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
    m_WindowData.VSyncEnabled = enabled;
}

void Window::SetEventCallback(std::function<void(Event&)> callback)
{
    m_WindowData.EventCallback = callback;
}
