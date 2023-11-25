#include "editor_save_popup.h"

#include <string>

#include "graphics/renderer.h"
#include "editor/editor_layer.h"
#include "core/input.h"
#include "core/logger.h"

EditorSavePopup::EditorSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * UICamera->GetAspectRatio(), size.y}),
      m_DisplaySavingPopup(false)
{
    ButtonConfig saveMapButtonConfig = { "Save", {-0.4f, -0.3f}, {0.4f, 0.1f} };
    m_SaveMapButton = std::make_unique<Button>(m_UICamera, saveMapButtonConfig);
    m_SaveMapButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(EditorSavePopup::OnSaveMapButtonPressed));

    ButtonConfig cancelSaveButtonConfig = { "Cancel", {0.4f, -0.3f}, {0.4f, 0.1f} };
    m_CancelSaveButton = std::make_unique<Button>(m_UICamera, cancelSaveButtonConfig);
    m_CancelSaveButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(EditorSavePopup::OnCancelSaveButtonPressed));

    InputBoxConfig mapNameInputBoxConfig = { glm::vec2(0.0f), glm::vec2(1.0f, 0.10f) };
    m_MapNameInputBox = std::make_unique<InputBox>(m_UICamera, mapNameInputBoxConfig);
    m_MapNameInputBox->SetAcceptedCallback(BIND_INPUT_BOX_CALLBACK_FN(EditorSavePopup::OnMapNameInputBoxAccepted));
    m_MapNameInputBox->SetCharacterLimit(20);
}

void EditorSavePopup::Draw()
{
    Renderer2D::BeginScene(m_UICamera);
    if (m_DisplaySavingPopup)
    {
        DrawSavingPopup();
    }
    Renderer2D::EndScene();
}

void EditorSavePopup::OnEvent(Event& event)
{
    bool isDisplayed = m_DisplaySavingPopup;
    m_SaveMapButton->OnEvent(event);
    m_CancelSaveButton->OnEvent(event);
    m_MapNameInputBox->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorSavePopup::OnKeyPressed));

    // Ignore non window events if saving popup is open
    if (event.GetCategory() != EventCategory::Window)
        event.Handled = isDisplayed;
}

void EditorSavePopup::DrawSavingPopup()
{
    Renderer2D::DrawQuad(glm::vec2(0), {1.4f, 1.0f}, {0.3f, 0.6f, 0.9f, 1.0f});

    static float textScale = 0.5f;
    static std::string saveText = "Input map file name";

    Renderer2D::DrawTextStr(saveText, {0.0f, 0.3f}, textScale, glm::vec3(0.9f), HTextAlign::MIDDLE);

    m_SaveMapButton->OnUpdate();
    m_CancelSaveButton->OnUpdate();
    m_MapNameInputBox->OnUpdate();
}

bool EditorSavePopup::OnKeyPressed(KeyPressedEvent& event)
{
    switch (event.GetKeyCode())
    {
        case GLFW_KEY_X:
        {
            if (!m_DisplaySavingPopup)
            {
                m_DisplaySavingPopup = true;
                m_MapNameInputBox->Clear();
                m_MapNameInputBox->SetFocused(true);
                EditorLayer::Get().GetCameraController()->ResetKeys();
            }

            return true;
        }
        case GLFW_KEY_ESCAPE:
        {
            ClosePopup();
            return true;
        }
        default:
            break;
    }

    return m_DisplaySavingPopup;
}

void EditorSavePopup::ClosePopup()
{
    m_DisplaySavingPopup = false;
    m_MapNameInputBox->Clear();
}

void EditorSavePopup::OnCancelSaveButtonPressed(ButtonCallbackData data)
{
    ClosePopup();
}

void EditorSavePopup::OnSaveMapButtonPressed(ButtonCallbackData data)
{
    if (SaveMap())
        ClosePopup();
}

void EditorSavePopup::OnMapNameInputBoxAccepted(InputBoxCallbackData data)
{
    if (SaveMap())
        ClosePopup();
}

bool EditorSavePopup::SaveMap()
{
    auto mapName = m_MapNameInputBox->GetText();

    if (mapName.empty())
    {
        LOG_WARN("EditorSavePopup: Empty map name");
        return false;
    }
    EditorLayer::Get().SaveMap(mapName);

    return true;
}
