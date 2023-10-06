#include "ui_layer.h"

#include "core/core.h"
#include "core/input.h"
#include "core/application.h"
#include "game/game_layer.h"
#include "graphics/renderer.h"

UILayer::UILayer()
    : Layer("UILayer")
{
    m_GameCamera = GameLayer::Get().GetCameraController()->GetCamera();
    m_UICamera = std::make_shared<OrthographicCamera>(m_GameCamera->GetAspectRatio());

    m_UIElements.emplace_back(std::make_shared<Minimap>(m_UICamera, m_GameCamera, GameLayer::Get().GetGameMapManager()));
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
                          { 0.95, 0.7, 0.5 }, TextAlignment::MIDDLE);
        Renderer2D::EndScene();
    }
    for (auto element : m_UIElements)
        element->Draw();
}

void UILayer::OnEvent(Event& event)
{
    for (auto element : m_UIElements)
        element->OnEvent(event);
}
