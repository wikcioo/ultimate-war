#pragma once

#include <memory>

#include "widgets/button.h"
#include "menu/views/view.h"
#include "event/window_event.h"

class BackableView : public MainMenuView
{
public:
    BackableView();
    ~BackableView();

    virtual void OnUpdate() override;
    virtual void OnEvent(Event& event) override;

private:
    void OnWindowSizeChanged();

    void OnBackButtonPressed(ButtonCallbackData data);
    glm::vec2 CalculateBackButtonPosition();

private:
    std::unique_ptr<Button> m_BackButton;
    glm::vec2 m_BackButtonSize;
};
