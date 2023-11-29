#include "camera_controller.h"

#include <algorithm>

#include "core/input.h"

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotate)
    : m_Rotate(rotate), m_IsMousePanning(false)
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
    if (m_Keys[GLFW_KEY_W])
    {
        movementVector.x -= compensatedMovementSpeed * sin(r);
        movementVector.y += compensatedMovementSpeed * cos(r);
    }
    if (m_Keys[GLFW_KEY_S])
    {
        movementVector.x += compensatedMovementSpeed * sin(r);
        movementVector.y -= compensatedMovementSpeed * cos(r);
    }

    if (m_Keys[GLFW_KEY_A])
    {
        movementVector.x -= compensatedMovementSpeed * cos(r);
        movementVector.y -= compensatedMovementSpeed * sin(r);
    }
    if (m_Keys[GLFW_KEY_D])
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

        if (m_Keys[GLFW_KEY_Q])
            cameraRotation += m_CameraRotationSpeed * dt;
        if (m_Keys[GLFW_KEY_E])
            cameraRotation -= m_CameraRotationSpeed * dt;

        m_Camera->SetRotation(cameraRotation);
    }

    if (m_IsMousePanning)
    {
        auto position = cameraPosition + glm::vec3(m_PanningStartRelMousePos - m_Camera->CalculateRelativeMousePosition(), 0.0f);
        m_Camera->SetPosition(position);
    }
}

void OrthographicCameraController::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnKeyReleased));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseButtonReleased));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
}

bool OrthographicCameraController::OnWindowResize(WindowResizedEvent& event)
{
    m_Camera->SetWindowAspectRatio();
    m_Camera->SetScale(event.GetHeight() / INITIAL_RELATIVE_HEIGHT_IN_PIXELS);
    return false;
}

bool OrthographicCameraController::OnKeyPressed(KeyPressedEvent& event)
{
    m_Keys[event.GetKeyCode()] = true;
    return false;
}

bool OrthographicCameraController::OnKeyReleased(KeyReleasedEvent& event)
{
    m_Keys[event.GetKeyCode()] = false;
    return false;
}

bool OrthographicCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    switch (event.GetMouseButton())
    {
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            m_IsMousePanning = true;
            m_PanningStartRelMousePos = m_Camera->CalculateRelativeMousePosition();
            return true;
        }
        default:
            return false;
    }

    return false;
}

bool OrthographicCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& event)
{
    switch (event.GetMouseButton())
    {
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            m_IsMousePanning = false;
            return true;
        }
        default:
            return false;
    }
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event)
{
    float zoom = m_Camera->GetZoom() - (event.getYOffset() / 10.0f);
    m_Camera->SetZoom(std::max(std::min(zoom, 100.0f), 0.1f));
    m_CameraMovementSpeed = m_Camera->GetZoom();
    return true;
}

void OrthographicCameraController::ResetKeys()
{
    memset(m_Keys, 0, sizeof(m_Keys));
}
