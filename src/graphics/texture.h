#pragma once

#include <glm/glm.hpp>

enum class TextureWrap
{
    NONE = 0,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    MIRRORED_REPEAT,
    REPEAT,
};

enum class TextureFilter
{
    NONE = 0,
    NEAREST,
    LINEAR,
};

struct TextureData
{
    glm::vec2 Size = {0.5f, 0.5f};
    unsigned char* Data = nullptr;
    unsigned int NrChannels = 3;
    TextureWrap WrapHorizontal = TextureWrap::REPEAT;
    TextureWrap WrapVertical = TextureWrap::REPEAT;
    TextureFilter MinFilter = TextureFilter::NEAREST;
    TextureFilter MagFilter = TextureFilter::NEAREST;
    bool IsMultisample = false;
};

class Texture2D
{
public:
    Texture2D(const TextureData& data);
    ~Texture2D();

    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }
    inline unsigned int GetID() const { return m_TextureID; }

    void Bind(unsigned int unit) const;
    void Unbind() const;

private:
    unsigned int m_Width, m_Height;
    unsigned int m_TextureID;
    unsigned int m_TextureTarget;
};
