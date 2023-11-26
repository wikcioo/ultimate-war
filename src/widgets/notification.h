#pragma once

#include <memory>
#include <string>

#include "core/camera.h"
#include "event/mouse_event.h"
#include "graphics/texture.h"

enum class NotificationLevel
{
    INFO,
    WARNING,
    ERR
};

enum class NotificationHAttach
{
    LEFT,
    MIDDLE,
    RIGHT
};

enum class NotificationVAttach
{
    TOP,
    MIDDLE,
    BOTTOM
};

struct NotificationConfig
{
    float TotalVisibleTimeMS = 10000;           // notification will completely disappear after this time
    float FadeOutTimeMS = 2000;                 // notification will start fading out this much time before it disappears
    float Width = 0.8f;                         // width of individual notification (height adjusted automatically)
    glm::vec2 WindowOffset = { 0.05f, 0.05f };  // offset from window corner
    float InstanceOffset = 0.01f;               // how far apart is each notification
    NotificationHAttach HAttach = NotificationHAttach::MIDDLE; // horizontal attachment
    NotificationVAttach VAttach = NotificationVAttach::TOP;    // vertical attachment
};

class Notification
{
public:
    static void Configure(const std::shared_ptr<OrthographicCamera>& camera, NotificationConfig config = NotificationConfig());
    static void Create(const std::string& text, NotificationLevel level);

    static void OnUpdate(float dt);
    static void OnEvent(Event& event);

private:
    static bool OnMouseButtonPressed(MouseButtonEvent& event);

    static void Draw();
    static void CleanUpExpired();
    static std::string ParseTextToFit(const std::string& text);
    static float GetTextHeight(const std::string& text);

    static void OnWindowSizeChanged();
    static void RecalculatePositionAndDirection();
    static float GetTime();

private:
    struct NotificationInstance
    {
        std::string Text;
        NotificationLevel Level;
        float EndTime;
        float Height;
    };

    static glm::vec2 s_Position;
    static float s_CloseIconSize;
    static float s_BorderOffset;
    static float s_ContentOffset;
    static float s_YDirection;
    static float s_TextScale;
    static float s_LevelTextScale;

    static std::shared_ptr<OrthographicCamera> s_Camera;
    static NotificationConfig s_Config;
    static std::vector<NotificationInstance> s_Notifications;
};
