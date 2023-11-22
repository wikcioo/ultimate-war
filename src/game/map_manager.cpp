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

    m_AvailableMapList = GetAvailableMaps();
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

void GameMapManager::Load(const std::string& mapName, const std::vector<std::vector<std::string>>& mapData)
{
    MapData map;
    for (size_t y = 0; y < mapData.size(); y++)
    {
        const auto& rowData = mapData[y];

        std::vector<std::shared_ptr<Tile>> row;
        for (size_t x = 0; x < rowData.size(); x++)
        {
            const auto& columnData = rowData[x];

            auto t = std::make_shared<Tile>((TileEnvironment)std::stoi(columnData), glm::ivec2(x, y));
            row.emplace_back(t);
        }

        map.emplace_back(row);
    }

    m_GameMap = std::make_shared<GameMap>(map);
    m_SelectedMap = mapName;
}

std::vector<std::string> GameMapManager::GetAvailableMaps()
{
    return FileSystem::GetFilesInDirectoryWithExtension(s_MapDirectory, s_MapFileSuffix);
}

std::string GameMapManager::GetMapPath(const std::string& mapName)
{
    return s_MapDirectory + mapName + s_MapFileSuffix;
}

