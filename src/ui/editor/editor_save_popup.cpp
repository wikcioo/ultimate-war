#include "editor_save_popup.h"

#include "editor/editor_layer.h"
#include "core/logger.h"
#include "util/util.h"
#include "widgets/notification.h"

#include <GLFW/glfw3.h>

EditorSavePopup::EditorSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset, const glm::vec2& size)
    : SavePopup(UICamera, "Input map file name", offset, size)
{
}

bool EditorSavePopup::OnKeyPressed(KeyPressedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_X)
    {
        EditorLayer::Get().GetCameraController()->ResetKeys();
    }

    return SavePopup::OnKeyPressed(event);
}

bool EditorSavePopup::OnSave()
{
    auto mapName = m_InputBox->GetText();
    Util::Trim(mapName);

    if (mapName.empty())
    {
        LOG_WARN("EditorSavePopup: Empty map name");
        Notification::Create("Empty map name", NotificationLevel::WARNING);
        return false;
    }

    if (!Util::IsAlphanumericWithSpaces(mapName))
    {
        LOG_WARN("EditorSavePopup: Map name is not alphanumeric");
        Notification::Create("Map name is not alphanumeric", NotificationLevel::WARNING);
        return false;
    }

    EditorLayer::Get().SaveMap(mapName);

    return true;
}
