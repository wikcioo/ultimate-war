#pragma once

#include <memory>

#include <glm/glm.hpp>
#include "graphics/texture.h"

struct ResourceData
{
    static const int NumResources = 4;
    glm::vec3 ResourceNumberColors[NumResources];
    std::shared_ptr<Texture2D> ResourceTextures[NumResources];
    float ResourceTextureScales[NumResources];
};

struct Resources
{
    int Wood;
    int Rock;
    int Steel;
    int Gold;

    static void Draw2x2(const Resources& res, const glm::vec2& position);

    Resources operator*(double scalar);
    Resources& operator+=(const Resources& other);
    Resources& operator-=(const Resources& other);
    bool operator>=(const Resources& other);

    static ResourceData GetResourceData();
};
