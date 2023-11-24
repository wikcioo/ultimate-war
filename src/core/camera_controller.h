#pragma once

#include <memory>

#include "core/camera.h"
#include "event/event.h"
#include "event/mouse_event.h"
#include "event/key_event.h"
#include "event/window_event.h"

class OrthographicCameraController
{
public:
    OrthographicCameraController(float aspectRatio, bool rotate = false);
    ~OrthographicCameraController() = default;

    void OnUpdate(float dt);
    void OnEvent(Event& event);

    inline const std::shared_ptr<OrthographicCamera>& GetCamera() const { return m_Camera; }

    void SetMovementSpeed(float speed) { m_CameraMovementSpeed = speed; }
    void ResetKeys();
    bool IsKeyPressed(unsigned int key) { return m_Keys[key]; }

private:
    bool OnWindowResize(WindowResizedEvent& event);
    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnKeyReleased(KeyReleasedEvent& event);
    bool OnMouseScrolled(MouseScrolledEvent& event);

private:
    std::shared_ptr<OrthographicCamera> m_Camera;
    float m_CameraMovementSpeed = 1.0f;
    float m_CameraRotationSpeed = 90.0f;
    bool m_Keys[1024] = {0};
    bool m_Rotate;
};
