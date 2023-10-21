#include "map_manager.h"

#include <sstream>

#include "core/file_system.h"
#include "game/tile.h"

std::string GameMapManager::s_MapDirectory = "assets/maps/";
std::string GameMapManager::s_MapFileSuffix = ".map";

GameMapManager::GameMapManager(const std::string& mapName)
    : m_SelectedMap("")
{
    if (!mapName.empty())
    {
        Load(mapName);
        m_SelectedMap = mapName;
    }

    RetrieveAvailableMaps();
}

void GameMapManager::Load(const std::string& mapName, bool flip_vertically)
{
    std::string content = FileSystem::ReadFile(GetMapPath(mapName));
    std::istringstream ss(content);

    std::string line;
    std::vector<std::string> rows;
    while (std::getline(ss, line))
        rows.emplace_back(line);

    if (flip_vertically)
        std::reverse(rows.begin(), rows.end());

    int x = 0, y = 0;
    int tileCode;
    MapData map;
    for (std::string row_str : rows)
    {
        std::istringstream sstream(row_str);
        std::vector<std::shared_ptr<Tile>> row;
        while (sstream >> tileCode)
        {
            auto t = std::make_shared<Tile>((TileEnvironment)tileCode, glm::ivec2(x, y));
            row.emplace_back(t);
            x++;
        }
        x = 0;
        y++;
        map.emplace_back(row);
    }

    m_GameMap = std::make_shared<GameMap>(map);
    m_SelectedMap = mapName;
}


void GameMapManager::RetrieveAvailableMaps()
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

std::string GameMapManager::GetMapPath(const std::string& mapName)
{
    return s_MapDirectory + mapName + s_MapFileSuffix;
}

