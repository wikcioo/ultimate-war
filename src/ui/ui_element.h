#pragma once

#include <glm/glm.hpp>

#include "event/event.h"

class UIElement
{
public:
    UIElement(const glm::vec2& position, const glm::vec2& size)
        : m_Position(position), m_Size(size)
    {
    }
    virtual ~UIElement() = default;

    virtual glm::vec2 GetPosition() const { return m_Position; }
    virtual glm::vec2 GetSize() const { return m_Size; }

    virtual void OnEvent(Event& event) {}
    virtual void Draw() = 0;

protected:
    glm::vec2 m_Position;
    glm::vec2 m_Size;
};
