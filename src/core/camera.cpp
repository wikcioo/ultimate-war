#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "core/input.h"
#include "core/logger.h"
#include "core/application.h"

OrthographicCamera::OrthographicCamera(float aspectRatio)
    : m_Projection(1.0f), m_View(1.0f), m_Position(0.0f), m_AspectRatio(aspectRatio), m_Rotation(0.0f), m_Zoom(1.0f)
{
    RecalculateProjectionMatrix();
    RecalculateViewMatrix();
}

glm::vec2 OrthographicCamera::ConvertRelativeSizeToPixel(const glm::vec2& size)
{
    float relWidth = m_Zoom * m_AspectRatio;
    float relHeight = m_Zoom;

    static auto window = Application::Get().GetWindow();
    float pixelSizeX = window->GetWidth() / (relWidth * 2) * size.x;
    float pixelSizeY = window->GetHeight() / (relHeight * 2) * size.y;

    return { pixelSizeX, pixelSizeY };
}

float OrthographicCamera::ConvertPixelSizeToRelative(float size, bool xAxis)
{
    auto result = ConvertPixelSizeToRelative({size, size});
    return xAxis ? result.x : result.y;
}

glm::vec2 OrthographicCamera::ConvertPixelSizeToRelative(const glm::vec2& size)
{
    float relWidth = m_Zoom * m_AspectRatio;
    float relHeight = m_Zoom;

    static auto window = Application::Get().GetWindow();
    float relSizeX = size.x * relWidth / window->GetWidth();
    float relSizeY = size.y * relHeight / window->GetHeight();

    return { relSizeX, relSizeY };
}

glm::vec2 OrthographicCamera::CalculateRelativeBottomLeftPosition()
{
    float relWidth = m_Zoom * m_AspectRatio;
    float relHeight = m_Zoom;
    return { m_Position.x - relWidth, m_Position.y - relHeight };
}

glm::vec2 OrthographicCamera::CalculateRelativeMousePosition()
{
    auto mousePos = Input::GetMousePosition();

    static auto window = Application::Get().GetWindow();
    float pixelWidth = (float)window->GetWidth();
    float pixelHeight = (float)window->GetHeight();

    auto relWindowSize = CalculateRelativeWindowSize();
    float relX = (mousePos.x * relWindowSize.x / pixelWidth) - m_Zoom * m_AspectRatio + m_Position.x;
    float relY = ((mousePos.y * relWindowSize.y / pixelHeight) - m_Zoom - m_Position.y) * -1;

    glm::vec2 offset = { relX - m_Position.x, relY - m_Position.y };
    float r = glm::radians(m_Rotation);
    float rotatedX = offset.x * glm::cos(r) - offset.y * glm::sin(r) + m_Position.x;
    float rotatedY = offset.x * glm::sin(r) + offset.y * glm::cos(r) + m_Position.y;
    return { rotatedX, rotatedY };
}

glm::vec2 OrthographicCamera::CalculateScreenRelativeMousePosition()
{
    auto mousePos = Input::GetMousePosition();

    static auto window = Application::Get().GetWindow();
    float pixelWidth = (float)window->GetWidth();
    float pixelHeight = (float)window->GetHeight();

    auto relWindowSize = CalculateRelativeWindowSize();
    float relX = (mousePos.x * relWindowSize.x / pixelWidth) - m_Zoom * m_AspectRatio;
    float relY = ((mousePos.y * relWindowSize.y / pixelHeight) - m_Zoom) * -1;

    return { relX, relY };
}

glm::vec2 OrthographicCamera::CalculateRelativeWindowSize()
{
    return { m_Zoom * m_AspectRatio * 2, m_Zoom * 2 };
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
    RecalculateViewMatrix();
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
