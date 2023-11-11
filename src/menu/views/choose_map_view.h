#pragma once

#include <memory>
#include <vector>

#include "event/window_event.h"
#include "menu/views/backable_view.h"
#include "widgets/toggle_button_group.h"

class ChooseMapView : public BackableView
{
public:
    ChooseMapView();
    ~ChooseMapView();

    virtual void OnUpdate() override;
    virtual void OnEvent(Event& event) override;

    const std::string& GetSelectedMap() { return m_SelectedMap; }

private:
    void OnMapButtonPressed(ButtonCallbackData data);

private:
    std::string m_SelectedMap;
    std::unique_ptr<ToggleButtonGroup> m_MapButtonGroup;
};
