#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include "core/camera.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/vertex_array.h"

// ratio of character spacing to character height
#define FONT_Y_SPACING_RATIO 0.3f

enum class HTextAlign
{
    LEFT   = 0,
    MIDDLE = 1,
    RIGHT  = 2
};

enum class VTextAlign
{
    TOP    = 0,
    MIDDLE = 1,
    BOTTOM = 2
};

class Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const std::shared_ptr<OrthographicCamera>& camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color,
                         std::optional<float> borderThickness = std::nullopt);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color,
                         std::optional<float> borderThickness = std::nullopt);

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture,
                         const glm::vec4& color = glm::vec4(1.0f));
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture,
                         const glm::vec4& color = glm::vec4(1.0f));

    static void DrawHexagon(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color,
                            std::optional<float> borderThickness = std::nullopt);
    static void DrawHexagon(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color,
                            std::optional<float> borderThickness = std::nullopt);
    static void DrawHexagon(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Shader>& shader,
                            ShaderData& shaderData);
    static void DrawHexagon(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Shader>& shader,
                            ShaderData& shaderData);

    static void DrawGeometry(const std::shared_ptr<VertexArray>& vertexArray, const glm::vec3& position, const glm::vec2& size,
                             const glm::vec4& color, std::optional<float> borderThickness = std::nullopt);

    static void DrawTextStr(const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color = glm::vec3(1.0f),
                            HTextAlign hAlign = HTextAlign::LEFT, VTextAlign vAlign = VTextAlign::BOTTOM,
                            const std::string& fontName = "rexlia");
    static void DrawTextStr(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color = glm::vec4(1.0f),
                            HTextAlign hAlign = HTextAlign::LEFT, VTextAlign vAlign = VTextAlign::BOTTOM,
                            const std::string& fontName = "rexlia");

    static glm::vec2 GetTextSize(const std::shared_ptr<OrthographicCamera>& camera, const std::string& text,
                                 const std::string& fontName);

    static void ClearColor(const glm::vec4& color);

private:
    struct Renderer2DData
    {
        std::shared_ptr<OrthographicCamera> Camera;
        std::shared_ptr<VertexArray> QuadVertexArray;
        std::shared_ptr<VertexArray> FontVertexArray;
        std::shared_ptr<VertexArray> HexagonVertexArray;
        std::shared_ptr<Shader> FlatColorShader;
        std::shared_ptr<Shader> TextureShader;
        std::shared_ptr<Shader> FontShader;
    };

    static Renderer2DData* s_Data;
};
