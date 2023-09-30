#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "core/camera.h"
#include "graphics/buffer.h"
#include "game/map_manager.h"
#include "event/mouse_event.h"
#include "event/window_event.h"
#include "ui/ui_element.h"

class Minimap : public UIElement
{
public:
    Minimap(const std::shared_ptr<OrthographicCamera>& UICamera,
            const std::shared_ptr<OrthographicCamera>& gameCamera,
            const std::shared_ptr<GameMapManager>& gameMapManager,
            const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    ~Minimap() = default;

    virtual void OnEvent(Event& event) override;
    virtual void Draw() override;

private:
    bool OnWindowResized(WindowResizedEvent& event);
    bool OnMouseMoved(MouseMovedEvent& event);
    bool OnMouseScrolled(MouseScrolledEvent& event);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

private:
    glm::vec2 m_Offset;
    glm::vec2 m_MinimapPos;
    std::unique_ptr<FrameBuffer> m_Framebuffer;
    std::shared_ptr<OrthographicCamera> m_UICamera;
    std::shared_ptr<OrthographicCamera> m_GameCamera;
    std::shared_ptr<OrthographicCamera> m_MinimapCamera;
    std::shared_ptr<GameMapManager> m_GameMapManager;
};
