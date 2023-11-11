#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "core/camera.h"
#include "event/mouse_event.h"

#define BIND_BTN_CALLBACK_FN(fn) std::bind(&fn, this, std::placeholders::_1)

struct ButtonConfig
{
    std::string Text;
    glm::vec2 Position = glm::vec2(0.0f);
    glm::vec2 Size = glm::vec2(0.0f);
    glm::vec3 TextColor = glm::vec3(0.9f);
    glm::vec3 BackgroundColor = glm::vec3(0.3f);
    std::string FontName = "rexlia";
};

struct ButtonCallbackData
{
    unsigned int Id;
    std::string Text;
};

class Button
{
public:
    Button(const std::shared_ptr<OrthographicCamera>& camera, ButtonConfig config);
    ~Button() = default;

    void OnUpdate();
    void OnEvent(Event& event);

    void SetPressedCallback(std::function<void(ButtonCallbackData)> callback) { m_PressedCallback = callback; }
    void SetPosition(const glm::vec2& position) { m_Position = position; }

    const glm::vec2& GetSize() { return m_Size; }
    unsigned int GetId() { return m_Id; }

    bool IsToggled() { return m_Toggled; }
    void SetToggled(bool toggled) { m_Toggled = toggled; }

    void SetDisabled(bool disabled) { m_Disabled = disabled; }

private:
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

private:
    static unsigned int GetNextId();

private:
    std::shared_ptr<OrthographicCamera> m_Camera;
    unsigned int m_Id;
    std::string m_Text;
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    glm::vec3 m_TextColor;
    glm::vec3 m_BackgroundColor;
    std::string m_FontName;
    float m_TextScale;
    bool m_Toggled;
    bool m_Disabled;

    std::function<void(ButtonCallbackData)> m_PressedCallback;
};
