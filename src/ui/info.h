#pragma once

#include <memory>

#include "game/player_manager.h"
#include "ui/ui_element.h"
#include "core/resource_manager.h"

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

    float m_TextScale = 0.3f;
    float m_ResourceScale = 0.08f;
    float m_ResourceOffset = 0.1f;
    float m_ResourceNumberOffset = 0.07f;
    float m_ResourceStartX = 0.07f;
    float m_ResourceStartY = 0.08f;

    std::shared_ptr<Texture2D> m_WoodTexture = ResourceManager::GetTexture("wood");
    std::shared_ptr<Texture2D> m_RockTexture = ResourceManager::GetTexture("rock");
    std::shared_ptr<Texture2D> m_SteelTexture = ResourceManager::GetTexture("steel");
    std::shared_ptr<Texture2D> m_GoldTexture = ResourceManager::GetTexture("gold");
};
