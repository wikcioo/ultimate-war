#include "renderer.h"

#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "util/util.h"
#include "game/tile.h"
#include "core/logger.h"
#include "core/resource_manager.h"

Renderer2D::Renderer2DData* Renderer2D::s_Data = new Renderer2DData();

void Renderer2D::Init()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    float quadVertices[4 * 5] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    unsigned int quadIndices[6] = {
        0, 2, 3,
        0, 1, 2
    };

    std::vector<int> quadLayout = {3, 2};
    auto quadVB = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
    auto quadIB = std::make_shared<IndexBuffer>(quadIndices, sizeof(quadIndices) / sizeof(unsigned int));
    s_Data->QuadVertexArray = std::make_shared<VertexArray>(quadVB, quadIB, quadLayout);

    float fontVertices[4 * 2] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };

    unsigned int fontIndices[6] = {
        0, 1, 2,
        2, 3, 1
    };

    std::vector<int> fontLayout = {2};
    auto fontVB = std::make_shared<VertexBuffer>(fontVertices, sizeof(fontVertices));
    auto fontIB = std::make_shared<IndexBuffer>(fontIndices, sizeof(fontIndices) / sizeof(unsigned int));
    s_Data->FontVertexArray = std::make_shared<VertexArray>(fontVB, fontIB, fontLayout);

    float h = glm::sqrt(3) / 2;
    float hexagonVertices[6 * 3] = {
        -1.0f, 0.0f, 0.0f,
        -0.5f,    h, 0.0f,
         0.5f,    h, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.5f,   -h, 0.0f,
        -0.5f,   -h, 0.0f
    };

    unsigned int hexagonIndices[4 * 3] = {
        0, 2, 1,
        0, 3, 2,
        0, 5, 3,
        5, 4, 3
    };

    std::vector<int> hexagonLayout = {3};
    auto hexagonVB = std::make_shared<VertexBuffer>(hexagonVertices, sizeof(hexagonVertices));
    auto hexagonIB = std::make_shared<IndexBuffer>(hexagonIndices, sizeof(hexagonIndices) / sizeof(unsigned int));
    s_Data->HexagonVertexArray = std::make_shared<VertexArray>(hexagonVB, hexagonIB, hexagonLayout);

    s_Data->FlatColorShader = ResourceManager::GetShader("color");
    s_Data->TextureShader = ResourceManager::GetShader("texture");
    s_Data->FontShader = ResourceManager::GetShader("font");
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::BeginScene(const std::shared_ptr<OrthographicCamera>& camera)
{
    s_Data->Camera = camera;

    s_Data->FlatColorShader->Bind();
    s_Data->FlatColorShader->SetMat4("u_ProjectionView", camera->GetProjectionViewMatrix());

    s_Data->TextureShader->Bind();
    s_Data->TextureShader->SetMat4("u_ProjectionView", camera->GetProjectionViewMatrix());

    s_Data->FontShader->Bind();
    s_Data->FontShader->SetMat4("u_ProjectionView", camera->GetProjectionViewMatrix());
}

void Renderer2D::EndScene()
{
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, std::optional<float> borderThickness)
{
    DrawQuad(glm::vec3(position, 0.0f), size, color, borderThickness);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, std::optional<float> borderThickness)
{
    DrawGeometry(s_Data->QuadVertexArray, position, size, color, borderThickness);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& color)
{
    DrawQuad(glm::vec3(position, 0.0f), size, texture, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& color)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

    texture->Bind(0);
    s_Data->TextureShader->Bind();
    s_Data->TextureShader->SetInt("u_Texture", 0);
    s_Data->TextureShader->SetMat4("u_Model", model);
    s_Data->TextureShader->SetFloat4("u_Color", color);

    s_Data->QuadVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, s_Data->QuadVertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::DrawHexagon(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, std::optional<float> borderThickness)
{
    DrawHexagon(glm::vec3(position, 0.0f), size, color, borderThickness);
}

void Renderer2D::DrawHexagon(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, std::optional<float> borderThickness)
{
    DrawGeometry(s_Data->HexagonVertexArray, position, size, color, borderThickness);
}

void Renderer2D::DrawHexagon(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Shader>& shader)
{
    DrawHexagon(glm::vec3(position, 0.0f), size, shader);
}

void Renderer2D::DrawHexagon(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Shader>& shader)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
    auto relBtmLeft = glm::vec2(position.x - TILE_WIDTH / 4.0f, position.y - TILE_HEIGHT / 4.0f) - s_Data->Camera->CalculateRelativeBottomLeftPosition();
    auto pxBtmLeft = s_Data->Camera->ConvertRelativeSizeToPixel(relBtmLeft);

    s_Data->HexagonVertexArray->Bind();

    shader->Bind();
    shader->SetMat4("u_ProjectionView", s_Data->Camera->GetProjectionViewMatrix());
    shader->SetMat4("u_Model", model);
    shader->SetFloat("u_Time", (float)glfwGetTime());
    shader->SetFloat2("u_BottomLeftPx", pxBtmLeft);
    shader->SetFloat2("u_SizePx", s_Data->Camera->ConvertRelativeSizeToPixel({TILE_WIDTH, TILE_HEIGHT}));

    glDrawElements(GL_TRIANGLES, s_Data->HexagonVertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::DrawGeometry(const std::shared_ptr<VertexArray>& vertexArray, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, std::optional<float> borderThickness)
{
    glm::mat4 fullScaleModel = glm::translate(glm::mat4(1.0f), glm::vec3(position)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
    vertexArray->Bind();
    s_Data->FlatColorShader->Bind();

    if (!borderThickness.has_value())
    {
        s_Data->FlatColorShader->SetMat4("u_Model", fullScaleModel);
        s_Data->FlatColorShader->SetFloat4("u_Color", color);
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        float thickness = borderThickness.value();
        if (thickness < 0.0f || thickness > 100.0f)
        {
            LOG_WARN("Renderer2D::DrawGeometry: borderThickness parameter outside of 0-100 bound");
            thickness = Util::Clamp<float>(thickness, 0.0f, 100.0f);
        }

        float ratio = (100.0f - thickness) / 100.0f;
        float outerHeight = size.y;
        float innerHeight = ratio * size.y;
        float heightDiff = outerHeight - innerHeight;
        float innerWidth = size.x - heightDiff;

        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        glm::mat4 scaledDownModel = glm::translate(glm::mat4(1.0f), glm::vec3(position)) * glm::scale(glm::mat4(1.0f), glm::vec3(innerWidth, innerHeight, 1.0f));
        s_Data->FlatColorShader->SetMat4("u_Model", scaledDownModel);
        s_Data->FlatColorShader->SetFloat4("u_Color", glm::vec4(0.0f));
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        s_Data->FlatColorShader->SetMat4("u_Model", fullScaleModel);
        s_Data->FlatColorShader->SetFloat4("u_Color", color);
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }
}

void Renderer2D::DrawTextStr(const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color,
                             HTextAlign hAlign, VTextAlign vAlign, const std::string& fontName)
{
    DrawTextStr(text, position, scale, glm::vec4(color, 1.0f), hAlign, vAlign, fontName);
}

void Renderer2D::DrawTextStr(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color,
                             HTextAlign hAlign, VTextAlign vAlign, const std::string& fontName)
{
    glm::vec2 pos_cpy = { position.x, position.y };

    s_Data->FontShader->Bind();

    auto font = ResourceManager::GetFont(fontName);
    auto characters = font->GetCharacters();

    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;

    while (std::getline(iss, line, '\n'))
        lines.push_back(line);

    float relCharHeight = s_Data->Camera->ConvertPixelSizeToRelative(characters['A'].Size.y) * scale;
    float relSpacing = relCharHeight * 0.3f;

    switch (vAlign)
    {
        case VTextAlign::BOTTOM:
            pos_cpy.y += (relCharHeight + relSpacing) * (lines.size() - 1);
            break;
        case VTextAlign::MIDDLE:
            pos_cpy.y += ((relCharHeight / 2.0f) * (lines.size() - 2.0f)) + (relSpacing * ((lines.size() - 1.0f) / 2.0f));
            break;
        case VTextAlign::TOP:
            pos_cpy.y -= relCharHeight;
            break;
    }

    for (const auto& line : lines)
    {
        // Determine horizontal length of a line
        float lineLength = 0.0f;
        for (std::string::const_iterator it = line.begin(); it != line.end(); it++)
        {
            auto c = characters[*it];
            lineLength += s_Data->Camera->ConvertPixelSizeToRelative(c.Advance >> 6) * scale;
        }

        switch (hAlign)
        {
            case HTextAlign::LEFT:
                break;
            case HTextAlign::MIDDLE:
                pos_cpy.x -= lineLength / 2;
                break;
            case HTextAlign::RIGHT:
                pos_cpy.x -= lineLength;
                break;
        }

        for (std::string::const_iterator c = line.begin(); c != line.end(); c++)
        {
            Font::Character ch = characters[*c];

            if (*c == ' ')
            {
                pos_cpy.x += s_Data->Camera->ConvertPixelSizeToRelative(ch.Advance >> 6) * scale;
            }
            else
            {
                float xpos = pos_cpy.x + s_Data->Camera->ConvertPixelSizeToRelative(ch.Bearing.x) * scale;
                float ypos = pos_cpy.y - s_Data->Camera->ConvertPixelSizeToRelative(ch.Size.y - ch.Bearing.y) * scale;
                glm::vec2 chRelSize = s_Data->Camera->ConvertPixelSizeToRelative(glm::vec2(ch.Size)) * scale;
                glm::mat4 chModel = glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, 0.0f)) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(chRelSize.x, chRelSize.y, 1.0f));

                ch.Texture->Bind(0);
                s_Data->FontShader->SetInt("u_Texture", 0);
                s_Data->FontShader->SetMat4("u_Model", chModel);
                s_Data->FontShader->SetFloat4("u_Color", color);

                s_Data->FontVertexArray->Bind();
                s_Data->FontVertexArray->GetIndexBuffer()->Bind();
                glDrawElements(GL_TRIANGLES, s_Data->FontVertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

                pos_cpy.x += s_Data->Camera->ConvertPixelSizeToRelative(ch.Advance >> 6) * scale;
            }
        }

        pos_cpy.x = position.x;
        pos_cpy.y -= relCharHeight + relSpacing;
    }
}

glm::vec2 Renderer2D::GetTextSize(const std::shared_ptr<OrthographicCamera>& camera, const std::string& text,
                                  const std::string& fontName)
{
    auto characters = ResourceManager::GetFont(fontName)->GetCharacters();

    float maxCharHeightPx = 0.0f;
    float textWidth = 0.0f;

    for (std::string::const_iterator c = text.begin(); c != text.end(); c++)
    {
        float charHeightPx = characters[*c].Size.y;
        if (charHeightPx > maxCharHeightPx)
            maxCharHeightPx = charHeightPx;

        textWidth += camera->ConvertPixelSizeToRelative(characters[*c].Advance >> 6);
    }

    return { textWidth, camera->ConvertPixelSizeToRelative(maxCharHeightPx, false) };
}

void Renderer2D::ClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
