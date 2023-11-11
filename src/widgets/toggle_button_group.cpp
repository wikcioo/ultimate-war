#include "toggle_button_group.h"

ToggleButtonGroup::ToggleButtonGroup(const std::shared_ptr<OrthographicCamera>& camera)
    : m_Camera(camera)
{
}

void ToggleButtonGroup::AddButton(ButtonConfig buttonConfig)
{
    Button* button = new Button(m_Camera, buttonConfig);
    button->SetPressedCallback(BIND_BTN_CALLBACK_FN(ToggleButtonGroup::OnButtonPressed));
    m_Buttons.push_back(button);
}

void ToggleButtonGroup::OnUpdate()
{
    for (auto button : m_Buttons)
        button->OnUpdate();
}

void ToggleButtonGroup::OnEvent(Event& event)
{
    for (auto button : m_Buttons)
    {
        if (event.Handled)
            break;
        button->OnEvent(event);
    }
}

void ToggleButtonGroup::OnButtonPressed(ButtonCallbackData data)
{
    for (auto button : m_Buttons)
    {
        if (button->GetId() == data.Id)
            button->SetToggled(true);
        else
            button->SetToggled(false);
    }

    m_PressedCallback(data);
}
