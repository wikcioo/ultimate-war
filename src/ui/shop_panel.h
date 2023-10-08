#pragma once

#include <memory>
#include <unordered_map>

#include "core/camera.h"
#include "ui/ui_element.h"
#include "game/unit.h"
#include "event/mouse_event.h"
#include "event/window_event.h"
#include "event/key_event.h"
#include "graphics/texture.h"

class ShopPanel : public UIElement
{
public:
    ShopPanel(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset,
              const glm::vec2& unitSize = glm::vec2(0.15f), float unitOffset = 0.05f);
    ~ShopPanel() = default;

    virtual void OnEvent(Event& event) override;
    virtual void Draw() override;

private:
    bool OnMouseScrolled(MouseScrolledEvent& event);
    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnMouseButtonPressedPanel(MouseButtonPressedEvent& event);
    bool OnMouseButtonPressedGame(MouseButtonPressedEvent& event);
    void SetCursorAttachedUnit(UnitType type);
    bool IsUnitAttachedToCursor() { return m_CursorAttachedUnit.Type != UnitType::NONE; }

private:
    std::unordered_map<UnitType, int> m_UnitCostMap;
    int m_UnitCount;
    float m_UnitOffset;
    glm::vec2 m_UnitSize;

    struct
    {
        UnitType Type = UnitType::NONE;
        std::shared_ptr<Texture2D> Texture;
    } m_CursorAttachedUnit;
};
