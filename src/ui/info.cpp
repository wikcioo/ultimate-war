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
    auto halfOfWidth = m_UICamera->GetHalfOfRelativeWidth();
    auto halfOfHeight = m_UICamera->GetHalfOfRelativeHeight();

    std::string wood   = "Wood: "   + std::to_string(currPlayer->GetResources().Wood);
    std::string stone  = "Rock: "   + std::to_string(currPlayer->GetResources().Rock);
    std::string iron   = "Steel: "  + std::to_string(currPlayer->GetResources().Steel);
    std::string gold   = "Gold: "   + std::to_string(currPlayer->GetResources().Gold);
    std::string turn   = "Turn: "   + std::to_string(GameLayer::Get().GetIteration());
    std::string player = "Player: " + currPlayer->GetName();

    Renderer2D::DrawTextStr(wood,   { -halfOfWidth + 0.05f, halfOfHeight - 0.1f }, scale, ColorData::Get().Resources.Wood, TextAlignment::LEFT);
    Renderer2D::DrawTextStr(stone,  { -halfOfWidth + 0.05f, halfOfHeight - 0.2f }, scale, ColorData::Get().Resources.Rock, TextAlignment::LEFT);
    Renderer2D::DrawTextStr(iron,   { -halfOfWidth + 0.05f, halfOfHeight - 0.3f }, scale, ColorData::Get().Resources.Steel, TextAlignment::LEFT);
    Renderer2D::DrawTextStr(gold,   { -halfOfWidth + 0.05f, halfOfHeight - 0.4f }, scale, ColorData::Get().Resources.Gold, TextAlignment::LEFT);
    Renderer2D::DrawTextStr(turn,   {  halfOfWidth - 0.05f, halfOfHeight - 0.1f }, scale, glm::vec3(0.9f), TextAlignment::RIGHT);
    Renderer2D::DrawTextStr(player, { 0.0f, halfOfHeight - 0.1f }, scale, currPlayer->GetColor(), TextAlignment::MIDDLE);

    Renderer2D::EndScene();
}
