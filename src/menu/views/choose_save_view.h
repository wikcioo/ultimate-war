#pragma once

#include <vector>

#include "widgets/button.h"
#include "menu/views/backable_view.h"

class ChooseSaveView : public BackableView
{
public:
    ChooseSaveView();
    ~ChooseSaveView();

    virtual void OnAttach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    void OnSaveButtonPressed(ButtonCallbackData data);

private:
    std::vector<Button*> m_Buttons;
};
