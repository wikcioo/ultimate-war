#include "game_layer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/tile.h"
#include "debug/debug_data.h"
#include "core/core.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "util/util.h"

GameLayer* GameLayer::s_Instance = nullptr;

GameLayer::GameLayer()
    : Layer("GameLayer"), m_IterationNumber(0), m_GameActive(true)
{
    s_Instance = this;

    auto window = Application::Get().GetWindow();
    m_CameraController = std::make_shared<OrthographicCameraController>((float)window->GetWidth() / (float)window->GetHeight(), true);
    m_GameMapManager = std::make_shared<GameMapManager>("");
    m_PlayerManager = std::make_shared<PlayerManager>();
    m_Arrow = std::make_shared<Arrow>();
}

void GameLayer::OnAttach()
{
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    m_CameraController->OnUpdate(dt);

    Renderer2D::ClearColor({0.2f, 0.2f, 0.2f, 1.0f});

    Renderer2D::BeginScene(m_CameraController->GetCamera());

    auto currentPlayer = m_PlayerManager->GetCurrentPlayer();
    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();
    bool isCursorOnAdjacentTile = false;

    struct
    {
        bool NotEnoughSpace = false;
        glm::vec2 Position;
    } notEnoughSpaceInfo;

    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);

            bool isCursorOnTile = false;
            if (!isCursorOnAdjacentTile && tile->InRange(relMousePos))
            {
                auto startTile = m_Arrow->GetStartTile();
                if (startTile && Tile::IsAdjacent({x, y}, startTile->GetCoords()) && tile->GetEnvironment() != TileEnvironment::NONE)
                {
                    isCursorOnAdjacentTile = true;
                    m_Arrow->SetEndPosition(tile->GetPosition());

                    if (tile->GetOwnedBy() == currentPlayer &&
                       !tile->HasSpaceForUnitGroups(m_Arrow->GetStartTile()->GetNumSelectedUnitGroups()))
                    {
                        m_Arrow->SetColor({1.0f, 0.0f, 0.0f, 1.0f});

                        notEnoughSpaceInfo.NotEnoughSpace = true;
                        notEnoughSpaceInfo.Position = {
                            m_Arrow->GetStartTile()->GetPosition().x + (tile->GetPosition().x - m_Arrow->GetStartTile()->GetPosition().x) / 2.0f,
                            m_Arrow->GetStartTile()->GetPosition().y + (tile->GetPosition().y - m_Arrow->GetStartTile()->GetPosition().y) / 2.0f
                        };
                    }
                    else
                    {
                        m_Arrow->SetColor({0.0f, 1.0f, 1.0f, 1.0f});
                    }
                }

                isCursorOnTile = true;
            }

            tile->Draw();
            if (isCursorOnTile)
            {
                Renderer2D::DrawHexagon(
                    tile->GetPosition(),
                    glm::vec2(1.0f),
                    ColorData::Get().TileColors.TileHoverBorderColor,
                    3.0f
                );

                tile->CheckUnitGroupHover(relMousePos);
                tile->CheckBuildingHover(relMousePos);
            }
        }
    }

    if (m_Arrow->IsVisible() && !isCursorOnAdjacentTile)
        m_Arrow->SetEndPosition(m_Arrow->GetStartTile()->GetPosition());

    m_Arrow->Draw();

    if (notEnoughSpaceInfo.NotEnoughSpace)
    {
        Renderer2D::DrawTextStr(
            "Not enough space",
            notEnoughSpaceInfo.Position,
            1.0f / m_CameraController->GetCamera()->GetZoom(),
            glm::vec3(1.0f),
            HTextAlign::MIDDLE
        );
    }

    Renderer2D::EndScene();
}

void GameLayer::OnEvent(Event& event)
{
    m_CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(GameLayer::OnKeyReleased));
}

bool GameLayer::OnKeyReleased(KeyReleasedEvent& event)
{
    if(!m_GameActive) return true;

    if(event.GetKeyCode() == GLFW_KEY_ENTER)
    {
        m_PlayerManager->NextTurn();
        return true;
    }

    return false;
}

void GameLayer::InitGame(NewGameDTO newGameData)
{
    m_GameMapManager->Load(newGameData.MapName);

    glm::vec2 mapMiddle = {
        (m_GameMapManager->GetGameMap()->GetTileCountX() * (3.0f / 4.0f * TILE_WIDTH + TILE_OFFSET) / 2.0f) - TILE_HEIGHT / 2.0f,
        (m_GameMapManager->GetGameMap()->GetTileCountY() * (TILE_HEIGHT + TILE_OFFSET) / 2.0f) - (TILE_HEIGHT + TILE_OFFSET) / 2.0f
    };

    m_CameraController->GetCamera()->SetPosition(glm::vec3(mapMiddle, 0.0f));

    for (auto player : newGameData.Players)
        m_PlayerManager->AddPlayer(player);

#if defined(DEBUG)
    int i = 6;
    for (auto player : m_PlayerManager->GetAllPlayers())
    {
        auto tile = m_GameMapManager->GetGameMap()->GetTile(i, 5);
        tile->CreateUnitGroup(UnitGroupType::SWORDSMAN);
        tile->CreateUnitGroup(UnitGroupType::DWARF);
        tile->CreateUnitGroup(UnitGroupType::DEMON);
        tile->CreateBuilding(BuildingType::GOLD_MINE);
        player->AddOwnedTile(tile);
        i++;
    }
#endif
}

void GameLayer::ResetArrow()
{
    for(auto tile : m_PlayerManager->GetCurrentPlayer()->GetOwnedTiles())
    {
        tile->DeselectAllUnitGroups();
    }
    m_Arrow->SetVisible(false);
}

void GameLayer::EndGame()
{
    m_GameActive = false;
}

bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    if(!m_GameActive) return true;

    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();
    auto currentPlayer = m_PlayerManager->GetCurrentPlayer();

    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);

            if (!tile->InRange(relMousePos))
                continue;

            ProcessTileInRange(tile, currentPlayer, relMousePos);
            m_Arrow->SetVisible(m_Arrow->GetStartTile() && m_Arrow->GetStartTile()->HasSelectedUnitGroups());
            return true;
        }
    }

    // If no tile in range then deselect all unit groups
    if (m_Arrow->GetStartTile())
        m_Arrow->GetStartTile()->DeselectAllUnitGroups();

    m_Arrow->SetVisible(false);
    return true;
}

void GameLayer::ProcessTileInRange(const std::shared_ptr<Tile>& tile, const std::shared_ptr<Player>& currentPlayer, const glm::vec2& relMousePos)
{
    if (!m_Arrow->GetStartTile()) m_Arrow->SetStartTile(tile);

    if (m_Arrow->GetStartTile() != tile)
    {
        // If assets can't exits on selected tile unit groups are deselected
        if (!tile->AssetsCanExist())
        {
            m_Arrow->GetStartTile()->DeselectAllUnitGroups();
            return;
        }

        if (m_Arrow->GetStartTile()->HasSelectedUnitGroups())
        {
            // If tile is not adjacent unit groups can't be moved and they are deselected
            if (!Tile::IsAdjacent(m_Arrow->GetStartTile()->GetCoords(), tile->GetCoords()))
            {
                m_Arrow->GetStartTile()->DeselectAllUnitGroups();
                return;
            }

            // If the tile is owned by the moving player and it has no space for units then return
            if (tile->GetOwnedBy() == currentPlayer &&
               !tile->HasSpaceForUnitGroups(m_Arrow->GetStartTile()->GetNumSelectedUnitGroups()))
                return;

            // Otherwise move units to tile
            m_Arrow->GetStartTile()->MoveToTile(tile);
        }
        else if (tile->GetOwnedBy() == currentPlayer)
        {
            m_Arrow->SetStartTile(tile);
            tile->HandleUnitGroupMouseClick(relMousePos);
            tile->HandleBuildingUpgradeIconMouseClick(relMousePos);
        }
    } // If the tile is owned by the current player and a unit grup or unit group box has not been clicked then deselect
    else if (tile->GetOwnedBy() == currentPlayer &&
            !tile->HandleUnitGroupMouseClick(relMousePos) &&
            !tile->HandleBuildingUpgradeIconMouseClick(relMousePos) &&
            !tile->IsMouseClickedInsideUnitGroupsBox(relMousePos))
    {
        tile->DeselectAllUnitGroups();
    }
}
