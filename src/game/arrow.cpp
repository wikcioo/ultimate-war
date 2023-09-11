#include "arrow.h"

#include <glm/gtc/matrix_transform.hpp>

#include "core/resource_manager.h"
#include "graphics/renderer.h"

Arrow::Arrow(const glm::vec4& color, float thickness, float tipLength)
    : m_Color(color), m_StartPosition({0.0f, 0.0f}), m_EndPosition({0.0f, 0.0f}),
      m_Thickness(thickness), m_TipLength(tipLength), m_Visible(false)
{
    float arrowVertices[7 * 2] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,

         1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f, -1.0f
    };

    unsigned int arrowIndices[3 * 3] = {
        0, 1, 2,
        1, 3, 2,
        4, 5, 6
    };

    auto arrowVB = std::make_shared<VertexBuffer>(arrowVertices, sizeof(arrowVertices), GL_DYNAMIC_DRAW);
    auto arrowIB = std::make_shared<IndexBuffer>(arrowIndices, sizeof(arrowIndices) / sizeof(unsigned int));
    std::vector<int> arrowLayout = {2};

    m_ArrowVA = std::make_shared<VertexArray>(arrowVB, arrowIB, arrowLayout);
}

float* Arrow::GetNewArrowData()
{
    float shaftSlope = (m_EndPosition.y - m_StartPosition.y) / (m_EndPosition.x - m_StartPosition.x);
    float arrowAngle = glm::atan(shaftSlope);
    /*
          adx
         _ _ _
        |   a/ = arrowAngle
        |   /
    ady |  / m_TipLength
        | /
        |/
    */
    float adx = glm::cos(arrowAngle) * m_TipLength;
    float ady = glm::sin(arrowAngle) * m_TipLength;

    glm::vec2 shaftEndPosition = m_EndPosition;
    if (shaftEndPosition.x < m_StartPosition.x)
    {
        shaftEndPosition.x += adx;
        shaftEndPosition.y += ady;
    }
    else
    {
        shaftEndPosition.x -= adx;
        shaftEndPosition.y -= ady;
    }

    float m = shaftSlope * -1;
    float bStart = m_StartPosition.y - (m * m_StartPosition.x);
    float bEnd = shaftEndPosition.y - (m * shaftEndPosition.x);

    float angle = glm::atan(m);
    glm::vec2 dir_vec = glm::vec2(glm::sin(angle), glm::cos(angle));

    static float halfThickness = m_Thickness / 2;
    glm::vec2 startShaftVertex1 = m_StartPosition  + halfThickness * -dir_vec;
    glm::vec2 startShaftVertex2 = m_StartPosition  + halfThickness *  dir_vec;
    glm::vec2 endShaftVertex1   = shaftEndPosition + halfThickness * -dir_vec;
    glm::vec2 endShaftVertex2   = shaftEndPosition + halfThickness *  dir_vec;

    glm::vec2 arrowVertex1 = shaftEndPosition + (m_Thickness) * -dir_vec;
    glm::vec2 arrowVertex2 = shaftEndPosition + (m_Thickness) *  dir_vec;

    float* data = new float[7 * 2] {
        startShaftVertex1.x, startShaftVertex1.y,
        startShaftVertex2.x, startShaftVertex2.y,
        endShaftVertex1.x, endShaftVertex1.y,
        endShaftVertex2.x, endShaftVertex2.y,

        arrowVertex1.x, arrowVertex1.y,
        arrowVertex2.x, arrowVertex2.y,
        m_EndPosition.x, m_EndPosition.y
    };

    return data;
}

void Arrow::Update()
{
    if (!m_Visible) return;

    float* data = GetNewArrowData();
    m_ArrowVA->GetVertexBuffer()->Bind();
    m_ArrowVA->GetVertexBuffer()->UpdateData(data, 0, 14 * sizeof(float));
    delete data;

    Renderer2D::DrawGeometry(m_ArrowVA, glm::vec3(0.0f), glm::vec2(1.0f), m_Color);
}
