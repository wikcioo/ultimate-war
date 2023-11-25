#pragma once

#include <memory>

#include "ui/ui_element.h"
#include "widgets/button.h"
#include "widgets/input_box.h"
#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"

class EditorSavePopup : public UIElement
{
public:
    EditorSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera,
                    const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    ~EditorSavePopup() = default;

    virtual void Draw() override;
    virtual void OnEvent(Event& event) override;

    void OnSaveMapButtonPressed(ButtonCallbackData data);
    void OnCancelSaveButtonPressed(ButtonCallbackData data);
    void OnMapNameInputBoxChanged(InputBoxCallbackData data);
    void OnMapNameInputBoxAccepted(InputBoxCallbackData data);

    void DrawSavingPopup();

    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
    bool OnMouseButtonScrolled(MouseScrolledEvent& event);

    bool SaveMap();

private:
    void ClosePopup();

private:
    std::unique_ptr<Button> m_SaveMapButton;
    std::unique_ptr<Button> m_CancelSaveButton;
    std::unique_ptr<InputBox> m_MapNameInputBox;
    bool m_DisplaySavingPopup;
};
