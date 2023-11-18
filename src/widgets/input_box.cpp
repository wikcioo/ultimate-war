#include "input_box.h"

#include "core/input.h"
#include "util/util.h"
#include "graphics/renderer.h"

#include <GLFW/glfw3.h>

InputBox::InputBox(const std::shared_ptr<OrthographicCamera>& camera, InputBoxConfig config)
    : m_Camera(camera), m_Id(GetNextId()), m_Text(""), m_Position(config.Position), m_Size(config.Size),
      m_TextColor(config.TextColor), m_BackgroundColor(config.BackgroundColor), m_BorderColor(config.BorderColor),
      m_FontName(config.FontName), m_TextLength(0.0f), m_TextHOffset(0.02f), m_CharacterLimit(-1)
{
    glm::vec2 textSize = Renderer2D::GetTextSize(m_Camera, "A", m_FontName);
    m_TextScale = (m_Size.y * 0.45f) / textSize.y;
}

void InputBox::OnUpdate()
{
    // Background
    Renderer2D::DrawQuad(m_Position, m_Size, glm::vec4(m_BackgroundColor, 1.0f));

    if (m_Focused)
    {
        // Border
        Renderer2D::DrawQuad(m_Position, m_Size, glm::vec4(m_BorderColor, 1.0f), 5.0f);
    }

    if (!m_Text.empty())
    {
        HTextAlign hAlignment = HTextAlign::LEFT;
        glm::vec2 position = { m_Position.x - m_Size.x / 2.0f + m_TextHOffset, m_Position.y };
        bool tooLong = false;

        if (Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale + m_TextHOffset > m_Size.x - m_TextHOffset)
        {
            // NOTE: if text is longer than input box width, utilize stencil buffer to only display
            // what can fit within the input box size adjusted for horizontal offset
            tooLong = true;
            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            Renderer2D::DrawQuad(m_Position, { m_Size.x - m_TextHOffset * 2.0f, m_Size.y }, glm::vec4(0.0f));

            hAlignment = HTextAlign::RIGHT;
            position = { m_Position.x + m_Size.x / 2.0f - m_TextHOffset, m_Position.y };
        }

        if (tooLong)
        {
            glStencilFunc(GL_EQUAL, 1, 0xFF);
            glStencilMask(0x00);
        }

        // Text inside of input box
        Renderer2D::DrawTextStr(
            m_Text,
            position,
            m_TextScale,
            m_TextColor,
            hAlignment,
            VTextAlign::MIDDLE,
            "rexlia"
        );

        if (tooLong)
        {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
        }
    }

    if (m_Focused)
    {
        // Cursor
        Renderer2D::DrawQuad(
            { m_Position.x - m_Size.x / 2.0f + m_TextHOffset + m_TextLength, m_Position.y },
            { 0.005f, m_Size.y * 0.7f },
            glm::vec4(m_BorderColor, 1.0f)
        );
    }
}

void InputBox::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(InputBox::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(InputBox::OnMouseButtonPressed));
}

void InputBox::SetText(const std::string& text)
{
    m_Text = text;

    if (Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale + m_TextHOffset <= m_Size.x - m_TextHOffset)
        m_TextLength = Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale;
    else
        m_TextLength = m_Size.x - m_TextHOffset * 2.0f;
}

void InputBox::Clear()
{
    m_Text.clear();
    m_TextLength = 0.0f;
}

bool InputBox::OnKeyPressed(KeyPressedEvent& event)
{
    if (!m_Focused) return false;

    if (event.GetKeyCode() == GLFW_KEY_ENTER)
    {
        if (m_AcceptedCallback)
            m_AcceptedCallback({ m_Id, m_Text });

        m_Focused = false;
        return true;
    }

    if (event.GetKeyCode() == GLFW_KEY_BACKSPACE && !m_Text.empty())
    {
        m_Text.pop_back();

        if (Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale + m_TextHOffset <= m_Size.x - m_TextHOffset)
            m_TextLength = Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale;
        else
            m_TextLength = m_Size.x - m_TextHOffset * 2.0f;

        if (m_KeyChangedCallback)
            m_KeyChangedCallback({ m_Id, m_Text });
    }
    else
    {
        if (m_Text.size() >= m_CharacterLimit) return false;

        int keyCode = event.GetKeyCode();

        if (keyCode >= GLFW_KEY_SPACE && keyCode <= GLFW_KEY_WORLD_2)
        {
            char ch = (char)keyCode;
            bool uppercaseModPressed = Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
                                       Input::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT);
            if (!uppercaseModPressed && ch >= 'A' && ch <= 'Z')
                ch = tolower(ch);

            m_Text += ch;

            if (Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale + m_TextHOffset <= m_Size.x - m_TextHOffset)
                m_TextLength = Renderer2D::GetTextSize(m_Camera, m_Text, m_FontName).x * m_TextScale;
            else
                m_TextLength = m_Size.x - m_TextHOffset * 2.0f;

            if (m_KeyChangedCallback)
                m_KeyChangedCallback({ m_Id, m_Text });

            return true;
        }
    }

    return false;
}

bool InputBox::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    auto relMousePos = m_Camera->CalculateRelativeMousePosition();
    if (Util::IsPointInRectangle(m_Position, m_Size, relMousePos))
    {
        m_Focused = true;
        return true;
    }
    else
    {
        m_Focused = false;
    }

    return false;
}

unsigned int InputBox::GetNextId()
{
    static unsigned int counter = 0;
    return counter++;
}
