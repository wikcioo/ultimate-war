#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "core/camera.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/vertex_array.h"

class Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const std::shared_ptr<OrthographicCamera>& camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float borderThickness = -1.0f);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float borderThickness = -1.0f);

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture);

    static void DrawHexagon(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawHexagon(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

    static void DrawGeometry(const std::shared_ptr<VertexArray> vertexArray, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float borderThickness = -1.0f);

    static void ClearColor(const glm::vec4& color);

private:
    struct Renderer2DData
    {
        std::shared_ptr<OrthographicCamera> Camera;
        std::shared_ptr<VertexArray> QuadVertexArray;
        std::shared_ptr<VertexArray> HexagonVertexArray;
        std::shared_ptr<Shader> FlatColorShader;
        std::shared_ptr<Shader> TextureShader;
    };

    static Renderer2DData* s_Data;
};
