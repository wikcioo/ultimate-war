#include "texture.h"

#include "core/logger.h"

#include <glad/glad.h>

Texture2D::Texture2D(unsigned int width, unsigned int height, unsigned char* data, unsigned int nrChannels, bool multisample)
    : m_Width(width), m_Height(height), m_TextureTarget(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D)
{
    glGenTextures(1, &m_TextureID);
    glBindTexture(m_TextureTarget, m_TextureID);

    glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int format;
    if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    else
        LOG_ERROR("Texture: Unsupported texture format");

    if (m_TextureTarget == GL_TEXTURE_2D_MULTISAMPLE)
        glTexImage2DMultisample(m_TextureTarget, 4, format, width, height, GL_TRUE);
    else
        glTexImage2D(m_TextureTarget, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

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
