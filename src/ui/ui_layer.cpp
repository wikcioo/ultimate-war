#include "ui_layer.h"

#include "core/core.h"
#include "core/input.h"
#include "core/application.h"
#include "game/game_layer.h"

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
    for (auto element : m_UIElements)
        element->Draw();
}

void UILayer::OnEvent(Event& event)
{
    for (auto element : m_UIElements)
        element->OnEvent(event);
}
