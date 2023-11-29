#include "game_save_popup.h"

#include "core/application.h"
#include "util/util.h"

#include <GLFW/glfw3.h>

GameSavePopup::GameSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset, const glm::vec2& size)
    : SavePopup(UICamera, "Input save file name", offset, size)
{
}

bool GameSavePopup::OnSave()
{
    auto saveName = m_InputBox->GetText();
    Util::Trim(saveName);

    if (saveName.empty())
    {
        LOG_WARN("GameSavePopup: Empty save name");
        return false;
    }

    if (!Util::IsAlphanumericWithSpaces(saveName))
    {
        LOG_WARN("GameSavePopup: Save name is not alphanumeric");
        return false;
    }

    Application::Get().SaveGame(saveName);
    return true;
}
