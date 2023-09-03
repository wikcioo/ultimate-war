#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "shader.h"
#include "vertex_array.h"

class Renderer
{
public:
    static void ClearColor(const glm::vec4& color);

    static void BeginScene();
    static void EndScene();

    static void Submit(const std::shared_ptr<Shader>& shader, std::shared_ptr<VertexArray>& vertexArray);
};
