#include "save_loader.h"

#include <sstream>

#include "core/file_system.h"
#include "loader/save_loader_exception.h"

std::string SaveLoader::s_SaveDirectory = "saves/";
std::string SaveLoader::s_SaveFileSuffix = ".war";

void SaveLoader::Save(const std::string& saveName, const std::shared_ptr<GameLayer>& gameLayer)
{
    std::string content;

    auto mapManager = gameLayer->GetGameMapManager();
    auto playerManager = gameLayer->GetPlayerManager();

    // collect game data
    std::string mapName = mapManager->GetSelectedMapName();
    Util::RemoveCRLF(mapName);
    int numberOfRows = mapManager->GetGameMap()->GetTileCountY();

    std::string mapTileData;
    for (int y = 0; y < mapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < mapManager->GetGameMap()->GetTileCountX(); x++)
        {
            mapTileData += std::to_string((int)mapManager->GetGameMap()->GetTile(x, y)->GetEnvironment());
            mapTileData += ' ';
        }

        mapTileData += '\n';
    }

    int iterationNumber = gameLayer->GetIteration();
    int currentPlayerIndex = playerManager->GetCurrentPlayerIndex();
    int numberOfPlayers = playerManager->GetAllPlayers().size();

    std::vector<std::string> players;
    for (const auto& player : playerManager->GetAllPlayers())
    {
        auto name = player->GetName();
        auto color = player->GetColor();
        auto res = player->GetResources();
        auto isAI = player->IsAIPlayer();

        std::ostringstream oss;
        oss << "\"" << name << "\" ";
        oss << "(" << color.r << "," << color.g << "," << color.b << ") ";
        oss << "(" << res.Wood << "," << res.Rock << "," << res.Steel << "," << res.Gold << ") ";
        oss << isAI;

        players.emplace_back(oss.str());
    }

    std::vector<std::string> tiles;
    for (int y = 0; y < mapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < mapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = mapManager->GetGameMap()->GetTile(x, y);
            if (!tile->AssetsCanExist())
                continue;

            std::ostringstream oss;
            oss << "(" << tile->GetCoords().x << "," << tile->GetCoords().y << ")\n";
            if (tile->GetOwnedBy())
            {
                auto playerName = tile->GetOwnedBy()->GetName();
                bool foundPlayer = false;
                for (size_t i = 0; i < players.size(); i++)
                {
                    auto player = players[i];
                    if (player.find("\"" + playerName + "\"") != std::string::npos)
                    {
                        oss << "P" << i << '\n';
                        foundPlayer = true;
                        break;
                    }
                }
                if (!foundPlayer)
                {
                    throw SaveLoaderException(
                        "SaveLoader: Could not find tile's owner '" + playerName + "'"
                    );
                }
            }

            for (auto ug : tile->GetUnitGroups())
            {
                auto type = (int)ug->GetType();
                auto moved_on_iteration = ug->GetMovedOnIteration();
                auto stats_vec = ug->GetUnitStats();

                if (!stats_vec.empty())
                {
                    auto stats = stats_vec[0];
                    oss << "U(" << type << "," << moved_on_iteration << ",";
                    oss << "(" << stats->Attack << ";" << stats->Defense << ";" << stats->Health << "))\n";
                }
                else
                {
                    throw SaveLoaderException("SaveLoader: Empty unit stats vector");
                }
            }

            for (auto b : tile->GetBuildings())
            {
                auto type = (int)b->GetType();
                auto level = b->GetLevel();

                oss << "B(" << type << "," << level << ")\n";
            }

            tiles.emplace_back(oss.str());
        }
    }

    // write data to content string
    content += mapName + '\n';
    content += std::to_string(numberOfRows) + '\n';
    content += mapTileData;
    content += std::to_string(iterationNumber) + '\n';
    content += std::to_string(currentPlayerIndex) + '\n';
    content += std::to_string(numberOfPlayers) + '\n';

    for (const auto& playerData : players)
        content += playerData + '\n';

    for (const auto& tileData : tiles)
        content += tileData;

    // save to disk
    FileSystem::WriteFile(s_SaveDirectory + saveName + s_SaveFileSuffix, content);
}

std::shared_ptr<GameLayer> SaveLoader::Load(const std::string& saveName)
{
    std::string filepath = s_SaveDirectory + saveName + s_SaveFileSuffix;
    std::string content = FileSystem::ReadFile(filepath);

    std::istringstream iss(content);
    std::vector<std::string> lines;

    std::string _line;
    while (std::getline(iss, _line))
        lines.push_back(_line);

    _SaveData data;
    int currentLineIndex = 0;

    // read map name
    data.MapName = lines[currentLineIndex++];

    // read map data
    std::string tileCode;
    int numOfMapRows = std::stoi(lines[currentLineIndex++]);
    for (int lineNumber = currentLineIndex; lineNumber < currentLineIndex + numOfMapRows; lineNumber++)
    {
        std::vector<std::string> row;
        std::istringstream sstream(lines[lineNumber]);

        while (sstream >> tileCode)
            row.emplace_back(tileCode);

        data.MapData.emplace_back(row);
    }
    currentLineIndex += numOfMapRows;

    // read iteration number
    data.Iteration = std::stoi(lines[currentLineIndex++]);

    // read current player index
    data.CurrentPlayerIndex = std::stoi(lines[currentLineIndex++]);

    // read number of players
    data.NumberOfPlayers = std::stoi(lines[currentLineIndex++]);

    // read player data
    for (int lineNumber = currentLineIndex; lineNumber < currentLineIndex + data.NumberOfPlayers; lineNumber++)
    {
        _PlayerData playerData;

        std::string line = lines[lineNumber];

        // extract player name
        size_t end = line.find_last_of("\"");
        if (end != std::string::npos)
        {
            playerData.Name = line.substr(1, end - 1);
        }
        else
        {
            throw SaveLoaderException("SaveLoader: Could not find player name in quotes");
        }

        // tokenize the rest of player data e.g. (1.0,0.0,0.0) (1,1,1,1) 0
        auto playerTokens = Tokenize(line.substr(end + 2), ' ');
        if (playerTokens.size() < 3)
        {
            throw SaveLoaderException(
                "SaveLoader: Incorrent number of player tokens: '" + std::to_string(playerTokens.size()) + "'"
            );
        }

        // tokenize color e.g. (1.0,0.0,0.0) - remove '(' and ')' before tokenization
        auto colorTokens = Tokenize(StripOuterChars(playerTokens[0]), ',');
        if (colorTokens.size() < 3)
        {
            throw SaveLoaderException(
                "SaveLoader: Incorrent number of colors: '" + std::to_string(colorTokens.size()) + "'"
            );
        }

        playerData.Color = {
            std::stof(colorTokens[0]),
            std::stof(colorTokens[1]),
            std::stof(colorTokens[2])
        };

        // tokenize resources e.g. (1,1,1,1) - remove '(' and ')' before tokenization
        auto resourceTokens = Tokenize(StripOuterChars(playerTokens[1]), ',');
        if (resourceTokens.size() < 4)
        {
            throw SaveLoaderException(
                "SaveLoader: Incorrent number of resources: '" + std::to_string(resourceTokens.size()) + "'"
            );
        }

        playerData.ResourceData.Wood  = std::stoi(resourceTokens[0]);
        playerData.ResourceData.Rock  = std::stoi(resourceTokens[1]);
        playerData.ResourceData.Steel = std::stoi(resourceTokens[2]);
        playerData.ResourceData.Gold  = std::stoi(resourceTokens[3]);

        Util::RemoveCRLF(playerTokens[2]);
        playerData.IsAI = playerTokens[2] == "1";

        data.PlayersData.push_back(playerData);
    }
    currentLineIndex += data.NumberOfPlayers;

    // read tile information - owned by player index, unit groups, buildings
    while (true)
    {
        if (currentLineIndex >= lines.size()-1)
            break;

        _TileData tileData;

        // read tile coords
        auto tileCoordsTokens = Tokenize(StripOuterChars(lines[currentLineIndex++]), ',');
        if (tileCoordsTokens.size() < 2)
        {
            throw SaveLoaderException(
                "SaveLoader: Incorrent number of tile coords: '" + std::to_string(tileCoordsTokens.size()) + "'"
            );
        }

        tileData.Coords = glm::ivec2(
            std::stoi(tileCoordsTokens[0]),
            std::stoi(tileCoordsTokens[1])
        );

        tileData.OwnedByPlayerIndex = -1;
        // read player index, unit group, building data
        // while it is not the beginning of the next tile data
        while (lines[currentLineIndex].find('(') != 0)
        {
            std::string tileDataRow = lines[currentLineIndex];

            switch (tileDataRow[0])
            {
                case 'P':
                {
                    tileData.OwnedByPlayerIndex = std::stoi(tileDataRow.substr(1, tileDataRow.length() - 1));
                    break;
                }
                case 'U':
                {
                    std::string unitData = tileDataRow.substr(1, tileDataRow.length() - 1);
                    auto unitDataTokens = Tokenize(StripOuterChars(unitData), ',');
                    if (unitDataTokens.size() < 3)
                    {
                        throw SaveLoaderException(
                            "SaveLoader: Incorrent number of unit group data tokens: '" + std::to_string(unitDataTokens.size()) + "'"
                        );
                    }

                    _UnitGroupData ugd;
                    ugd.Type = (UnitGroupType)std::stoi(unitDataTokens[0]);
                    ugd.MovedOnIteration = std::stoi(unitDataTokens[1]);

                    // read stats
                    auto statsTokens = Tokenize(StripOuterChars(unitDataTokens[2]), ';');
                    if (statsTokens.size() < 3)
                    {
                        throw SaveLoaderException(
                            "SaveLoader: Incorrent number of stats data tokens: '" + std::to_string(statsTokens.size()) + "'"
                        );
                    }

                    ugd.Stats = {
                        std::stoi(statsTokens[0]),
                        std::stoi(statsTokens[1]),
                        std::stoi(statsTokens[2])
                    };

                    tileData.UnitGroupData.emplace_back(ugd);
                    break;
                }
                case 'B':
                {
                    std::string buildingData = tileDataRow.substr(1, tileDataRow.length() - 1);
                    auto buildingDataTokens = Tokenize(StripOuterChars(buildingData), ',');
                    if (buildingDataTokens.size() < 2)
                    {
                        throw SaveLoaderException(
                            "SaveLoader: Incorrent number of building data tokens: '" + std::to_string(buildingDataTokens.size()) + "'"
                        );
                    }

                    _BuildingData bd;
                    bd.Type = (BuildingType)std::stoi(buildingDataTokens[0]);
                    bd.Level = std::stoi(buildingDataTokens[1]);

                    tileData.BuildingData.emplace_back(bd);
                    break;
                }
            }

            if (currentLineIndex + 1 < lines.size())
                currentLineIndex++;
            else
                break;
        }

        data.TilesData.emplace_back(tileData);
    }

    return ConstructGameLayer(data);
}

std::vector<std::string> SaveLoader::GetAvailableSaves()
{
    return FileSystem::GetFilesInDirectoryWithExtension(s_SaveDirectory, s_SaveFileSuffix);
}

std::shared_ptr<GameLayer> SaveLoader::ConstructGameLayer(const _SaveData& data)
{
    std::shared_ptr<GameLayer> gameLayer = std::make_shared<GameLayer>();

    // gather player data
    std::vector<PlayerDTO> players;
    for (size_t i = 0; i < data.PlayersData.size(); i++)
    {
        const _PlayerData& playerData = data.PlayersData[i];

        // find all tile coords owned by the player
        std::vector<glm::vec2> ownedTileCoords;
        for (const auto& tileData : data.TilesData)
        {
            if (tileData.OwnedByPlayerIndex == i)
                ownedTileCoords.emplace_back(tileData.Coords);
        }

        players.emplace_back(
            PlayerDTO(
                playerData.Name,
                playerData.Color,
                ownedTileCoords,
                playerData.ResourceData,
                playerData.IsAI
            )
        );
    }

    gameLayer->InitGame({ data.MapName, players, data.MapData, true });

    gameLayer->SetIterationNumber(data.Iteration);

    gameLayer->GetPlayerManager()->SetCurrentPlayerIndex(data.CurrentPlayerIndex);

    for (const auto& player : gameLayer->GetPlayerManager()->GetAllPlayers())
        gameLayer->GetPlayerManager()->UpdatePlayerStatus(player);

    // add unit groups and buildings to tiles
    auto gameMap = gameLayer->GetGameMapManager()->GetGameMap();

    for (int y = 0; y < gameMap->GetTileCountY(); y++)
    {
        for (int x = 0; x < gameMap->GetTileCountX(); x++)
        {
            auto it = std::find_if(data.TilesData.begin(), data.TilesData.end(), [x, y](const _TileData& tileData) {
                return tileData.Coords.x == x && tileData.Coords.y == y;
            });

            if (it != data.TilesData.end())
            {
                auto tile = gameMap->GetTile(x, y);
                for (const auto& unitGroup : it->UnitGroupData)
                {
                    UnitGroup ug(unitGroup.Type, new UnitStats(unitGroup.Stats));
                    ug.SetMovedOnIteration(unitGroup.MovedOnIteration);

                    tile->CreateUnitGroup(ug);
                }

                for (const auto& building : it->BuildingData)
                {
                    Building b(building.Type);
                    b.SetLevel((unsigned int)building.Level);

                    tile->CreateBuilding(b);
                }
            }
        }
    }

    return gameLayer;
}

std::vector<std::string> SaveLoader::Tokenize(const std::string& str, char separator)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, separator))
        tokens.push_back(token);

    return tokens;
}

std::string SaveLoader::StripOuterChars(const std::string& str)
{
    return str.substr(1, str.length() - 2);
}
