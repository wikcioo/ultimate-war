#include "graphics_context.h"

#include "core/logger.h"

GraphicsContext::GraphicsContext(GLFWwindow* glfwWindow)
    : m_GLFWwindow(glfwWindow)
{
}

void GraphicsContext::Init()
{
    glfwMakeContextCurrent(m_GLFWwindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_CRITICAL("Failed to initialize GLAD");
    }

    LOG_INFO("Graphics Info:");
    LOG_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    LOG_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    LOG_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void GraphicsContext::SwapBuffers()
{
    glfwSwapBuffers(m_GLFWwindow);
}
