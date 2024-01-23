#pragma once

#include "ui/ui_element.h"
#include "widgets/button.h"

class ConfirmPopup : public UIElement
{
public:
    ConfirmPopup(const std::shared_ptr<OrthographicCamera>& UICamera,
                 const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    virtual ~ConfirmPopup() = default;

    virtual void Draw() override;
    virtual void OnEvent(Event& event) override;

    void Open(const std::string& text, std::function<void()> yesCallback, std::function<void()> noCallback);
    void Close() { m_DisplayConfirmPopup = false; }

private:
    void OnYesButtonPressed(ButtonCallbackData data);
    void OnNoButtonPressed(ButtonCallbackData data);

    void DrawDimmedBackground();
    void DrawConfirmPopup();

private:
    std::unique_ptr<Button> m_YesButton;
    std::unique_ptr<Button> m_NoButton;
    std::function<void()> m_YesCallback;
    std::function<void()> m_NoCallback;
    bool m_DisplayConfirmPopup;
    std::string m_Text;
};
