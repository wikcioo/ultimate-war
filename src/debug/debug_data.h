#if defined(DEBUG)

#pragma once

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
        float BorderThickness = 0.01f;
        float Zoom = 3.0f;
    } MinimapData;

private:
    DebugData() {}
};

#endif
