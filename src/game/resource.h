#pragma once

#include <memory>

#include <glm/glm.hpp>
#include "graphics/texture.h"

struct ResourceData
{
    static const int NumResources = 4;
    glm::vec3 ResourceNumberColors[NumResources];
    std::shared_ptr<Texture2D> ResourceTextures[NumResources];
};

struct Resources
{
    int Wood;
    int Rock;
    int Steel;
    int Gold;

    Resources& operator+=(const Resources& other);
    Resources& operator-=(const Resources& other);
    bool operator>=(const Resources& other);

    static ResourceData GetResourceData();
};
