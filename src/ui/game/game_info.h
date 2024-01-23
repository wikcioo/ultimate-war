#pragma once

#include <memory>

#include "game/player_manager.h"
#include "ui/ui_element.h"
#include "core/resource_manager.h"

class GameInfo : public UIElement
{
public:
    GameInfo(const std::shared_ptr<OrthographicCamera>& UICamera,
         const std::shared_ptr<PlayerManager>& playerManager,
         const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    ~GameInfo() = default;

    virtual void Draw() override;

private:
    void DrawTopBar();
    void DrawGameOverAndLeaderboard();
    void DrawBuildingUpgradeInfo();

private:
    std::shared_ptr<PlayerManager> m_PlayerManager;

    float m_BarHeight = 0.08f;
    float m_TextScale = 0.25f;
    float m_ResourceScale = 0.07f;
    float m_ResourceNumberOffset = 0.008f;
    float m_Stride = 0.27f;
};
