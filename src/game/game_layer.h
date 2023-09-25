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
#include "game/map_manager.h"
#include "game/arrow.h"
#include "game/color_data.h"

class GameLayer : public Layer
{
#if defined(DEBUG)
    friend class DebugLayer;
#endif
public:
    GameLayer();
    ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

    inline std::shared_ptr<OrthographicCameraController> GetCameraController() const { return m_CameraController; }
    inline std::shared_ptr<GameMapManager> GetGameMapManager() const { return m_GameMapManager; }

private:
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

private:
    std::shared_ptr<OrthographicCameraController> m_CameraController;
    std::shared_ptr<GameMapManager> m_GameMapManager;
    std::shared_ptr<Arrow> m_Arrow;

    glm::vec2 m_StarPosition = {0.0f, 0.0f};
};
