#include "buffer.h"

#include <numeric>

#include <glad/glad.h>

VertexBuffer::VertexBuffer(float* vertices, unsigned int size)
{
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetLayout(const std::vector<int>& layout)
{
    int stride = std::accumulate(layout.begin(), layout.end(), 0);

    int offset = 0;
    for (int i = 0; i < layout.size(); i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            layout[i],
            GL_FLOAT,
            GL_FALSE,
            stride * sizeof(float),
            (const void*)(offset * sizeof(float))
        );

        offset += layout[i];
    }
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
}

IndexBuffer::IndexBuffer(unsigned int* indices, unsigned int count)
{
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    m_IndexCount = count;
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
