#pragma once

#include <string>
#include <vector>

typedef std::vector<std::vector<int>> MapData;

class GameMap
{
public:
    GameMap(const std::string& filepath = "");
    ~GameMap() = default;

    inline int GetWidth() const { return !m_GameMap.empty() ? m_GameMap[0].size() : 0; }
    inline int GetHeight() const { return m_GameMap.size(); }

    void Load(const std::string& filepath);
    int GetTile(int x, int y) const;

private:
    MapData m_GameMap;
};
