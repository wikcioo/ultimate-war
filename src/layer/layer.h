#pragma once

#include <string>

#include "event/event.h"

class Layer
{
public:
    Layer(const std::string& name, bool isActive = true);
    virtual ~Layer() = default;

    virtual void OnAttach() {};
    virtual void OnDetach() {};
    virtual void OnUpdate(float dt) {};
    virtual void OnEvent(Event& event) {};

    inline const std::string& GetName() const { return m_Name; }

    bool IsActive() { return m_IsActive; }
    void SetIsActive(bool isActive) { m_IsActive = isActive; }

private:
    std::string m_Name;
    bool m_IsActive;
};
