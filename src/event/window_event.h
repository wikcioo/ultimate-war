#pragma once

#include "event.h"

class WindowClosedEvent : public Event
{
public:
    WindowClosedEvent() {}

    EVENT_CLASS_TYPE(WindowClosed);
};

class WindowResizedEvent : public Event
{
public:
    WindowResizedEvent(unsigned int width, unsigned int height)
        : m_Width(width), m_Height(height)
    {
    }

    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }

    EVENT_CLASS_TYPE(WindowResized);

private:
    unsigned int m_Width, m_Height;
};

class WindowMinimizedEvent : public Event
{
public:
    WindowMinimizedEvent() {}

    EVENT_CLASS_TYPE(WindowMinimized);
};

class WindowMaximizedEvent : public Event
{
public:
    WindowMaximizedEvent() {}

    EVENT_CLASS_TYPE(WindowMaximized);
};
