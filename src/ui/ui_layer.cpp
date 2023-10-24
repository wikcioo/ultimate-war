#include "ui_layer.h"

#include "core/core.h"
#include "core/input.h"
#include "core/application.h"
#include "game/game_layer.h"
#include "graphics/renderer.h"
#include "ui/minimap.h"
#include "ui/shop_panel.h"
#include "ui/info.h"

UILayer::UILayer()
    : Layer("UILayer")
{
    m_GameCamera = GameLayer::Get().GetCameraController()->GetCamera();
    m_UICamera = std::make_shared<OrthographicCamera>(m_GameCamera->GetAspectRatio());

    m_UIElements.emplace_back(
        std::make_shared<Minimap>(
            m_UICamera,
            m_GameCamera,
            GameLayer::Get().GetGameMapManager(),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.5f * m_GameCamera->GetAspectRatio(), 0.5f)
        ));

    m_UIElements.emplace_back(
        std::make_shared<ShopPanel>(
            m_UICamera,
            glm::vec2(0.5f * m_GameCamera->GetAspectRatio() + 0.4f, 0.0f)
        ));

    m_UIElements.emplace_back(std::make_shared<Info>(m_UICamera, GameLayer::Get().GetPlayerManager()));
}

void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
}

void UILayer::OnUpdate(float dt)
{
    if(!GameLayer::Get().IsGameActive())
    {
        Renderer2D::BeginScene(m_UICamera);
        Renderer2D::DrawTextStr("Game Over", { 0.0f, 0.0f }, 1.0f,
                          { 0.95, 0.7, 0.5 }, HTextAlign::MIDDLE);
        Renderer2D::EndScene();
    }
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
