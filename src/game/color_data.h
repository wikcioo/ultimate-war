#pragma once

#include <vector>

#include <glm/glm.hpp>

class ColorData
{
public:
    ColorData(const ColorData&) = delete;
    ColorData& operator=(const ColorData&) = delete;
    ~ColorData() = default;

    static ColorData& Get()
    {
        static ColorData* s_Instance = new ColorData();
        return *s_Instance;
    }

    struct
    {
        glm::vec4 MiniMapColor;
        glm::vec4 TileHoverBorderColor;
        glm::vec4 AssetBackgroundColor;
        glm::vec3 OceanColor;
        glm::vec3 ForestColor;
        glm::vec3 DesertColor;
        glm::vec3 MountainsColor;
    } TileColors;

    struct
    {
        glm::vec4 ShopPanelBackgroundColor;
        glm::vec4 ShopPanelHighlighUnitGroupColor;
    } UITheme;

    struct
    {
        glm::vec3 Wood;
        glm::vec3 Rock;
        glm::vec3 Steel;
        glm::vec3 Gold;
    } Resources;

private:
    ColorData() {}
};
