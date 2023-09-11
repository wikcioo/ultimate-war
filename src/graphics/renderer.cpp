#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "core/resource_manager.h"

Renderer2D::Renderer2DData* Renderer2D::s_Data = new Renderer2DData();

void Renderer2D::Init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float quadVertices[4 * 5] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int quadIndices[6] = {
        0, 2, 3,
        0, 1, 2
    };

    std::vector<int> quadLayout = {3, 2};
    auto quadVB = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
    auto quadIB = std::make_shared<IndexBuffer>(quadIndices, sizeof(quadIndices) / sizeof(unsigned int));
    s_Data->QuadVertexArray = std::make_shared<VertexArray>(quadVB, quadIB, quadLayout);

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
}

void Renderer2D::EndScene()
{
}

void Renderer2D::Submit(const std::shared_ptr<Shader>& shader, std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& model)
{
    shader->SetMat4("u_Model", model);
    shader->SetMat4("u_ProjectionView", s_Data->Camera->GetProjectionViewMatrix());

    vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    DrawQuad(glm::vec3(position, 0.0f), size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    DrawGeometry(s_Data->QuadVertexArray, position, size, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture)
{
    DrawQuad(glm::vec3(position, 0.0f), size, texture);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

    texture->Bind(0);
    s_Data->TextureShader->Bind();
    s_Data->TextureShader->SetInt("u_Texture", 0);
    s_Data->TextureShader->SetMat4("u_Model", model);

    s_Data->QuadVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, s_Data->QuadVertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::DrawHexagon(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    DrawHexagon(glm::vec3(position, 0.0f), size, color);
}

void Renderer2D::DrawHexagon(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    DrawGeometry(s_Data->HexagonVertexArray, position, size, color);
}

void Renderer2D::DrawGeometry(const std::shared_ptr<VertexArray> vertexArray, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

    s_Data->FlatColorShader->Bind();
    s_Data->FlatColorShader->SetMat4("u_Model", model);
    s_Data->FlatColorShader->SetFloat4("u_Color", color);

    vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::ClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}
