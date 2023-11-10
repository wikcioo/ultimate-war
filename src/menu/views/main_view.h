#pragma once

#include <vector>

#include "widgets/button.h"
#include "menu/views/view.h"

class MainView : public MainMenuView
{
public:
    MainView();
    ~MainView();

    virtual void OnAttach() override;
    virtual void OnUpdate() override;
    virtual void OnEvent(Event& event) override;

private:
    void OnStartNewGameButtonPressed(ButtonCallbackData data);

private:
    std::vector<Button*> m_Buttons;
};
