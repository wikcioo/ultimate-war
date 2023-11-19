#pragma once

#include <vector>

#include "widgets/button.h"
#include "menu/views/view.h"
#if defined(DEBUG)
    #include "event/key_event.h"
#endif

class MainView : public MainMenuView
{
public:
    MainView();
    ~MainView();

    virtual void OnAttach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    void OnStartNewGameButtonPressed(ButtonCallbackData data);
    void OnContinueLastGameButtonPressed(ButtonCallbackData data);
    void OnOpenMapEditorButtonPressed(ButtonCallbackData data);
    void OnShowContributorsButtonPressed(ButtonCallbackData data);
    void OnExitButtonPressed(ButtonCallbackData data);

#if defined(DEBUG)
    bool OnKeyPressed(KeyPressedEvent& event);
#endif

private:
    std::vector<Button*> m_Buttons;
};
