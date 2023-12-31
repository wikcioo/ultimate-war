#include "buffer.h"

#include <numeric>

#include "core/logger.h"
#include "core/application.h"

VertexBuffer::VertexBuffer(float* vertices, unsigned int size, unsigned int usage)
{
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void VertexBuffer::UpdateData(float* data, unsigned int offset, unsigned int size)
{
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

IndexBuffer::IndexBuffer(unsigned int* indices, unsigned int count)
{
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_IndexCount = count;
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
    : m_Width(width), m_Height(height)
{
    TextureData textureData;
    textureData.Size = { m_Width, m_Height };
    textureData.NrChannels = 3;
    textureData.IsMultisample = true;

    m_MultiSampledColorTexture = std::make_shared<Texture2D>(textureData);
    glGenFramebuffers(1, &m_MultiSampledBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_MultiSampledBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MultiSampledColorTexture->GetID(), 0);

    unsigned int multiSampledRenderBuffer;
    glGenRenderbuffers(1, &multiSampledRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, multiSampledRenderBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multiSampledRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Framebuffer: Incomplete multisampled framebuffer");

    textureData.IsMultisample = false;
    m_DisplayedColorTexture = std::make_shared<Texture2D>(textureData);
    glGenFramebuffers(1, &m_IntermediateBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_IntermediateBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_DisplayedColorTexture->GetID(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Framebuffer: Incomplete multisampled framebuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_IntermediateBufferID);
    glDeleteFramebuffers(1, &m_MultiSampledBufferID);
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_MultiSampledBufferID);
    glViewport(0, 0, m_Width, m_Height);
}

void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    auto window = Application::Get().GetWindow();
    glViewport(0, 0, window->GetWidth(), window->GetHeight());
}

void FrameBuffer::PostProcess() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MultiSampledBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_IntermediateBufferID);
    glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
