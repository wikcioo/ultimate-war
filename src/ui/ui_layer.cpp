#include "ui_layer.h"

#include "core/input.h"
#include "core/application.h"
#include "game/game_layer.h"
#include "graphics/renderer.h"
#include "ui/game/minimap.h"
#include "ui/game/shop_panel.h"
#include "ui/game/game_info.h"
#include "ui/game/game_save_popup.h"
#include "ui/editor/editor_info.h"
#include "ui/editor/editor_save_popup.h"
#include "widgets/notification.h"

std::shared_ptr<ConfirmPopup> UILayer::s_ConfirmPopup = nullptr;

UILayer::UILayer()
    : Layer("UILayer")
{
    auto window = Application::Get().GetWindow();
    m_UICamera = std::make_shared<OrthographicCamera>((float)window->GetWidth() / (float)window->GetHeight());

    UILayer::s_ConfirmPopup = std::make_shared<ConfirmPopup>(m_UICamera);

    Notification::Configure(m_UICamera);
}

void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
    m_UIElementStack.Clear();
}

void UILayer::OnUpdate(float dt)
{
    for (const auto& element : m_UIElementStack)
        element->Draw();

    Notification::OnUpdate(dt);
}

void UILayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(UILayer::OnWindowResized));

    Notification::OnEvent(event);

    for (auto it = m_UIElementStack.rbegin(); it != m_UIElementStack.rend(); it++)
        (*it)->OnEvent(event);
}

void UILayer::PushGameLayerElements()
{
    auto gameCamera = GameLayer::Get().GetCameraController()->GetCamera();
    float minimapHeight = 0.5f;
    m_UIElementStack.PushElement(
        std::make_shared<Minimap>(
            m_UICamera,
            gameCamera,
            GameLayer::Get().GetGameMapManager(),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(minimapHeight * MINIMAP_ASPECT_RATIO, minimapHeight)
        ));

    m_UIElementStack.PushElement(
        std::make_shared<ShopPanel>(
            m_UICamera,
            glm::vec2(m_UICamera->GetHalfOfRelativeWidth(), 0.0f)
        ));

    m_UIElementStack.PushElement(std::make_shared<GameInfo>(m_UICamera, GameLayer::Get().GetPlayerManager()));
    m_UIElementStack.PushElement(std::make_shared<GameSavePopup>(m_UICamera));
}

void UILayer::PushEditorLayerElements()
{
    m_UIElementStack.PushElement(std::make_shared<EditorInfo>(m_UICamera));
    m_UIElementStack.PushElement(std::make_shared<EditorSavePopup>(m_UICamera));
    m_UIElementStack.PushElement(UILayer::s_ConfirmPopup);
}

bool UILayer::OnWindowResized(WindowResizedEvent& event)
{
    m_UICamera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
    m_UICamera->SetScale(event.GetHeight() / INITIAL_RELATIVE_HEIGHT_IN_PIXELS);
    return false;
}
