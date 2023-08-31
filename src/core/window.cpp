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

    LOG_INFO("Initialized GLFW window");

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, (void*)&m_WindowData);
    SetVSync(true);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_CRITICAL("Failed to initialize GLAD");
        return;
    }

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS)
        {
            KeyPressedEvent event(key, 0);
            data.EventCallback(event);
        }
        else if (action == GLFW_RELEASE)
        {
            KeyReleasedEvent event(key);
            data.EventCallback(event);
        }
        else if(action == GLFW_REPEAT)
        {
            KeyPressedEvent event(key, 1);
            data.EventCallback(event);
        }
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS)
        {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
        }
        else if (action == GLFW_RELEASE)
        {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event((float)xpos,(float)ypos);
        data.EventCallback(event);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseScrolledEvent event((float)xoffset, (float)yoffset);
        data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        WindowClosedEvent event;
        data.EventCallback(event);
    });

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        data.Width = width;
        data.Height = height;

        WindowResizedEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
    {
        auto data = *(WindowData*)glfwGetWindowUserPointer(window);

        if (maximized == GLFW_TRUE)
        {
            WindowMaximizedEvent event;
            data.EventCallback(event);
        }
        else if (maximized == GLFW_FALSE)
        {
            WindowMinimizedEvent event;
            data.EventCallback(event);
        }
    });
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void Window::OnUpdate()
{
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
