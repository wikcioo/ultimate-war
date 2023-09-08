#include "map.h"

#include <sstream>

#include "core/file_system.h"

std::string GameMap::s_MapDirectory = "assets/maps/";
std::string GameMap::s_MapFileSuffix = ".map";

GameMap::GameMap(const std::string& mapName)
    : m_SelectedMap("")
{
    if (!mapName.empty())
    {
        Load(mapName);
        m_SelectedMap = mapName;
    }

    RetrieveAvailableMaps();
}

void GameMap::Load(const std::string& mapName, bool flip_vertically)
{
    std::string content = FileSystem::ReadFile(GetMapPath(mapName));
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

    m_MapData = map;
    m_SelectedMap = mapName;
}

int GameMap::GetTile(int x, int y) const
{
    if (x <= GetWidth() && y <= GetHeight() && x > -1 && y > -1)
        return m_MapData[y][x];

    return -1;
}

void GameMap::RetrieveAvailableMaps()
{
    std::vector<std::string> files = FileSystem::GetAllFilesInDirectory(s_MapDirectory);

    // Remove all entries in files vector which do not end with s_MapFileSuffix
    files.erase(std::remove_if(files.begin(), files.end(), [](const std::string& s) {
        if (s.length() <= s_MapFileSuffix.length()) return true;
        return (s.compare(s.length() - s_MapFileSuffix.length(), s_MapFileSuffix.length(), s_MapFileSuffix) != 0);
    }), files.end());

    for (auto& file : files)
        file = Util::StripFileExtension(file);

    m_AvailableMapList = files;
}

std::string GameMap::GetMapPath(const std::string& mapName)
{
    return s_MapDirectory + mapName + s_MapFileSuffix;
}
