#pragma once

#include <glm/glm.hpp>

#include "application.h"

class Input
{
public:
    static bool IsKeyPressed(int key)
    {
        auto window = Application::Get().GetWindow()->GetNativeWindow();
        int state = glfwGetKey(window, key);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    static bool IsMouseButtonPressed(int button)
    {
        auto window = Application::Get().GetWindow()->GetNativeWindow();
        int state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    static glm::vec2 GetMousePosition()
    {
        auto window = Application::Get().GetWindow()->GetNativeWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { xpos, ypos };
    }
};
