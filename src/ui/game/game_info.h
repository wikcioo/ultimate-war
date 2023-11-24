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
    std::shared_ptr<PlayerManager> m_PlayerManager;

    float m_TextScale = 0.3f;
    float m_ResourceScale = 0.08f;
    float m_ResourceOffset = 0.1f;
    float m_ResourceNumberOffset = 0.07f;
    float m_ResourceStartX = 0.07f;
    float m_ResourceStartY = 0.08f;
};
