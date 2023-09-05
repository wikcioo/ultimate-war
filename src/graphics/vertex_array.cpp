#include "vertex_array.h"

#include <glad/glad.h>

VertexArray::VertexArray(const std::shared_ptr<VertexBuffer>& vertexBuffer,
                         const std::shared_ptr<IndexBuffer>& indexBuffer,
                         const std::vector<int>& layout)
    : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer)
{
    glGenVertexArrays(1, &m_ArrayID);
    glBindVertexArray(m_ArrayID);

    vertexBuffer->Bind();
    vertexBuffer->SetLayout(layout);

    indexBuffer->Bind();
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_ArrayID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_ArrayID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
