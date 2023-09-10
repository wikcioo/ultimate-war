#pragma once

#include <vector>

#include <glad/glad.h>

class VertexBuffer
{
public:
    VertexBuffer(float* vertices, unsigned int size, unsigned int usage = GL_STATIC_DRAW);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    void SetLayout(const std::vector<int>& layout);
    void UpdateData(float* data, unsigned int offset, unsigned int size);

private:
    unsigned int m_BufferID;
};

class IndexBuffer
{
public:
    IndexBuffer(unsigned int* indices, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetIndexCount() const { return m_IndexCount; }

private:
    unsigned int m_BufferID;
    unsigned int m_IndexCount;
};
