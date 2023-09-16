#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "game/tile.h"

typedef std::vector<std::vector<Tile*>> MapData;

class GameMap
{
public:
    GameMap(const std::string& filepath = "");
    ~GameMap() = default;

    inline int GetTileCountX() const { return !m_MapData.empty() ? m_MapData[0].size() : 0; }
    inline int GetTileCountY() const { return m_MapData.size(); }

    inline std::vector<std::string>& GetAvailableMaps() { return m_AvailableMapList; }
    inline std::string GetSelectedMapName() const { return m_SelectedMap; }

    const glm::vec4& GetTileDefaultColor(int type);
    const glm::vec4& GetTileHighlightColor(int type);

    void SetTileDefaultColor(int type, const glm::vec4& color) { m_TileDefaultColorMap.insert({type, color}); }
    void SetTileHighlightColor(int type, const glm::vec4& color) { m_TileHighlightColorMap.insert({type, color}); }

    void Load(const std::string& filepath, bool flip_vertically = true);
    Tile* GetTile(int x, int y);

private:
    void RetrieveAvailableMaps();
    std::string GetMapPath(const std::string& mapName);
    glm::vec2 CalculateTilePosition(int x, int y);

private:
    MapData m_MapData;
    std::string m_SelectedMap;
    std::vector<std::string> m_AvailableMapList;
    const glm::vec4 m_MissingTileTypeColor;
    std::unordered_map<int, glm::vec4> m_TileDefaultColorMap;
    std::unordered_map<int, glm::vec4> m_TileHighlightColorMap;

    static std::string s_MapDirectory;
    static std::string s_MapFileSuffix;
};
