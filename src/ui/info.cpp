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

    auto currPlayer = m_PlayerManager->GetCurrentPlayer();
    auto halfOfWidth = m_UICamera->GetHalfOfRelativeWidth();
    auto halfOfHeight = m_UICamera->GetHalfOfRelativeHeight();

    const int numResources = 4;

    int resourceNumbers[numResources] = {
        currPlayer->GetResources().Wood,
        currPlayer->GetResources().Rock,
        currPlayer->GetResources().Steel,
        currPlayer->GetResources().Gold
    };

    glm::vec3 resourceNumberColors[numResources] = {
        ColorData::Get().Resources.Wood,
        ColorData::Get().Resources.Rock,
        ColorData::Get().Resources.Steel,
        ColorData::Get().Resources.Gold
    };

    std::shared_ptr<Texture2D> resourceTextures[numResources] = {
        m_WoodTexture,
        m_RockTexture,
        m_SteelTexture,
        m_GoldTexture
    };

    for (int i = 0; i < numResources; i++)
    {
        Renderer2D::DrawQuad(
            { -halfOfWidth + m_ResourceStartX, halfOfHeight - m_ResourceStartY - m_ResourceOffset * i},
            glm::vec2(m_ResourceScale),
            resourceTextures[i]
        );

        Renderer2D::DrawTextStr(
            std::to_string(resourceNumbers[i]),
            {
                -halfOfWidth + m_ResourceStartX + m_ResourceNumberOffset,
                halfOfHeight - (m_ResourceStartY + 0.025f) - m_ResourceOffset * i
            },
            m_TextScale,
            resourceNumberColors[i],
            HTextAlign::LEFT
        );
    }

    std::string turn   = "Turn: "   + std::to_string(GameLayer::Get().GetIteration());
    std::string player = "Player: " + currPlayer->GetName();

    Renderer2D::DrawTextStr(turn, {  halfOfWidth - 0.05f, halfOfHeight - 0.1f }, m_TextScale, glm::vec3(0.9f), HTextAlign::RIGHT);
    Renderer2D::DrawTextStr(player, { 0.0f, halfOfHeight - 0.1f }, m_TextScale, currPlayer->GetColor(), HTextAlign::MIDDLE);

    Renderer2D::EndScene();
}
