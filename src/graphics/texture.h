#pragma once

class Texture2D
{
public:
    Texture2D(unsigned int width, unsigned int height, unsigned char* data, unsigned int nrChannels);
    ~Texture2D();

    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }

    void Bind(unsigned int unit) const;
    void Unbind() const;

private:
    unsigned int m_Width, m_Height;
    unsigned int m_TextureID;
};
