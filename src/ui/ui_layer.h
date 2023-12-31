#pragma once

#include <memory>

#include "core/camera.h"
#include "layer/layer.h"
#include "game/map_manager.h"
#include "event/window_event.h"
#include "ui/common/confirm_popup.h"
#include "ui/ui_element.h"
#include "ui/ui_element_stack.h"

class UILayer : public Layer
{
public:
    UILayer();
    ~UILayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

    void PushGameLayerElements();
    void PushEditorLayerElements();

    static std::shared_ptr<ConfirmPopup> s_ConfirmPopup;

private:
    bool OnWindowResized(WindowResizedEvent& event);
    void ConfigureNotification();

private:
    std::shared_ptr<OrthographicCamera> m_UICamera;
    UIElementStack m_UIElementStack;
};
