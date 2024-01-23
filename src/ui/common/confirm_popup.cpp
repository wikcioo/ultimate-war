#include "confirm_popup.h"

#include "graphics/renderer.h"

ConfirmPopup::ConfirmPopup(const std::shared_ptr<OrthographicCamera>& UICamera,
                           const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * UICamera->GetAspectRatio(), size.y}),
      m_DisplayConfirmPopup(false)
{
    ButtonConfig yesButtonConfig = { "Yes", {-0.4f, -0.15f}, {0.4f, 0.1f} };
    m_YesButton = std::make_unique<Button>(m_UICamera, yesButtonConfig);
    m_YesButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(ConfirmPopup::OnYesButtonPressed));

    ButtonConfig noButtonConfig = { "No", {0.4f, -0.15f}, {0.4f, 0.1f} };
    m_NoButton = std::make_unique<Button>(m_UICamera, noButtonConfig);
    m_NoButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(ConfirmPopup::OnNoButtonPressed));
}

void ConfirmPopup::Draw()
{
    if (m_DisplayConfirmPopup)
    {
        Renderer2D::BeginScene(m_UICamera);

        DrawDimmedBackground();
        DrawConfirmPopup();

        Renderer2D::EndScene();
    }
}

void ConfirmPopup::OnEvent(Event& event)
{
    bool isDisplayed = m_DisplayConfirmPopup;
    if (isDisplayed)
    {
        m_YesButton->OnEvent(event);
        m_NoButton->OnEvent(event);
    }

    // Ignore non window events if saving popup is open
    if (event.GetCategory() != EventCategory::Window)
        event.Handled = isDisplayed;
}

void ConfirmPopup::Open(const std::string& text, std::function<void()> yesCallback, std::function<void()> noCallback)
{
    m_Text = text;
    m_YesCallback = yesCallback;
    m_NoCallback = noCallback;
    m_DisplayConfirmPopup = true;
}

void ConfirmPopup::OnYesButtonPressed(ButtonCallbackData data)
{
    m_YesCallback();
}

void ConfirmPopup::OnNoButtonPressed(ButtonCallbackData data)
{
    m_NoCallback();
}

void ConfirmPopup::DrawDimmedBackground()
{
    Renderer2D::DrawQuad(
        glm::vec2(0.0f),
        { m_UICamera->GetHalfOfRelativeWidth() * 2, m_UICamera->GetHalfOfRelativeHeight() * 2 },
        glm::vec4(0.0f, 0.0f, 0.0f, 0.8f)
    );
}

void ConfirmPopup::DrawConfirmPopup()
{
    Renderer2D::DrawQuad(glm::vec2(0), {1.4f, 0.5f}, {0.3f, 0.6f, 0.9f, 1.0f});

    static float textScale = 0.3f;
    Renderer2D::DrawTextStr(m_Text, {0.0f, 0.08f}, textScale, glm::vec3(0.9f), HTextAlign::MIDDLE);

    m_YesButton->OnUpdate();
    m_NoButton->OnUpdate();
}
