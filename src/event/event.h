#pragma once

#include <string>
#include <sstream>
#include <functional>

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define BIND_EVENT_STATIC_FN(fn) std::bind(&fn, std::placeholders::_1)

enum class EventType
{
    None = 0,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    WindowClosed, WindowResized, WindowMinimized, WindowMaximized
};

enum class EventCategory
{
    None = 0,
    Key,
    Mouse,
    Window
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                               virtual EventType GetType() const override { return GetStaticType(); }\
                               virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual EventCategory GetCategory() const override { return EventCategory::category; }

class Event
{
public:
    friend class EventDispatcher;

    virtual EventType GetType() const = 0;
    virtual const char* GetName() const = 0;
    virtual EventCategory GetCategory() const = 0;
    virtual std::string ToString() const { return GetName(); }

    bool Handled = false;
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
        if (m_Event.GetType() == T::GetStaticType())
        {
            m_Event.Handled = callbackFunc(*(T*)&m_Event);
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
