#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "graphics/shader.h"
#include "graphics/vertex_array.h"

class Arrow
{
public:
    Arrow(const glm::vec4& color = {0.0f, 1.0f, 1.0f, 1.0f}, float thickness = 0.1f, float tipLength = 0.2f);
    ~Arrow() = default;

    void Draw();

    void SetStartPosition(const glm::vec2& startPosition) { m_StartPosition = startPosition; }
    void SetEndPosition(const glm::vec2& endPosition) { m_EndPosition = endPosition; }
    void SetVisible(bool visible) { m_Visible = visible; }
    void SetColor(const glm::vec4& color) { m_Color = color; }

    inline const bool IsVisible() const { return m_Visible; }

private:
    float* GetNewArrowData();

private:
    std::shared_ptr<VertexArray> m_ArrowVA;
    glm::vec2 m_StartPosition;
    glm::vec2 m_EndPosition;
    glm::vec4 m_Color;
    float m_Thickness;
    float m_TipLength;
    bool m_Visible;
};
