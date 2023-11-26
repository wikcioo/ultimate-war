#include "notification.h"

#include <unordered_map>

#include <glm/glm.hpp>

#include "util/util.h"
#include "graphics/renderer.h"
#include "core/resource_manager.h"

#include <GLFW/glfw3.h>

glm::vec2 Notification::s_Position;
float Notification::s_CloseIconSize;
float Notification::s_BorderOffset;
float Notification::s_ContentOffset;
float Notification::s_YDirection;
float Notification::s_TextScale;
float Notification::s_LevelTextScale;
std::shared_ptr<OrthographicCamera> Notification::s_Camera;
NotificationConfig Notification::s_Config;
std::vector<Notification::NotificationInstance> Notification::s_Notifications;

static std::string NOTIFICATION_FONT = "rexlia";

static std::unordered_map<NotificationLevel, glm::vec3> s_LevelColorMap = {
    { NotificationLevel::INFO,    { 0.9f, 0.9f, 0.9f } },
    { NotificationLevel::WARNING, { 0.9f, 0.9f, 0.1f } },
    { NotificationLevel::ERR,     { 0.9f, 0.1f, 0.1f } }
};

static std::string NotificationLevelToStr(NotificationLevel level)
{
    switch (level)
    {
        case NotificationLevel::INFO:    return "Info";
        case NotificationLevel::WARNING: return "Warning";
        case NotificationLevel::ERR:     return "Error";
        default: return "Unknown";
    };
}

void Notification::Configure(const std::shared_ptr<OrthographicCamera>& camera, NotificationConfig config)
{
    s_Camera = camera;
    s_Config = config;

    s_CloseIconSize = 0.03f;
    s_BorderOffset = 0.01f;
    s_ContentOffset = 0.01f;
    s_TextScale = 0.14f;
    s_LevelTextScale = 0.18f;

    RecalculatePositionAndDirection();
}

void Notification::Create(const std::string& text, NotificationLevel level)
{
    std::string parsedText = ParseTextToFit(text);
    float parsedTextHeight = GetTextHeight(parsedText);
    float totalHeight = parsedTextHeight + s_CloseIconSize + (s_BorderOffset * 2.0f) + s_ContentOffset;
    s_Notifications.push_back({ parsedText, level, GetTime() + s_Config.TotalVisibleTimeMS, totalHeight });
}

void Notification::OnUpdate(float dt)
{
    Draw();
    CleanUpExpired();
}

void Notification::OnEvent(Event& event)
{
    if (event.GetCategory() == EventCategory::Window)
        OnWindowSizeChanged();

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_STATIC_FN(Notification::OnMouseButtonPressed));
}

// check if the mouse was clicked on the cross icon of the notification
// if so, it removes it from the notification list
bool Notification::OnMouseButtonPressed(MouseButtonEvent& event)
{
    int notificationCount = s_Notifications.size();
    auto relMousePos = s_Camera->CalculateRelativeMousePosition();

    std::vector<size_t> toBeDeletedIndexes;

    for (size_t i = 0; i < notificationCount; i++)
    {
        auto& instance = s_Notifications[i];

        // re-evaluate position.y based on the amount of notifications
        // and height of each individual notification
        float heightOffset = 0.0f;
        for (int j = notificationCount - 1; j > i; j--)
            heightOffset += s_YDirection * (s_Notifications[j].Height + s_Config.InstanceOffset);

        glm::vec2 instancePosition = {
            s_Position.x,
            s_Position.y +
                s_YDirection * (instance.Height / 2.0f) +
                heightOffset
        };

        glm::vec2 closeIconPosition = instancePosition +
                                     (glm::vec2(s_Config.Width, instance.Height) / 2.0f) -
                                     (s_CloseIconSize / 2.0f) - s_BorderOffset;

        if (Util::IsPointInRectangle(closeIconPosition, glm::vec2(s_CloseIconSize), relMousePos))
            toBeDeletedIndexes.push_back(i);
    }

    for (size_t index : toBeDeletedIndexes)
        s_Notifications.erase(s_Notifications.begin() + index);

    return toBeDeletedIndexes.size() > 0;
}

void Notification::Draw()
{
    static std::shared_ptr<Texture2D> s_CrossTexture = ResourceManager::GetTexture("cross");

    float time = GetTime();
    int notificationCount = s_Notifications.size();
    auto relMousePos = s_Camera->CalculateRelativeMousePosition();

    for (int i = 0; i < notificationCount; i++)
    {
        auto& instance = s_Notifications[i];

        if (instance.EndTime > time)
        {
            float alpha = 1.0f;
            float timeUntilExpired = instance.EndTime - time;
            if (timeUntilExpired < s_Config.FadeOutTimeMS)
            {
                // adjust alpha to value between 0.0 and 1.0
                alpha = timeUntilExpired / s_Config.FadeOutTimeMS;
            }

            // re-evaluate position.y based on the amount of notifications
            // and height of each individual notification
            float heightOffset = 0.0f;
            for (int j = notificationCount - 1; j > i; j--)
                heightOffset += s_YDirection * (s_Notifications[j].Height + s_Config.InstanceOffset);

            glm::vec2 instancePosition = {
                s_Position.x,
                s_Position.y +
                    s_YDirection * (instance.Height / 2.0f) +
                    heightOffset
            };

            // draw background
            Renderer2D::DrawQuad(
                instancePosition,
                glm::vec2(s_Config.Width, instance.Height),
                glm::vec4(0.3f, 0.3f, 0.3f, alpha)
            );

            // draw border and refresh timeout if hovered over
            if (Util::IsPointInRectangle(instancePosition, glm::vec2(s_Config.Width, instance.Height), relMousePos))
            {
                Renderer2D::DrawQuad(
                    instancePosition,
                    glm::vec2(s_Config.Width, instance.Height),
                    glm::vec4(0.9f, 0.9f, 0.9f, alpha),
                    3.0f
                );
                instance.EndTime = GetTime() + s_Config.TotalVisibleTimeMS;
            }

            // draw content text
            Renderer2D::DrawTextStr(
                instance.Text,
                {
                    instancePosition.x - (s_Config.Width / 2.0f) + s_BorderOffset,
                    instancePosition.y + (instance.Height / 2.0f) - s_CloseIconSize - (s_BorderOffset * 2.0f) - s_ContentOffset
                },
                s_TextScale,
                glm::vec4(0.9f, 0.9f, 0.9f, alpha),
                HTextAlign::LEFT,
                VTextAlign::TOP,
                NOTIFICATION_FONT
            );

            glm::vec2 closeIconPosition = instancePosition +
                                         (glm::vec2(s_Config.Width, instance.Height) / 2.0f) -
                                         (s_CloseIconSize / 2.0f) - s_BorderOffset;

            // draw close icon
            Renderer2D::DrawQuad(
                closeIconPosition,
                glm::vec2(s_CloseIconSize),
                s_CrossTexture,
                glm::vec4(1.0, 1.0f, 1.0f, alpha)
            );

            // draw magnified close icon if hovered over
            if (Util::IsPointInRectangle(closeIconPosition, glm::vec2(s_CloseIconSize), relMousePos))
            {
                Renderer2D::DrawQuad(
                    closeIconPosition,
                    glm::vec2(s_CloseIconSize * 1.15f),
                    s_CrossTexture,
                    glm::vec4(1.0, 1.0f, 1.0f, alpha)
                );
            }

            // draw level text
            Renderer2D::DrawTextStr(
                NotificationLevelToStr(instance.Level),
                {
                    instancePosition.x - (s_Config.Width / 2.0f) + s_BorderOffset,
                    closeIconPosition.y
                },
                s_LevelTextScale,
                glm::vec4(s_LevelColorMap[instance.Level], alpha),
                HTextAlign::LEFT,
                VTextAlign::MIDDLE,
                NOTIFICATION_FONT
            );
        }
    }
}

void Notification::CleanUpExpired()
{
    float time = GetTime();
    Util::RemoveElementsFromContainerWithCondition<std::vector<NotificationInstance>, NotificationInstance>(
        s_Notifications, [time](const NotificationInstance& instance) {
        return instance.EndTime <= time;
    });
}

std::string Notification::ParseTextToFit(const std::string& text)
{
    std::string result;
    float notificationWidth = s_Config.Width - (s_BorderOffset * 2.0f);
    int lastNewLineIndex = 0;

    for (size_t i = 0; i < text.size(); i++)
    {
        std::string subText = text.substr(lastNewLineIndex, i + 1 - lastNewLineIndex);
        auto subTextSize = Renderer2D::GetTextSize(s_Camera, subText, NOTIFICATION_FONT) * s_TextScale;

        if (subTextSize.x >= notificationWidth)
        {
            char lastChar = subText[subText.size() - 1];
            if (lastChar != ' ' && lastChar != '\n')
                result += '-';
            result += '\n';
            lastNewLineIndex = i;
        }

        result += text[i];
    }

    return result;
}

float Notification::GetTextHeight(const std::string& text)
{
    int numberOfLines = 1;
    for (const auto& ch : text)
    {
        if (ch == '\n')
            numberOfLines++;
    }

    float charHeight = Renderer2D::GetTextSize(s_Camera, "A", NOTIFICATION_FONT).y * s_TextScale;
    return (charHeight + charHeight * FONT_Y_SPACING_RATIO) * numberOfLines;
}

void Notification::OnWindowSizeChanged()
{
    RecalculatePositionAndDirection();
}

void Notification::RecalculatePositionAndDirection()
{
    glm::vec2 relWindowSize = s_Camera->CalculateRelativeWindowSize();

    float xPos = 0.0f;
    switch (s_Config.HAttach)
    {
        case NotificationHAttach::LEFT:
        {
            xPos = -relWindowSize.x / 2.0f + s_Config.Width / 2.0f + s_Config.WindowOffset.x;
            break;
        }
        case NotificationHAttach::RIGHT:
        {
            xPos =  relWindowSize.x / 2.0f - s_Config.Width / 2.0f - s_Config.WindowOffset.x;
            break;
        }
        case NotificationHAttach::MIDDLE:
            break;
    }

    float yPos = 0.0f;
    float yDir = 1.0f; // 1.0 is up, -1.0 is down
    switch (s_Config.VAttach)
    {
        case NotificationVAttach::TOP:
        {
            yPos =  relWindowSize.y / 2.0f - s_Config.WindowOffset.y;
            yDir = -1.0f;
            break;
        }
        case NotificationVAttach::BOTTOM:
        {
            yPos = -relWindowSize.y / 2.0f + s_Config.WindowOffset.y;
            break;
        }
        case NotificationVAttach::MIDDLE:
            break;
    }

    s_Position = { xPos, yPos };
    s_YDirection = yDir;
}

float Notification::GetTime()
{
    return (float)glfwGetTime() * 1000.0f;
}
