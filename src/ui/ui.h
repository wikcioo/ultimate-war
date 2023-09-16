#pragma once

#include <memory>

#include "core/camera.h"
#include "layer/layer.h"
#include "ui/minimap.h"
#include "game/map.h"

struct UILayerData
{
    std::shared_ptr<OrthographicCamera> _GameCamera;
    std::shared_ptr<GameMap> _GameMap;
};

class UILayer : public Layer
{
public:
    UILayer(const UILayerData& data);
    ~UILayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    glm::vec2 CalculateScreenRelativeMousePosition();

private:
    std::shared_ptr<OrthographicCamera> m_UICamera;
    std::shared_ptr<OrthographicCamera> m_GameCamera;
    std::vector<std::shared_ptr<UIElement>> m_UIElements;
};
