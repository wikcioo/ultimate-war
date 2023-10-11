#pragma once

#include <memory>

#include "game/player_manager.h"
#include "ui/ui_element.h"

class Info : public UIElement
{
public:
    Info(const std::shared_ptr<OrthographicCamera>& UICamera,
         const std::shared_ptr<PlayerManager>& playerManager,
         const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    ~Info() = default;

    virtual void Draw() override;

private:
    std::shared_ptr<PlayerManager> m_PlayerManager;
};
