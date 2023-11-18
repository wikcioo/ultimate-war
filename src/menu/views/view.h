#pragma once

#include "event/event.h"
#include "menu/main_menu_layer.h"

class MainMenuView
{
public:
    MainMenuView()
        : m_Camera(MainMenuLayer::Get().GetMainMenuCamera())
    {
    }
    virtual ~MainMenuView() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnEvent(Event& event) {}

protected:
    std::shared_ptr<OrthographicCamera> m_Camera;
};
