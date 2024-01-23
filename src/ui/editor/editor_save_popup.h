#pragma once

#include "ui/common/save_popup.h"

class EditorSavePopup : public SavePopup
{
public:
    EditorSavePopup(const std::shared_ptr<OrthographicCamera>& UICamera,
                    const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });

private:
    virtual bool OnKeyPressed(KeyPressedEvent& event) override;
    virtual bool OnSave() override;
};
