#pragma once

#include <memory>

#include "core/camera_controller.h"
#include "layer/layer.h"
#include "event/window_event.h"
#include "event/mouse_event.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "graphics/texture.h"
#include "graphics/vertex_array.h"
#include "game/map.h"
#include "game/arrow.h"

class GameLayer : public Layer
{
public:
    GameLayer();
    ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;
    virtual void OnDebugRender() override;

private:
    std::pair<float, float> CalculateRelativeMousePosition();
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

private:
    std::shared_ptr<Shader> m_ColorShader;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<OrthographicCameraController> m_CameraController;
    std::unique_ptr<GameMap> m_GameMap;
    std::unique_ptr<Arrow> m_Arrow;

    glm::vec2 m_StarPosition = {0.0f, 0.0f};
};
