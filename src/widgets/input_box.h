#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "core/camera.h"
#include "event/key_event.h"
#include "event/mouse_event.h"

#define BIND_INPUT_BOX_CALLBACK_FN(fn) std::bind(&fn, this, std::placeholders::_1)

struct InputBoxConfig
{
    glm::vec2 Position = glm::vec2(0.0f);
    glm::vec2 Size = glm::vec2(0.4f, 0.1f);
    glm::vec3 TextColor = glm::vec3(0.9f);
    glm::vec3 BackgroundColor = glm::vec3(0.3f);
    glm::vec3 BorderColor = glm::vec3(0.9f);
    std::string FontName = "rexlia";
};

struct InputBoxCallbackData
{
    unsigned int Id;
    std::string Text;
};

class InputBox
{
public:
    InputBox(const std::shared_ptr<OrthographicCamera>& camera, InputBoxConfig config);
    ~InputBox() = default;

    void OnUpdate();
    void OnEvent(Event& event);

    void SetAcceptedCallback(std::function<void(InputBoxCallbackData)> callback) { m_AcceptedCallback = callback; }
    void SetOnKeyChangedCallback(std::function<void(InputBoxCallbackData)> callback) { m_KeyChangedCallback = callback; }

    void SetPosition(const glm::vec2& position) { m_Position = position; }
    inline const glm::vec2& GetPosition() { return m_Position; }
    inline const glm::vec2& GetSize() { return m_Size; }

    void SetFocused(bool focused) { m_Focused = focused; }
    inline bool IsFocused() { return m_Focused; }

    void SetText(const std::string& text);
    void SetCharacterLimit(int limit) { m_CharacterLimit = limit; }
    void Clear();

    inline std::string GetText() { return m_Text; }

private:
    bool OnKeyPressed(KeyPressedEvent& event);
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
    glm::vec3 m_BorderColor;
    std::string m_FontName;
    float m_TextScale;
    float m_TextLength;
    float m_TextHOffset;
    bool m_Focused;
    int m_CharacterLimit; // no limit if below zero

    std::function<void(InputBoxCallbackData)> m_AcceptedCallback;
    std::function<void(InputBoxCallbackData)> m_KeyChangedCallback;
};
