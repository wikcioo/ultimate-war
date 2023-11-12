#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>

#include "graphics/texture.h"

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

class FrameBuffer
{
public:
    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void PostProcess() const;

    inline const std::shared_ptr<Texture2D>& GetTexture() const { return m_DisplayedColorTexture; }

private:
    unsigned int m_Width, m_Height;
    unsigned int m_IntermediateBufferID;
    unsigned int m_MultiSampledBufferID;
    std::shared_ptr<Texture2D> m_DisplayedColorTexture;
    std::shared_ptr<Texture2D> m_MultiSampledColorTexture;
};
