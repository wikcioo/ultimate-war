#include "ui_layer.h"

#include "core/input.h"
#include "core/application.h"
#include "game/game_layer.h"
#include "graphics/renderer.h"
#include "ui/game/minimap.h"
#include "ui/game/shop_panel.h"
#include "ui/game/game_info.h"

UILayer::UILayer()
    : Layer("UILayer")
{
    m_GameCamera = GameLayer::Get().GetCameraController()->GetCamera();
    m_UICamera = std::make_shared<OrthographicCamera>(m_GameCamera->GetAspectRatio());

    float minimapHeight = 0.5f;
    m_UIElements.emplace_back(
        std::make_shared<Minimap>(
            m_UICamera,
            m_GameCamera,
            GameLayer::Get().GetGameMapManager(),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(minimapHeight * MINIMAP_ASPECT_RATIO, minimapHeight)
        ));

    m_UIElements.emplace_back(
        std::make_shared<ShopPanel>(
            m_UICamera,
            glm::vec2(m_UICamera->GetHalfOfRelativeWidth(), 0.0f)
        ));

    m_UIElements.emplace_back(std::make_shared<GameInfo>(m_UICamera, GameLayer::Get().GetPlayerManager()));
}

void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
}

void UILayer::OnUpdate(float dt)
{
    for (auto element : m_UIElements)
        element->Draw();
}

void UILayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(UILayer::OnWindowResized));

    for (auto element : m_UIElements)
        element->OnEvent(event);
}

bool UILayer::OnWindowResized(WindowResizedEvent& event)
{
    m_UICamera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
    m_UICamera->SetScale(event.GetHeight() / INITIAL_RELATIVE_HEIGHT_IN_PIXELS);
    return false;
}
