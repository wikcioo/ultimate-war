#pragma once

#include <glm/glm.hpp>

// Initial relative height is 2.0f
#define INITIAL_RELATIVE_HEIGHT_IN_PIXELS 1080.0f

class OrthographicCamera
{
public:
    OrthographicCamera(float aspectRatio);
    ~OrthographicCamera() = default;

    float ConvertPixelSizeToRelative(float size, bool xAxis = true);
    glm::vec2 ConvertPixelSizeToRelative(const glm::vec2& size);
    glm::vec2 ConvertRelativeSizeToPixel(const glm::vec2& size);
    glm::vec2 CalculateRelativeBottomLeftPosition();
    glm::vec2 CalculateRelativeMousePosition();
    glm::vec2 CalculateScreenRelativeMousePosition();
    glm::vec2 CalculateRelativeWindowSize();

    void SetPosition(const glm::vec3& position);
    void SetAspectRatio(float ratio);
    void SetRotation(float rotation);
    void SetZoom(float zoom);
    void SetScale(float scale);

    inline glm::vec3 GetPosition() const { return m_Position; }
    inline float GetAspectRatio() const { return m_AspectRatio; }
    inline float GetRotation() const { return m_Rotation; }
    inline float GetZoom() const { return m_Zoom; }

    float GetHalfOfRelativeHeight();
    float GetHalfOfRelativeWidth();

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
    float m_Scale;
};
