#include "texture.h"

#include "core/logger.h"

#include <glad/glad.h>

static unsigned int TextureWrapToGL(TextureWrap wrap)
{
    switch (wrap)
    {
        case TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
        case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case TextureWrap::REPEAT: return GL_REPEAT;
        default:
            LOG_ERROR("Unknown texture wrap option");
            return GL_REPEAT;
    }
}

static unsigned int TextureFilterToGL(TextureFilter filter)
{
    switch (filter)
    {
        case TextureFilter::NEAREST: return GL_NEAREST;
        case TextureFilter::LINEAR: return GL_LINEAR;
        default:
            LOG_ERROR("Unknown texture filter option");
            return GL_LINEAR;
    }
}

Texture2D::Texture2D(const TextureData& data)
    : m_Width(data.Size.x), m_Height(data.Size.y), m_TextureTarget(data.IsMultisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D)
{
    glGenTextures(1, &m_TextureID);
    glBindTexture(m_TextureTarget, m_TextureID);

    glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_S, TextureWrapToGL(data.WrapHorizontal));
    glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_T, TextureWrapToGL(data.WrapVertical));
    glTexParameteri(m_TextureTarget, GL_TEXTURE_MIN_FILTER, TextureFilterToGL(data.MinFilter));
    glTexParameteri(m_TextureTarget, GL_TEXTURE_MAG_FILTER, TextureFilterToGL(data.MagFilter));

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &data.BorderColor[0]);

    int format;
    if (data.NrChannels == 1)
        format = GL_RED;
    else if (data.NrChannels == 3)
        format = GL_RGB;
    else if (data.NrChannels == 4)
        format = GL_RGBA;
    else
        LOG_ERROR("Texture: Unsupported texture format");

    if (m_TextureTarget == GL_TEXTURE_2D_MULTISAMPLE)
        glTexImage2DMultisample(m_TextureTarget, 4, format, m_Width, m_Height, GL_TRUE);
    else
        glTexImage2D(m_TextureTarget, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data.Data);

    glGenerateMipmap(m_TextureTarget);
    glBindTexture(m_TextureTarget, 0);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_TextureID);
}

void Texture2D::Bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(m_TextureTarget, m_TextureID);
}

void Texture2D::Unbind() const
{
    glBindTexture(m_TextureTarget, 0);
}
