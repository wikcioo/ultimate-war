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
        std::vector<glm::vec4> PlayerTileColor;
        glm::vec4 MiniMapColor;
        glm::vec4 TileHoverBorderColor;
    } TileColors;

private:
    ColorData() {}
};
