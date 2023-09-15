#include "map.h"

#include <sstream>

#include "core/file_system.h"

std::string GameMap::s_MapDirectory = "assets/maps/";
std::string GameMap::s_MapFileSuffix = ".map";

static Tile g_BadTile(-1, {0.0f, 0.0f});

GameMap::GameMap(const std::string& mapName)
    : m_SelectedMap(""), m_MissingTileTypeColor({1.0f, 0.0f, 1.0f, 1.0f})
{
    if (!mapName.empty())
    {
        Load(mapName);
        m_SelectedMap = mapName;
    }

    RetrieveAvailableMaps();
}

const glm::vec4& GameMap::GetTileDefaultColor(int type)
{
    if (m_TileDefaultColorMap.find(type) != m_TileDefaultColorMap.end())
        return m_TileDefaultColorMap[type];

    return m_MissingTileTypeColor;
}

const glm::vec4& GameMap::GetTileHighlightColor(int type)
{
    if (m_TileHighlightColorMap.find(type) != m_TileHighlightColorMap.end())
        return m_TileHighlightColorMap[type];

    return m_MissingTileTypeColor;
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

    int x = 0, y = 0;
    int tileCode;
    MapData map;
    for (std::string row_str : rows)
    {
        std::istringstream sstream(row_str);
        std::vector<Tile*> row;
        while (sstream >> tileCode)
        {
            auto pos = CalculateTilePosition(x, y);
            Tile* t = new Tile(tileCode, pos);
            row.emplace_back(t);
            x++;
        }
        x = 0;
        y++;
        map.emplace_back(row);
    }

    m_MapData = map;
    m_SelectedMap = mapName;

    for (int i = 0; i < 5; i++)
    {
        m_MapData[0][0]->AddUnit(UnitType::ARCHER);
    }
    for (int i = 0; i < 5; i++)
    {
        m_MapData[0][0]->AddUnit(UnitType::HARPY);
    }
}

Tile* GameMap::GetTile(int x, int y)
{
    if (x < GetWidth() && y < GetHeight() && x > -1 && y > -1)
        return m_MapData[y][x];

    auto pos = CalculateTilePosition(x, y);
    g_BadTile = Tile(-1, pos);
    return &g_BadTile;
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

glm::vec2 GameMap::CalculateTilePosition(int x, int y)
{
    float w = tileWidth;
    float h = tileHeight;

    float dx = (w-(w/4)) * x + (x * tileOffset / 2 * glm::sqrt(3));
    float dy = (h * y) + (y * tileOffset);

    if (x & 1)
    {
        dy += (h + tileOffset) / 2;
    }

    return { dx, dy };
}
