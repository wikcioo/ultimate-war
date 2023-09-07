#pragma once

#include <string>

#include "event/event.h"

class Layer
{
public:
    Layer(const std::string& name);
    virtual ~Layer() = default;

    virtual void OnAttach() {};
    virtual void OnDetach() {};
    virtual void OnUpdate(float dt) {};
    virtual void OnEvent(Event& event) {};
    virtual void OnDebugRender() {};

    inline const std::string& GetName() const { return m_Name; }

private:
    std::string m_Name;
};
