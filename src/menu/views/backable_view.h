#pragma once

#include <memory>

#include "widgets/button.h"
#include "menu/views/view.h"
#include "event/window_event.h"
#include "event/key_event.h"

class BackableView : public MainMenuView
{
public:
    BackableView(ViewName backViewName);
    ~BackableView();

    virtual void OnAttach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    void OnWindowSizeChanged();
    bool OnKeyPressed(KeyPressedEvent& event);

    void OnBackButtonPressed(ButtonCallbackData data);
    glm::vec2 CalculateBackButtonPosition();

private:
    std::unique_ptr<Button> m_BackButton;
    glm::vec2 m_BackButtonSize;
    ViewName m_BackViewName;
};
