#include "save_popup.h"

#include "graphics/renderer.h"

#include <GLFW/glfw3.h>

SavePopup::SavePopup(const std::shared_ptr<OrthographicCamera>& UICamera, const std::string& saveText,
                     const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * UICamera->GetAspectRatio(), size.y}),
      m_SaveText(saveText), m_DisplaySavingPopup(false)
{
    ButtonConfig saveButtonConfig = { "Save", {-0.4f, -0.3f}, {0.4f, 0.1f} };
    m_SaveButton = std::make_unique<Button>(m_UICamera, saveButtonConfig);
    m_SaveButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(SavePopup::OnSaveButtonPressed));

    ButtonConfig cancelButtonConfig = { "Cancel", {0.4f, -0.3f}, {0.4f, 0.1f} };
    m_CancelButton = std::make_unique<Button>(m_UICamera, cancelButtonConfig);
    m_CancelButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(SavePopup::OnCancelButtonPressed));

    InputBoxConfig inputBoxConfig = { glm::vec2(0.0f), glm::vec2(1.0f, 0.10f) };
    m_InputBox = std::make_unique<InputBox>(m_UICamera, inputBoxConfig);
    m_InputBox->SetAcceptedCallback(BIND_INPUT_BOX_CALLBACK_FN(SavePopup::OnInputBoxAccepted));
    m_InputBox->SetCharacterLimit(20);
}

bool SavePopup::OnKeyPressed(KeyPressedEvent& event)
{
    switch (event.GetKeyCode())
    {
        case GLFW_KEY_X:
        {
            if (!m_DisplaySavingPopup)
            {
                m_DisplaySavingPopup = true;
                m_InputBox->Clear();
                m_InputBox->SetFocused(true);
            }

            return true;
        }
        case GLFW_KEY_ESCAPE:
        {
            ClosePopup();
            return true;
        }
        default:
            break;
    }

    return m_DisplaySavingPopup;
}

void SavePopup::Draw()
{
    Renderer2D::BeginScene(m_UICamera);
    if (m_DisplaySavingPopup)
    {
        DrawDimmedBackground();
        DrawSavingPopup();
    }
    Renderer2D::EndScene();
}

void SavePopup::OnEvent(Event& event)
{
    bool isDisplayed = m_DisplaySavingPopup;
    m_SaveButton->OnEvent(event);
    m_CancelButton->OnEvent(event);
    m_InputBox->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(SavePopup::OnKeyPressed));

    // Ignore non window events if saving popup is open
    if (event.GetCategory() != EventCategory::Window)
        event.Handled = isDisplayed;
}

void SavePopup::DrawDimmedBackground()
{
    Renderer2D::DrawQuad(
        glm::vec2(0.0f),
        { m_UICamera->GetHalfOfRelativeWidth() * 2, m_UICamera->GetHalfOfRelativeHeight() * 2 },
        glm::vec4(0.0f, 0.0f, 0.0f, 0.8f)
    );
}

void SavePopup::DrawSavingPopup()
{
    Renderer2D::DrawQuad(glm::vec2(0), {1.4f, 1.0f}, {0.3f, 0.6f, 0.9f, 1.0f});

    static float textScale = 0.5f;
    Renderer2D::DrawTextStr(m_SaveText, {0.0f, 0.3f}, textScale, glm::vec3(0.9f), HTextAlign::MIDDLE);

    m_SaveButton->OnUpdate();
    m_CancelButton->OnUpdate();
    m_InputBox->OnUpdate();
}

void SavePopup::OnCancelButtonPressed(ButtonCallbackData data)
{
    ClosePopup();
}

void SavePopup::OnSaveButtonPressed(ButtonCallbackData data)
{
    if (OnSave())
        ClosePopup();
}

void SavePopup::OnInputBoxAccepted(InputBoxCallbackData data)
{
    if (OnSave())
        ClosePopup();
}

void SavePopup::ClosePopup()
{
    m_DisplaySavingPopup = false;
    m_InputBox->Clear();
    m_InputBox->SetFocused(false);
}
