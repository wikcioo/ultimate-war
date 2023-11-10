#pragma once

#include <vector>

#include "widgets/button.h"

class ToggleButtonGroup
{
public:
    ToggleButtonGroup(const std::shared_ptr<OrthographicCamera>& camera);
    ~ToggleButtonGroup() = default;

    void AddButton(ButtonConfig buttonConfig);

    void OnUpdate();
    void OnEvent(Event& event);

    void SetPressedCallback(std::function<void(ButtonCallbackData)> callback) { m_PressedCallback = callback; }

private:
    void OnButtonPressed(ButtonCallbackData data);

private:
    std::vector<Button*> m_Buttons;
    std::shared_ptr<OrthographicCamera> m_Camera;
    std::function<void(ButtonCallbackData)> m_PressedCallback;
};
