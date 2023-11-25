#pragma once

#include <memory>

#include "ui/ui_element.h"

class EditorInfo : public UIElement
{
public:
    EditorInfo(const std::shared_ptr<OrthographicCamera>& UICamera,
         const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 0.5f });
    ~EditorInfo() = default;

    virtual void Draw() override;

private:
};
