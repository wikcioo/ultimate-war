#include "game_info.h"

#include <string>

#include "graphics/renderer.h"
#include "game/game_layer.h"

static std::string INFO_FONT = "rexlia";

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

    int earnedResourceNumbers[resourceData.NumResources] = { 0, 0, 0, 0 };

    float stride = m_Stride;
    bool isEarnedResourceInfoVisible = false;
    if (GameLayer::Get().IsEarnedResourcesInfoVisible())
    {
        stride += 0.12;
        isEarnedResourceInfoVisible = true;

        for (const auto& tile : currPlayer->GetOwnedTiles())
        {
            auto resources = tile->GetResources();
            earnedResourceNumbers[0] += resources.Wood;
            earnedResourceNumbers[1] += resources.Rock;
            earnedResourceNumbers[2] += resources.Steel;
            earnedResourceNumbers[3] += resources.Gold;
        }
    }

    glm::vec2 barPosition = { 0.0f, halfOfHeight - m_BarHeight / 2.0f };
    glm::vec2 barSize = { halfOfWidth * 2.0f, m_BarHeight };
    Renderer2D::DrawQuad(barPosition, barSize, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));

    for (int i = 0; i < resourceData.NumResources; i++)
    {
        float leftXPosition = -stride * 2.0f;
        Renderer2D::DrawQuad(
            {
                leftXPosition + (i * stride),
                barPosition.y
            },
            glm::vec2(m_ResourceScale * resourceData.ResourceTextureScales[i]),
            resourceData.ResourceTextures[i]
        );

        std::string resourceText = std::to_string(resourceNumbers[i]);
        if (isEarnedResourceInfoVisible)
        {
            std::ostringstream oss;
            oss << std::to_string(resourceNumbers[i] + earnedResourceNumbers[i]);
            oss << "[+" << earnedResourceNumbers[i] << "]";
            resourceText = oss.str();
        }

        Renderer2D::DrawTextStr(
            resourceText,
            {
                leftXPosition + m_ResourceScale / 2.0f + m_ResourceNumberOffset + (i * stride),
                barPosition.y
            },
            m_TextScale,
            isEarnedResourceInfoVisible ? glm::vec3(0.1f, 0.8f, 0.2f) : resourceData.ResourceNumberColors[i],
            HTextAlign::LEFT,
            VTextAlign::MIDDLE,
            INFO_FONT
        );
    }

    std::string turn   = "Turn: "   + std::to_string(GameLayer::Get().GetIteration());
    std::string player = "Player: " + currPlayer->GetName();

    Renderer2D::DrawTextStr(
        turn,
        {
            barPosition.x + barSize.x / 2.0f - 0.05f,
            barPosition.y
        },
        m_TextScale,
        glm::vec3(0.9f),
        HTextAlign::RIGHT,
        VTextAlign::MIDDLE,
        INFO_FONT
    );

    Renderer2D::DrawTextStr(
        player,
        {
            barPosition.x - barSize.x / 2.0f + 0.05f,
            barPosition.y
        },
        m_TextScale,
        currPlayer->GetColor(),
        HTextAlign::LEFT,
        VTextAlign::MIDDLE,
        INFO_FONT
    );

    if(!GameLayer::Get().IsGameActive())
    {
        Renderer2D::DrawTextStr(
            "Game Over",
            { 0.0f, 0.0f },
            1.0f,
            { 0.95f, 0.7f, 0.5f },
            HTextAlign::MIDDLE,
            VTextAlign::MIDDLE,
            INFO_FONT
        );
    }

    Renderer2D::EndScene();
}
