#pragma once

#include <memory>

#include "core/camera.h"
#include "layer/layer.h"
#include "event/window_event.h"
#include "event/mouse_event.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "graphics/vertex_array.h"

class GameLayer : public Layer
{
public:
    GameLayer();
    ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    bool OnWindowResize(WindowResizedEvent& event);
    bool OnMouseScrolled(MouseScrolledEvent& event);

private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<OrthographicCamera> m_Camera;

    float m_CameraMovementSpeed = 3.0f;
    float m_CameraRotationSpeed = 90.0f;
};
