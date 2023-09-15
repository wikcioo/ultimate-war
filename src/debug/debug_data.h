#if defined(DEBUG)

#pragma once

class DebugData
{
public:
    DebugData(const DebugData&) = delete;
    DebugData& operator=(const DebugData&) = delete;
    ~DebugData() = default;

    static DebugData* Get() {
        static DebugData* s_Instance = new DebugData();
        return s_Instance;
    }

    struct
    {
        bool ShowUnitBackground = false;
        int UnitRows = 2;
        int UnitsPerRow = 5;
        int UnitWidthToOffsetRatio = 10;
    } UnitData;

    struct
    {
        float HeightRatio = 0.8f;
    } TileData;


private:
    DebugData() {}
};

#endif
