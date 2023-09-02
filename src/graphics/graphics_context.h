#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GraphicsContext
{
public:
    GraphicsContext(GLFWwindow* glfwWindow);
    ~GraphicsContext() = default;

    void Init();
    void SwapBuffers();

private:
    GLFWwindow* m_GLFWwindow;
};
