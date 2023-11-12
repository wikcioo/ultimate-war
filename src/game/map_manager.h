#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game/map.h"

class GameMapManager
{
public:
    GameMapManager(const std::string& filepath = "");
    ~GameMapManager() = default;

    static std::vector<std::string> GetAvailableMaps();
    static std::string GetMapPath(const std::string& mapName);

    inline std::string GetSelectedMapName() const { return m_SelectedMap; }
    inline const std::shared_ptr<GameMap>& GetGameMap() const { return m_GameMap; }

    void Load(const std::string& filepath, bool flip_vertically = true);

private:
    std::string m_SelectedMap;
    std::vector<std::string> m_AvailableMapList;
    std::shared_ptr<GameMap> m_GameMap;

    static std::string s_MapDirectory;
    static std::string s_MapFileSuffix;
};
