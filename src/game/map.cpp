#include "map.h"

#include <sstream>

#include "core/filesystem.h"

GameMap::GameMap(const std::string& filepath)
{
    if (!filepath.empty())
    {
        Load(filepath);
    }
}

void GameMap::Load(const std::string& filepath)
{
    std::string content = FileSystem::ReadFile(filepath);

    std::istringstream ss(content);
    std::string line;
    int tileCode;
    MapData map;

    while (std::getline(ss, line))
    {
        std::istringstream sstream(line);
        std::vector<int> row;
        while (sstream >> tileCode)
            row.emplace_back(tileCode);
        map.emplace_back(row);
    }

    m_GameMap = map;
}

int GameMap::GetTile(int x, int y) const
{
    if (x <= GetWidth() && y <= GetHeight() && x > -1 && y > -1)
    {
        return m_GameMap[y][x];
    }

    return -1;
}
