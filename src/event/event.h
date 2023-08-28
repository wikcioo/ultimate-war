#pragma once

#include <string>
#include <sstream>
#include <functional>

enum class EventType
{
    None = 0,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    WindowClosed, WindowResized, WindowMinimized, WindowMaximized
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                               virtual EventType GetEventType() const override { return GetStaticType(); }\
                               virtual const char* GetName() const override { return #type; }

class Event
{
public:
    friend class EventDispatcher;

    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual std::string ToString() const { return GetName(); }

protected:
    bool m_Handled = false;
};

class EventDispatcher
{
    template <typename T>
    using EventFn = std::function<bool(T&)>;
public:
    EventDispatcher(Event& event)
        : m_Event(event)
    {
    }

    template <typename T>
    bool Dispatch(EventFn<T> callbackFunc)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.m_Handled = callbackFunc(*(T*)&m_Event);
            return true;
        }

        return false;
    }

private:
    Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& event)
{
    return os << event.ToString();
}
