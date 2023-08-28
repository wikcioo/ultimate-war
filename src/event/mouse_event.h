#pragma once

#include "event.h"

class MouseButtonEvent : public Event
{
public:
    inline int GetMouseButton() const { return m_Button; }

protected:
    MouseButtonEvent(int button)
        : m_Button(button)
    {
    }

    int m_Button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(int button)
        : MouseButtonEvent(button)
    {
    }

    virtual std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << m_Button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonPressed);
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(int button)
        : MouseButtonEvent(button)
    {
    }

    virtual std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << m_Button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonReleased);
};

class MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(float x, float y)
        : m_XPos(x), m_YPos(y)
    {
    }

    inline float GetX() const { return m_XPos; }
    inline float GetY() const { return m_YPos; }

    virtual std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << m_XPos << ", " << m_YPos;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseMoved);

private:
    float m_XPos, m_YPos;
};

class MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset)
    {
    }

    inline float getXOffset() const { return m_XOffset; }
    inline float getYOffset() const { return m_YOffset; }

    virtual std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseScrolled);

private:
    float m_XOffset, m_YOffset;
};
