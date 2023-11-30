#pragma once

#include "ui/ui_element.h"
#include "widgets/button.h"
#include "widgets/input_box.h"

class SavePopup : public UIElement
{
public:
    SavePopup(const std::shared_ptr<OrthographicCamera>& UICamera, const std::string& saveText,
              const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    virtual ~SavePopup() = default;

protected:
    virtual bool OnKeyPressed(KeyPressedEvent& event);
    virtual bool OnSave() = 0;

protected:
    void ClosePopup();

private:
    virtual void Draw() override;
    virtual void OnEvent(Event& event) override;

    void DrawDimmedBackground();
    void DrawSavingPopup();

    void OnSaveButtonPressed(ButtonCallbackData data);
    void OnCancelButtonPressed(ButtonCallbackData data);
    void OnInputBoxAccepted(InputBoxCallbackData data);

protected:
    std::unique_ptr<Button> m_SaveButton;
    std::unique_ptr<Button> m_CancelButton;
    std::unique_ptr<InputBox> m_InputBox;
    bool m_DisplaySavingPopup;
    std::string m_SaveText;
};
