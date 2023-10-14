#if defined(DEBUG)

#pragma once

#include "graphics/renderer.h"

class DebugData
{
public:
    DebugData(const DebugData&) = delete;
    DebugData& operator=(const DebugData&) = delete;
    ~DebugData() = default;

    static DebugData* Get()
    {
        static DebugData* s_Instance = new DebugData();
        return s_Instance;
    }

    struct
    {
        float BorderThickness = 5.0f;
        float Zoom = 3.0f;
    } MinimapData;

    struct
    {
        std::string FontName = "vinque";
        char Text[100] = "Welcome back traveller!";
        TextAlignment Alignment = TextAlignment::MIDDLE;
        glm::vec2 Pos = { 0.0f, 0.848f };
        glm::vec3 Color = { 0.95f, 0.7f, 0.05f };
        float Scale = 1.0f;
    } Font;

private:
    DebugData() {}
};

#endif
