#pragma once

#include <vector>

#include <glm/glm.hpp>

class ColorData
{
public:
    ColorData(const ColorData&) = delete;
    ColorData& operator=(const ColorData&) = delete;
    ~ColorData() = default;

    static ColorData* Get()
    {
        static ColorData* s_Instance = new ColorData();
        return s_Instance;
    }

    struct
    {
        glm::vec4 MiniMapColor;
        glm::vec4 TileHoverBorderColor;
    } TileColors;

private:
    ColorData() {}
};
