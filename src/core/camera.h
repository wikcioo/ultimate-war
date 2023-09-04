#pragma once

#include <glm/glm.hpp>

class OrthographicCamera
{
public:
    OrthographicCamera(float aspectRatio);
    ~OrthographicCamera() = default;

    void SetPosition(const glm::vec3& position);
    void SetAspectRatio(float ratio);
    void SetRotation(float rotation);
    void SetZoom(float zoom);

    inline glm::vec3 GetPosition() const { return m_Position; }
    inline float GetAspectRatio() const { return m_AspectRatio; }
    inline float GetRotation() const { return m_Rotation; }
    inline float GetZoom() const { return m_Zoom; }

    inline glm::mat4 GetProjectionViewMatrix() const { return m_ProjectionView; }

private:
    void RecalculateProjectionMatrix();
    void RecalculateViewMatrix();
    void RecalculateProjectionViewMatrix();

private:
    glm::mat4 m_Projection;
    glm::mat4 m_View;
    glm::mat4 m_ProjectionView;

    glm::vec3 m_Position;
    float m_AspectRatio;
    float m_Rotation;
    float m_Zoom;
};