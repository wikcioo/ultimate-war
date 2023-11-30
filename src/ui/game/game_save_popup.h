#pragma once

#include "ui/common/save_popup.h"

class GameSavePopup : public SavePopup
{
public:
    GameSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera,
                    const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });

private:
    virtual bool OnSave() override;
};
