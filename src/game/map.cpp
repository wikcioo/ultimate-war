#include "map.h"

#include <sstream>

#include "core/file_system.h"

GameMap::GameMap(const std::string& filepath)
{
    if (!filepath.empty())
    {
        Load(filepath);
    }
}

void GameMap::Load(const std::string& filepath, bool flip_vertically)
{
    std::string content = FileSystem::ReadFile(filepath);
    std::istringstream ss(content);

    std::string line;
    std::vector<std::string> rows;
    while (std::getline(ss, line))
        rows.emplace_back(line);

    if (flip_vertically)
        std::reverse(rows.begin(), rows.end());

    int tileCode;
    MapData map;
    for (std::string row_str : rows)
    {
        std::istringstream sstream(row_str);
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
