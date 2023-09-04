#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "logger.h"

OrthographicCamera::OrthographicCamera(float aspectRatio)
    : m_Projection(1.0f), m_View(1.0f), m_Position(0.0f), m_AspectRatio(aspectRatio), m_Rotation(0.0f), m_Zoom(1.0f)
{
    RecalculateProjectionMatrix();
    RecalculateViewMatrix();
}

void OrthographicCamera::SetAspectRatio(float ratio)
{
    m_AspectRatio = ratio;
    RecalculateProjectionMatrix();
}

void OrthographicCamera::SetZoom(float zoom)
{
    m_Zoom = zoom;
    RecalculateProjectionMatrix();
}

void OrthographicCamera::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    RecalculateProjectionViewMatrix();
}

void OrthographicCamera::SetRotation(float rotation)
{
    m_Rotation = rotation;
    RecalculateViewMatrix();
}

void OrthographicCamera::RecalculateProjectionMatrix()
{
    float left   = m_Zoom * m_AspectRatio * -1;
    float right  = m_Zoom * m_AspectRatio;
    float bottom = m_Zoom * -1;
    float top    = m_Zoom;

    m_Projection = glm::ortho(left, right, bottom, top, 0.0f, 1.0f);
    RecalculateProjectionViewMatrix();
}

void OrthographicCamera::RecalculateViewMatrix()
{
    m_View = glm::translate(glm::mat4(1.0f), m_Position);
    m_View = glm::rotate(m_View, glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    m_View = glm::inverse(m_View);
    RecalculateProjectionViewMatrix();
}

void OrthographicCamera::RecalculateProjectionViewMatrix()
{
    m_ProjectionView = m_Projection * m_View;
}
