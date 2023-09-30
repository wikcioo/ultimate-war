#pragma once

#include <memory>

#include "core/camera.h"
#include "layer/layer.h"
#include "ui/minimap.h"
#include "game/map_manager.h"

class UILayer : public Layer
{
public:
    UILayer();
    ~UILayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    std::shared_ptr<OrthographicCamera> m_UICamera;
    std::shared_ptr<OrthographicCamera> m_GameCamera;
    std::vector<std::shared_ptr<UIElement>> m_UIElements;
};
