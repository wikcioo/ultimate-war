#include "info.h"

#include <string>

#include "graphics/renderer.h"
#include "game/game_layer.h"

Info::Info(const std::shared_ptr<OrthographicCamera>& UICamera,
           const std::shared_ptr<PlayerManager>& playerManager,
           const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * UICamera->GetAspectRatio(), size.y}),
      m_PlayerManager(playerManager)
{
}

void Info::Draw()
{
    Renderer2D::BeginScene(m_UICamera);

    float scale = 0.6f;
    auto currPlayer = m_PlayerManager->GetCurrentPlayer();
    auto aspectRatio = m_UICamera->GetAspectRatio();

    std::string gold = "Gold: " + std::to_string(currPlayer->GetGold());
    std::string turn = "Turn: " + std::to_string(GameLayer::Get().GetIteration());
    std::string player = "Player: " + currPlayer->GetName();

    Renderer2D::DrawTextStr(gold, { -aspectRatio + 0.1f, 0.9f }, scale, currPlayer->GetColor(), TextAlignment::LEFT);
    Renderer2D::DrawTextStr(turn, { aspectRatio - 0.1f, 0.9f }, scale, currPlayer->GetColor(), TextAlignment::RIGHT);
    Renderer2D::DrawTextStr(player, { 0.0f, 0.9f }, scale, currPlayer->GetColor(), TextAlignment::MIDDLE);

    Renderer2D::EndScene();
}
