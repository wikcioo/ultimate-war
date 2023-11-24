#include "game_info.h"

#include <string>

#include "graphics/renderer.h"
#include "game/game_layer.h"

GameInfo::GameInfo(const std::shared_ptr<OrthographicCamera>& UICamera,
           const std::shared_ptr<PlayerManager>& playerManager,
           const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * UICamera->GetAspectRatio(), size.y}),
      m_PlayerManager(playerManager)
{
}

void GameInfo::Draw()
{
    Renderer2D::BeginScene(m_UICamera);

    auto currPlayer = m_PlayerManager->GetCurrentPlayer();
    auto halfOfWidth = m_UICamera->GetHalfOfRelativeWidth();
    auto halfOfHeight = m_UICamera->GetHalfOfRelativeHeight();

    static auto resourceData = Resources::GetResourceData();

    int resourceNumbers[resourceData.NumResources] = {
        currPlayer->GetResources().Wood,
        currPlayer->GetResources().Rock,
        currPlayer->GetResources().Steel,
        currPlayer->GetResources().Gold
    };

    for (int i = 0; i < resourceData.NumResources; i++)
    {
        Renderer2D::DrawQuad(
            { -halfOfWidth + m_ResourceStartX, halfOfHeight - m_ResourceStartY - m_ResourceOffset * i},
            glm::vec2(m_ResourceScale),
            resourceData.ResourceTextures[i]
        );

        Renderer2D::DrawTextStr(
            std::to_string(resourceNumbers[i]),
            {
                -halfOfWidth + m_ResourceStartX + m_ResourceNumberOffset,
                halfOfHeight - (m_ResourceStartY + 0.025f) - m_ResourceOffset * i
            },
            m_TextScale,
            resourceData.ResourceNumberColors[i],
            HTextAlign::LEFT
        );
    }

    std::string turn   = "Turn: "   + std::to_string(GameLayer::Get().GetIteration());
    std::string player = "Player: " + currPlayer->GetName();

    Renderer2D::DrawTextStr(turn, {  halfOfWidth - 0.05f, halfOfHeight - 0.1f }, m_TextScale, glm::vec3(0.9f), HTextAlign::RIGHT);
    Renderer2D::DrawTextStr(player, { 0.0f, halfOfHeight - 0.1f }, m_TextScale, currPlayer->GetColor(), HTextAlign::MIDDLE);

    if(!GameLayer::Get().IsGameActive())
    {
        Renderer2D::DrawTextStr("Game Over", { 0.0f, 0.0f }, 1.0f,
                          { 0.95, 0.7, 0.5 }, HTextAlign::MIDDLE);
    }

    Renderer2D::EndScene();
}
