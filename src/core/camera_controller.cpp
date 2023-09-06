#include "camera_controller.h"

#include <algorithm>

#include "core/core.h"
#include "core/input.h"

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotate)
    : m_Rotate(rotate)
{
    m_Camera = std::make_shared<OrthographicCamera>(aspectRatio);
}

void OrthographicCameraController::OnUpdate(float dt)
{
    glm::vec3 cameraPosition = m_Camera->GetPosition();

    if (Input::IsKeyPressed(GLFW_KEY_W))
        cameraPosition.y += m_CameraMovementSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_S))
        cameraPosition.y -= m_CameraMovementSpeed * dt;

    if (Input::IsKeyPressed(GLFW_KEY_A))
        cameraPosition.x -= m_CameraMovementSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_D))
        cameraPosition.x += m_CameraMovementSpeed * dt;

    m_Camera->SetPosition(cameraPosition);

    if (m_Rotate)
    {
        float cameraRotation = m_Camera->GetRotation();

        if (Input::IsKeyPressed(GLFW_KEY_Q))
            cameraRotation += m_CameraRotationSpeed * dt;
        if (Input::IsKeyPressed(GLFW_KEY_E))
            cameraRotation -= m_CameraRotationSpeed * dt;

        m_Camera->SetRotation(cameraRotation);
    }
}

void OrthographicCameraController::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
}

bool OrthographicCameraController::OnWindowResize(WindowResizedEvent& event)
{
    m_Camera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
    return true;
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event)
{
    float zoom = m_Camera->GetZoom() - (event.getYOffset() / 10.0f);
    m_Camera->SetZoom(std::max(std::min(zoom, 10.0f), 0.1f));
    m_CameraMovementSpeed = m_Camera->GetZoom();
    return true;
}
