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

void Renderer::Submit(const std::shared_ptr<Shader>& shader, std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& model)
{
    shader->SetMat4("u_Model", model);
    shader->SetMat4("u_ProjectionView", Renderer::s_Data->Camera->GetProjectionViewMatrix());

    vertexArray->Bind();

    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}
