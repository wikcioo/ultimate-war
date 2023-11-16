#include "button.h"

#include "core/logger.h"
#include "util/util.h"
#include "graphics/renderer.h"

Button::Button(const std::shared_ptr<OrthographicCamera>& camera, ButtonConfig config)
    : m_Camera(camera), m_Id(GetNextId()), m_Text(config.Text), m_Position(config.Position), m_Size(config.Size),
      m_TextColor(config.TextColor), m_BackgroundColor(config.BackgroundColor), m_FontName(config.FontName),
      m_Toggled(false), m_Disabled(false)
{
    glm::vec2 textSize = Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName);
    m_TextScale = (m_Size.y * 0.45f) / textSize.y;
}

void Button::OnUpdate()
{
    glm::vec3 bgColor = m_BackgroundColor;
    glm::vec3 textColor = m_TextColor;

    if (m_Disabled)
    {
        bgColor = glm::vec3(0.4f);
        textColor = glm::vec3(0.6f);
    }
    else
    {
        bool hoveredOver = false;
        if (Util::IsPointInRectangle(m_Position, m_Size, m_Camera->CalculateRelativeMousePosition()))
            hoveredOver = true;

        if (hoveredOver || m_Toggled)
            bgColor *= 0.8f;
    }

    Renderer2D::DrawQuad(m_Position, m_Size, glm::vec4(bgColor, 1.0f));
    Renderer2D::DrawTextStr(m_Text, m_Position, m_TextScale, textColor, HTextAlign::MIDDLE, VTextAlign::MIDDLE, m_FontName);
}

void Button::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Button::OnMouseButtonPressed));
}

bool Button::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    if (m_Disabled) return false;

    auto relMousePosition = m_Camera->CalculateRelativeMousePosition();
    if (Util::IsPointInRectangle(m_Position, m_Size, relMousePosition))
    {
        if (m_PressedCallback)
        {
            m_PressedCallback({ m_Id, m_Text });
            return true;
        }
        else
        {
            LOG_WARN("Button: PressedCallback not set for '{0}'", m_Text);
        }
    }

    return false;
}

unsigned int Button::GetNextId()
{
    static unsigned int counter = 0;
    return counter++;
}
