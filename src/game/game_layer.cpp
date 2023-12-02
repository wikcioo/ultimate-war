#include "game_layer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/tile.h"
#include "debug/debug_data.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "util/util.h"

GameLayer* GameLayer::s_Instance = nullptr;

GameLayer::GameLayer()
    : Layer("GameLayer"), m_IterationNumber(0), m_GameActive(true), m_ShowEarnedResourcesInfo(false)
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
    auto camera = m_CameraController->GetCamera();

    Renderer2D::ClearColor({0.2f, 0.2f, 0.2f, 1.0f});

    Renderer2D::BeginScene(camera);

    auto currentPlayer = m_PlayerManager->GetCurrentPlayer();
    auto relMousePos = camera->CalculateRelativeMousePosition();
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
                    m_Arrow->SetVisible(true);

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
                else
                {
                    m_Arrow->SetVisible(false);
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

    if (m_Arrow->IsActivated() && m_Arrow->IsVisible())
        m_Arrow->Draw();

    if (notEnoughSpaceInfo.NotEnoughSpace)
    {
        Renderer2D::DrawTextStr(
            "Not enough space",
            notEnoughSpaceInfo.Position,
            1.0f / camera->GetZoom(),
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
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(GameLayer::OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(GameLayer::OnKeyReleased));
}

bool GameLayer::OnKeyPressed(KeyPressedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_TAB && Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        Application::Get().OpenMainMenu();
        return true;
    }

    if (m_GameActive && event.GetKeyCode() == GLFW_KEY_ENTER && event.GetRepeatCount() != 1)
    {
        m_PlayerManager->NextTurn();
        return true;
    }

    if (m_GameActive && event.GetKeyCode() == GLFW_KEY_LEFT_ALT)
    {
        m_ShowEarnedResourcesInfo = true;
        return true;
    }

    return false;
}

bool GameLayer::OnKeyReleased(KeyReleasedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_LEFT_ALT)
    {
        m_ShowEarnedResourcesInfo = false;
        return true;
    }

    return false;
}

void GameLayer::InitGame(NewGameDTO newGameData)
{
    if (newGameData.MapData.has_value())
        m_GameMapManager->Load(newGameData.MapName, newGameData.MapData.value());
    else
        m_GameMapManager->Load(newGameData.MapName);

    glm::vec2 mapMiddle = {
        (m_GameMapManager->GetGameMap()->GetTileCountX() * (3.0f / 4.0f * TILE_WIDTH + TILE_OFFSET) / 2.0f) - TILE_WIDTH / 2.0f,
        (m_GameMapManager->GetGameMap()->GetTileCountY() * (TILE_HEIGHT + TILE_OFFSET) / 2.0f) - (TILE_HEIGHT + TILE_OFFSET) / 4.0f
    };

    m_CameraController->GetCamera()->SetPosition(glm::vec3(mapMiddle, 0.0f));

    for (auto player : newGameData.Players)
    {
        auto _player = m_PlayerManager->AddPlayer(player);
        for (const auto& tileCoords : player.TileCoords)
        {
            auto tile = GameLayer::Get().GetGameMapManager()->GetGameMap()->GetTile(tileCoords.x, tileCoords.y);

            if (!newGameData.LoadedFromSave)
            {
                UnitGroup ug(UnitGroupType::SWORDSMAN);
                ug.SetMovedOnIteration(-1);
                tile->CreateUnitGroup(ug);
            }

            _player->AddOwnedTile(tile);
        }
    }

    if (!newGameData.LoadedFromSave)
    {
        for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
        {
            for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
            {
                auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);
                if (tile->AssetsCanExist() && !tile->IsOwned())
                    tile->AddRandomUnits();
            }
        }
    }
}

void GameLayer::ResetArrow()
{
    for (auto tile : m_PlayerManager->GetCurrentPlayer()->GetOwnedTiles())
    {
        tile->DeselectAllUnitGroups();
    }
    m_Arrow->SetActivated(false);
}

void GameLayer::EndGame()
{
    m_GameActive = false;
}

bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    if (!m_GameActive) return true;

    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();

    switch (event.GetMouseButton())
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            auto currentPlayer = m_PlayerManager->GetCurrentPlayer();

            for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
            {
                for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
                {
                    auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);

                    if (!tile->InRange(relMousePos))
                        continue;

                    ProcessTileInRange(tile, currentPlayer, relMousePos);
                    m_Arrow->SetActivated(m_Arrow->GetStartTile() && m_Arrow->GetStartTile()->HasSelectedUnitGroups());
                    return true;
                }
            }

            // If no tile in range then deselect all unit groups
            if (m_Arrow->GetStartTile())
                m_Arrow->GetStartTile()->DeselectAllUnitGroups();

            m_Arrow->SetActivated(false);
            return true;
        }
        default:
            return false;
    }

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
