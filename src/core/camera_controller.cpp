#include "camera_controller.h"

#include <algorithm>

#include "core/input.h"

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotate)
    : m_Rotate(rotate)
{
    m_Camera = std::make_shared<OrthographicCamera>(aspectRatio);
}

void OrthographicCameraController::OnUpdate(float dt)
{
    glm::vec3 cameraPosition = m_Camera->GetPosition();

    float cameraRotation = m_Camera->GetRotation();
    float r = glm::radians(cameraRotation);

    glm::vec2 movementVector = { 0.0f, 0.0f };
    float compensatedMovementSpeed = m_CameraMovementSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_W))
    {
        movementVector.x -= compensatedMovementSpeed * sin(r);
        movementVector.y += compensatedMovementSpeed * cos(r);
    }
    if (Input::IsKeyPressed(GLFW_KEY_S))
    {
        movementVector.x += compensatedMovementSpeed * sin(r);
        movementVector.y -= compensatedMovementSpeed * cos(r);
    }

    if (Input::IsKeyPressed(GLFW_KEY_A))
    {
        movementVector.x -= compensatedMovementSpeed * cos(r);
        movementVector.y -= compensatedMovementSpeed * sin(r);
    }
    if (Input::IsKeyPressed(GLFW_KEY_D))
    {
        movementVector.x += compensatedMovementSpeed * cos(r);
        movementVector.y += compensatedMovementSpeed * sin(r);
    }

    if (movementVector.x != 0.0f || movementVector.y != 0.0f)
    {
        cameraPosition += glm::vec3(glm::normalize(movementVector) * compensatedMovementSpeed, 0.0f);
        m_Camera->SetPosition(cameraPosition);
    }

    if (m_Rotate)
    {

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
    m_Camera->SetWindowAspectRatio();
    m_Camera->SetScale(event.GetHeight() / INITIAL_RELATIVE_HEIGHT_IN_PIXELS);
    return false;
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event)
{
    float zoom = m_Camera->GetZoom() - (event.getYOffset() / 10.0f);
    m_Camera->SetZoom(std::max(std::min(zoom, 10.0f), 0.1f));
    m_CameraMovementSpeed = m_Camera->GetZoom();
    return true;
}
