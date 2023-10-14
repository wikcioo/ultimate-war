#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "graphics/texture.h"

class Font
{
public:
    Font(const std::string& filepath);
    ~Font() = default;

    struct Character {
        std::shared_ptr<Texture2D> Texture;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        unsigned int Advance;
    };

    inline std::unordered_map<char, Character>& GetCharacters() { return m_Characters; }

private:
    std::unordered_map<char, Character> m_Characters;
};
