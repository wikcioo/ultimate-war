#include "game_save_popup.h"

#include "core/application.h"
#include "util/util.h"
#include "widgets/notification.h"

#include <GLFW/glfw3.h>

GameSavePopup::GameSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset, const glm::vec2& size)
    : SavePopup(UICamera, "Input save file name", offset, size)
{
}

bool GameSavePopup::OnKeyPressed(KeyPressedEvent& event)
{
    auto gameName = GameLayer::Get().GetName();
    if (event.GetKeyCode() == GLFW_KEY_X && !gameName.empty())
    {
        Application::Get().SaveGame(gameName);
        Notification::Create("Saved the game", NotificationLevel::INFO);
        return true;
    }

    return SavePopup::OnKeyPressed(event);
}

bool GameSavePopup::OnSave()
{
    auto saveName = m_InputBox->GetText();
    Util::Trim(saveName);

    if (saveName.empty())
    {
        LOG_WARN("GameSavePopup: Empty save name");
        Notification::Create("Empty save name", NotificationLevel::WARNING);
        return false;
    }

    if (!Util::IsAlphanumericWithSpaces(saveName))
    {
        LOG_WARN("GameSavePopup: Save name is not alphanumeric");
        Notification::Create("Save name is not alphanumeric", NotificationLevel::WARNING);
        return false;
    }

    Application::Get().SaveGame(saveName);
    Notification::Create("Saved the game", NotificationLevel::INFO);
    return true;
}
