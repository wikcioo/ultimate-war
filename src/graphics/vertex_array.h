#pragma once

#include <memory>

#include "buffer.h"

class VertexArray
{
public:
    VertexArray(const std::shared_ptr<VertexBuffer>& vertexBuffer,
                const std::shared_ptr<IndexBuffer>& indexBuffer,
                const std::vector<int>& layout);
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    inline const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
    inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

private:
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    unsigned int m_ArrayID;
};
