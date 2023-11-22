#pragma once

#include <string>

#include <glm/glm.hpp>

#include "game/game_layer.h"
#include "game/resource.h"

class SaveLoader
{
public:
    static std::shared_ptr<GameLayer> Load(const std::string& saveName);
    static std::vector<std::string> GetAvailableSaves();

private:
    static std::string s_SaveDirectory;
    static std::string s_SaveFileSuffix;

private:
    struct _PlayerData
    {
        std::string Name;
        glm::vec3 Color;
        Resources ResourceData;
    };

    struct _UnitGroupData
    {
        UnitGroupType Type;
        int MovedOnIteration;
        UnitStats Stats;
    };

    struct _BuildingData
    {
        BuildingType Type;
        int Level;
    };

    struct _TileData
    {
        glm::ivec2 Coords;
        int OwnedByPlayerIndex;
        std::vector<_UnitGroupData> UnitGroupData;
        std::vector<_BuildingData> BuildingData;
    };

    struct _SaveData
    {
        std::string MapName;
        std::vector<std::vector<std::string>> MapData;
        int Iteration;
        int CurrentPlayerIndex;
        int NumberOfPlayers;
        std::vector<_PlayerData> PlayersData;
        std::vector<_TileData> TilesData;
    };

private:
    static std::shared_ptr<GameLayer> ConstructGameLayer(const _SaveData& data);
    static std::vector<std::string> Tokenize(const std::string& str, char token);
    static std::string StripOuterChars(const std::string& str);
};
