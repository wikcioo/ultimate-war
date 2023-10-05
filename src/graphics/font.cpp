#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

#include "core/logger.h"

Font::Font(const std::string& filepath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("Freetype: Could not initialize FreeType Library");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath.c_str(), 0, &face))
    {
        LOG_ERROR("Freetype: Failed to load font at {0}", filepath);
    }

    FT_Set_Pixel_Sizes(face, 128, 128);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOG_ERROR("Freetype: Failed to load glyph \"{0}\"", c);
            continue;
        }

        TextureData data = {
            .Size = {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            .Data = face->glyph->bitmap.buffer,
            .NrChannels = 1,
            .WrapHorizontal = TextureWrap::CLAMP_TO_EDGE,
            .WrapVertical = TextureWrap::CLAMP_TO_EDGE,
            .MinFilter = TextureFilter::LINEAR,
            .MagFilter = TextureFilter::LINEAR
        };

        auto texture = std::make_shared<Texture2D>(data);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        m_Characters.insert({c, character});
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
