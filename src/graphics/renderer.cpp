#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Renderer::SceneData* Renderer::s_Data = new SceneData();

void Renderer::ClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::BeginScene(const std::shared_ptr<OrthographicCamera>& camera)
{
    Renderer::s_Data->Camera = camera;
}

void Renderer::EndScene()
{

}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, std::shared_ptr<VertexArray>& vertexArray)
{
    shader->Bind();
    shader->SetFloat4("u_Color", {0.2f, 0.7f, 0.7f, 1.0f});
    shader->SetMat4("u_ProjectionView", Renderer::s_Data->Camera->GetProjectionViewMatrix());

    vertexArray->Bind();

    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}
