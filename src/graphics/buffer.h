#pragma once

#include <vector>

class VertexBuffer
{
public:
    VertexBuffer(float* vertices, unsigned int size);

    void Bind() const;
    void Unbind() const;

    void SetLayout(const std::vector<int>& layout);

private:
    unsigned int m_BufferID;
};

class IndexBuffer
{
public:
    IndexBuffer(unsigned int* indices, unsigned int count);

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetIndexCount() const { return m_IndexCount; }

private:
    unsigned int m_BufferID;
    unsigned int m_IndexCount;
};
