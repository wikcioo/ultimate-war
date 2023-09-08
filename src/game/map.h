#pragma once

#include <string>
#include <vector>

typedef std::vector<std::vector<int>> MapData;

class GameMap
{
public:
    GameMap(const std::string& filepath = "");
    ~GameMap() = default;

    inline int GetWidth() const { return !m_MapData.empty() ? m_MapData[0].size() : 0; }
    inline int GetHeight() const { return m_MapData.size(); }

    inline std::vector<std::string>& GetAvailableMaps() { return m_AvailableMapList; }
    inline std::string GetSelectedMapName() const { return m_SelectedMap; }

    void Load(const std::string& filepath, bool flip_vertically = true);
    int GetTile(int x, int y) const;

private:
    void RetrieveAvailableMaps();
    std::string GetMapPath(const std::string& mapName);

private:
    MapData m_MapData;
    std::string m_SelectedMap;
    std::vector<std::string> m_AvailableMapList;

    static std::string s_MapDirectory;
    static std::string s_MapFileSuffix;
};
