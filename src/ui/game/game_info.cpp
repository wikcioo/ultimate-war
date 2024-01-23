#include "game_info.h"

#include <string>
#include <algorithm>

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

    DrawTopBar();
    DrawBuildingUpgradeInfo();

    if(!GameLayer::Get().IsGameActive())
    {
        DrawGameOverAndLeaderboard();
    }

    Renderer2D::EndScene();
}

void GameInfo::DrawTopBar()
{
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
}

void GameInfo::DrawGameOverAndLeaderboard()
{
    auto halfOfWidth = m_UICamera->GetHalfOfRelativeWidth();
    auto halfOfHeight = m_UICamera->GetHalfOfRelativeHeight();

    // Draw dimmed background
    Renderer2D::DrawQuad(
        glm::vec2(0.0f),
        { halfOfWidth * 2.0f, halfOfHeight * 2.0f },
        glm::vec4(0.0f, 0.0f, 0.0f, 0.8f)
    );

    Renderer2D::DrawTextStr(
        "Game Over",
        { 0.0f, halfOfHeight - 0.18f },
        0.7f,
        { 0.95f, 0.7f, 0.5f },
        HTextAlign::MIDDLE,
        VTextAlign::TOP,
        INFO_FONT
    );

    glm::vec2 leaderboardPosition = { 0.0f, -0.2f };
    glm::vec2 leaderboardSize = { 1.2f, halfOfHeight * 2.0f - 0.6f };

    glm::vec2 leaderboardTextPosition = {
        leaderboardPosition.x,
        leaderboardPosition.y + leaderboardSize.y / 2.0f - 0.05f
    };

    static auto confettiTexture = ResourceManager::GetTexture("confetti");
    static float confettiSize = 0.1f;
    static float confettiXOffset = 0.5f;

    Renderer2D::DrawQuad(
        {
            leaderboardTextPosition.x - confettiXOffset,
            leaderboardTextPosition.y
        },
        glm::vec2(confettiSize),
        confettiTexture
    );

    Renderer2D::DrawQuad(
        {
            leaderboardTextPosition.x + confettiXOffset,
            leaderboardTextPosition.y
        },
        glm::vec2(confettiSize),
        confettiTexture
    );

    Renderer2D::DrawTextStr(
        "Leaderboard",
        leaderboardTextPosition,
        0.45f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::MIDDLE,
        INFO_FONT
    );

    auto playerManager = GameLayer::Get().GetPlayerManager();
    auto defeatOrder = playerManager->GetDefeatOrder();
    auto players = playerManager->GetAllPlayers();

    glm::vec2 pos = {
        leaderboardPosition.x - leaderboardSize.x / 2.0f + 0.1f,
        leaderboardPosition.y + leaderboardSize.y / 2.0f - 0.3f
    };


    for (int i = defeatOrder.size() - 1; i >= 0; i--)
    {
        Renderer2D::DrawTextStr(
            std::to_string(defeatOrder.size() - i) + ".",
            {
                pos.x + 0.1f,
                pos.y
            },
            0.3f,
            glm::vec3(1.0f),
            HTextAlign::RIGHT,
            VTextAlign::MIDDLE,
            INFO_FONT
        );

        Renderer2D::DrawTextStr(
            defeatOrder[i]->GetName(),
            {
                pos.x + 0.13f,
                pos.y
            },
            0.3f,
            glm::vec3(1.0f),
            HTextAlign::LEFT,
            VTextAlign::MIDDLE,
            INFO_FONT
        );

        pos.y -= 0.1f;
    }
}

void GameInfo::DrawBuildingUpgradeInfo()
{
    static float offset = 0.02f;
    static glm::vec2 size = glm::vec2(0.42f, 0.25f);

    auto halfOfWidth = m_UICamera->GetHalfOfRelativeWidth();
    auto halfOfHeight = m_UICamera->GetHalfOfRelativeHeight();

    glm::vec2 position = {
        -halfOfWidth + size.x / 2.0f + offset,
        halfOfHeight - m_BarHeight - size.y / 2.0f - offset
    };

    auto info = GameLayer::Get().GetBuildingUpgradeInfo();
    if (info.Show)
    {
        // draw background
        Renderer2D::DrawQuad(position, size, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));

        // draw text
        Renderer2D::DrawTextStr(
            "Upgrade cost",
            { position.x, position.y + size.y / 2.0f - 0.03f },
            0.2f,
            glm::vec3(1.0f),
            HTextAlign::MIDDLE,
            VTextAlign::TOP,
            "rexlia"
        );

        // draw resources
        Resources::Draw2x2(
            info._Building->GetUpgradeCost(),
            { position.x, position.y + 0.02f }
        );
    }
}
