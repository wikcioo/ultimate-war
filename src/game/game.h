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
    std::shared_ptr<Shader> m_ColorShader;
    std::shared_ptr<Shader> m_TextureShader;
    std::shared_ptr<Texture2D> m_StarTexture;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<VertexArray> m_QuadVA;
    std::shared_ptr<OrthographicCameraController> m_CameraController;
    std::unique_ptr<GameMap> m_GameMap;
    glm::vec3 m_TileColor;
};
