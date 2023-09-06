#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "core/camera.h"

#include "shader.h"
#include "vertex_array.h"

class Renderer
{
public:
    static void ClearColor(const glm::vec4& color);

    static void BeginScene(const std::shared_ptr<OrthographicCamera>& camera);
    static void EndScene();

    static void Submit(const std::shared_ptr<Shader>& shader, std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& model);

private:
    struct SceneData
    {
        std::shared_ptr<OrthographicCamera> Camera;
    };

    static SceneData* s_Data;
};
