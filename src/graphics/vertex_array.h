#pragma once

#include <memory>

#include "buffer.h"

class VertexArray
{
public:
    VertexArray(const std::shared_ptr<VertexBuffer>& vertexBuffer,
                const std::shared_ptr<IndexBuffer>& indexBuffer,
                const std::vector<int>& layout);

    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_ArrayID;
};
